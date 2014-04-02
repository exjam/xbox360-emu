#include "system.h"

#include <ppc/interpreter.h>
#include <ppc/instructions.h>
#include <util/memory.h>
#include <prs/parser.h>
#include <kernel.h>

#include <Windows.h>
#include <fstream>
#include <map>

using namespace prs;

struct ast_number
{
   int value;

   template<typename Result>
   void construct(Result &&result)
   {
      auto str = ast_to_string<std::string>(result);

      if (str.size()) {
         value = std::stoi(str);
      } else {
         value = 0;
      }
   }
};

struct ast_symbol
{
   std::string value;

   template<typename Result>
   void construct(Result &&result)
   {
      value = ast_to_string<decltype(value)>(result);
   }
};

struct ast_decl
{
   int ordinal;
   std::string name;
   int argCount;

   template<typename Result>
   void construct(Result &&result)
   {
      ordinal = std::get<0>(result).value;
      name = std::get<2>(result).value;

      if (std::get<3>(result)) {
         argCount = std::get<1>(*std::get<3>(result)).value;
      } else {
         argCount = -1;
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

   template<typename Result>
   void construct(Result &&result)
   {
      for (auto &decl : result) {
         Declaration d;
         d.name = decl.name;
         d.args = decl.argCount;
         decls.insert(std::make_pair(decl.ordinal, d));
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

static bool parseKernelDeclarations(std::string path, ast_krn &krn)
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
      decltype(whitespace) whitespace_parser;
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

typedef void (*fpEmuInitKernel)(IKernel *kernel);

bool System::loadLibrary(xex::ImportLibrary &library)
{
   std::string path;
   HMODULE handle;
   ast_krn krn;

   path = "kernel/" + library.name;
   handle = LoadLibraryA(path.c_str());
   library.handle = reinterpret_cast<xex::LibraryHandle>(handle);

   if (handle) {
      fpEmuInitKernel proc = reinterpret_cast<fpEmuInitKernel>(GetProcAddress(handle, "EmuInitKernel"));

      if (proc) {
         (*proc)(this);
      }
   }

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

bool System::loadImport(xex::ImportLibrary &library, xex::ImportLibrary::Import &import)
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

bool System::resolveImports(xex::ImportLibraries &imports)
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
               auto instr = ppc::createInstruction(ppc::InstructionID::krncall);
               instr.uimm = import.ordinal;
               instr.rA = import.args;
               instr.rD = 1;

               Memory::write<uint32_t>(import.thunk,
                                       instr.value);

               Memory::write<uint64_t>(import.thunk + 4,
                                       reinterpret_cast<uint64_t>(import.handle));
            } else {
               /* Import Variable */
               Memory::write<uint32_t>(import.address,
                                       reinterpret_cast<uint32_t>(import.handle));
            }
         } else {
            if (import.thunk) {
               auto instr = ppc::createInstruction(ppc::InstructionID::krncall);
               instr.uimm = import.ordinal;
               instr.rA = instr.rD = 0;

               Memory::write<uint32_t>(import.thunk, instr.value);

               Memory::write<uint64_t>(import.thunk + 4,
                                       reinterpret_cast<uint64_t>(&lib));
            }
         }
      }
   }

   return true;
}
