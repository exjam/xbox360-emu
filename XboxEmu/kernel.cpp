#include "ppc/interpreter.h"
#include "ppc/instructions.h"
#include "util/be/memory.h"
#include "parser/parser.h"

#include <Windows.h>
#include <fstream>
#include <map>

struct ast_number
{
   ast_number() : value(0)
   {
   }

   ast_number(int value) : value(value)
   {
   }

   int value;

   template<typename Char>
   void construct(std::vector<Char>& chs)
   {
      std::string word;

      for (auto ch : chs) {
         word.push_back(ch);
      }

      if (word.size()) {
         value = std::stoi(word);
      }
   }
};

struct ast_symbol
{
   std::string value;

   template<typename Type>
   void construct(Type& input)
   {
      value = ast_to_string<decltype(value)>(input);
   }
};

struct ast_decl
{
   ast_number ordinal;
   ast_symbol name;
   ast_number argCount;

   template<typename Type>
   void construct(Type& input)
   {
      ordinal = input.first.first.first;
      name = input.first.second;

      if (input.second) {
         argCount = input.second->second;
      } else {
         argCount.value = -1;
      }
   }
};

struct ast_krn
{
   struct Declaration
   {
      std::string name;
      int args;
   };

   std::map<int, Declaration> decls;

   void construct(std::vector<ast_decl>& input)
   {
      for (auto &decl : input) {
         Declaration d;
         d.name = decl.name.value;
         d.args = decl.argCount.value;
         decls.insert(std::make_pair(decl.ordinal.value, d));
      }
   }
};

auto whitespace = atomic(*(char_(' ') | char_('\t') | char_('\n') | char_('\r')));

auto letter = char_range('a', 'z') | char_range('A', 'Z');
auto digit  = char_range('0', '9');

auto number = ast<ast_number>() >> atomic(*digit);
auto symbol = ast<ast_symbol>() >> atomic((letter | char_('_')) >> *(letter | digit | char_('_')));

auto decl   = ast<ast_decl>()   >> (number >> char_(':')  >> symbol >> -(char_(':') >> number));

auto krn_parser = ast<ast_krn>() >> *decl;

bool parseKernelDeclarations(std::string path, ast_krn &krn)
{
   std::fstream in;
   std::string file;

   in.open(path, std::ifstream::in);

   if (!in.is_open()) {
      xDebug() << "Could not open " << path << " for reading";
      return false;
   }

   /* Read whole file */
   for (std::string line; std::getline(in, line);) {
      file.append(line.begin(), line.end());
      file += '\n';
   }

   in.close();

   /* Parse file */
   struct ParseContext
   {
      decltype(whitespace) ws;
   } ctx = { whitespace };

   auto pos = file.begin();

   if (!krn_parser.parse(pos, file.end(), ctx, krn) || pos != file.end()) {
      decltype(pos) lineStart, lineEnd;
      auto error = pos;

      for (lineStart = pos; lineStart != file.begin() && *lineStart != '\n'; --lineStart);
      for (lineEnd = pos; lineEnd != file.end() && *lineEnd != '\n'; ++lineEnd);

      xDebug() << "Syntax Error";
      xDebug() << std::string(lineStart, lineEnd);
      xDebug() << std::string((size_t)(pos - lineStart), ' ') << '^';

      return false;
   }

   return true;
}

/*
PowerPC calling convention:
r3....rn
return r3

x64 calling convention:
rcx, rdx, r8, r9, stack ...
return rax
*/
bool loadLibrary(xex::ImportLibrary &library)
{
   std::string path;
   HMODULE handle;
   ast_krn krn;

   path = "kernel/" + library.name;
   handle = LoadLibraryA(path.c_str());
   library.handle = reinterpret_cast<xex::LibraryHandle>(handle);

   path += ".krn";

   if (!parseKernelDeclarations(path, krn)) {
      xDebug() << "Error parsing " << path;
   } else {
      for (auto &import : library.imports) {
         auto itr = krn.decls.find(import.ordinal);

         if (itr == krn.decls.end()) {
            xDebug() << "Ordinal " << import.ordinal << " not declared in " << path;
         } else {
            import.args = itr->second.args;
            import.name = itr->second.name;

            if (import.args == -1) {
               xDebug() << "Ordinal " << import.ordinal << " resolved to variable " << import.name;
            } else {
               xDebug() << "Ordinal " << import.ordinal << " resolved to function " << import.name << "(" << import.args << ")";
            }
         }
      }
   }

   return handle != nullptr;
}

bool loadImport(xex::ImportLibrary &library, xex::ImportLibrary::Import &import)
{
   FARPROC handle;

   if (!library.handle) {
      return false;
   }

   handle = GetProcAddress(reinterpret_cast<HMODULE>(library.handle),
                           reinterpret_cast<LPCSTR>(import.ordinal));

   import.handle = reinterpret_cast<xex::ImportHandle>(handle);

   return handle != nullptr;
}

bool processImports(xex::ImportLibraries &imports)
{
   for (auto &lib : imports.libraries) {
      if (!loadLibrary(lib)) {
         xDebug() << "Could not load library " << lib.name;
         continue;
      }

      for (auto &import : lib.imports) {
         if (loadImport(lib, import)) {
            if (import.thunk) {
               /* Import Function */
               ppc::Instruction instr = { 0 };
               instr.opcode = ppc::op::krncall;
               instr.uimm = import.ordinal;
               instr.rA = import.args;
               instr.rD = 1;

               be::Memory::write<uint32_t>(import.thunk,
                                           instr.value);

               be::Memory::write<uint64_t>(import.thunk + 4,
                                           reinterpret_cast<uint64_t>(import.handle));
            } else {
               /* Import Variable */
               be::Memory::write<uint32_t>(import.address,
                                           reinterpret_cast<uint32_t>(import.handle));
            }
         } else {
            if (import.thunk) {
               ppc::Instruction instr = { 0 };
               instr.opcode = ppc::op::krncall;
               instr.uimm = import.ordinal;
               instr.rA = instr.rD = 0;

               be::Memory::write<uint32_t>(import.thunk, instr.value);

               be::Memory::write<uint64_t>(import.thunk + 4,
                                           reinterpret_cast<uint64_t>(&lib));
            }
         }
      }
   }
   return true;
}

bool loadBinary(xex::Binary &binary)
{
   processImports(binary.header.importLibraries);
   return true;
}
