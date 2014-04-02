#include <filesystem>
#include <utility>
#include <algorithm>
#include <functional> 
#include <cctype>
#include <locale>
#include <vector>
#include <map>
#include <prs/parser.h>
#include "common/log.h"

using namespace prs;

std::tr2::sys::path g_kernelRoot;

struct ast_string
{
   std::string value;

   template<typename Result>
   void construct(Result &&result)
   {
      value = ast_to_string<decltype(value)>(result);
   }
};

struct ast_number
{
   int value;

   template<typename Result>
   void construct(Result &&result)
   {
      value = std::stoi(ast_to_string<std::string>(result));
   }
};

struct ast_export
{
   std::string name;
   int ordinal;

   template<typename Result>
   void construct(Result &&result)
   {
      name = std::get<0>(result).value;
      ordinal = std::get<2>(result).value;
   }
};

struct ast_libdef
{
   std::string name;
   std::map<std::string, int> exports;

   template<typename Result>
   void construct(Result &&result)
   {
      name = std::get<1>(result).value;

      for (auto &exp : std::get<3>(result)) {
         exports.insert(std::make_pair(exp.name, exp.ordinal));
      }
   }
};

auto whitespace = atomic(*(char_(' ') | char_('\t') | char_('\n') | char_('\r')));

auto letter     = char_range('a', 'z') | char_range('A', 'Z');
auto digit      = char_range('0', '9');

auto number     = ast<ast_number>() >> (atomic(+digit));
auto symbol     = ast<ast_string>() >> (atomic((letter | char_('_')) >> *(letter | digit | char_('_'))));

auto lib_name   = ast<ast_string>() >> (atomic(*(letter | digit | char_('.') | char_('_'))));

auto export_def = ast<ast_export>() >> (symbol >> char_('@') >> number);

auto def_parser = ast<ast_libdef>() >> (string_("LIBRARY") >> lib_name >> string_("EXPORTS") >> *export_def);

bool parseDef(std::tr2::sys::path path, ast_libdef &ast)
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

   if (!def_parser.parse(pos, file.end(), ctx, ast) || pos != file.end()) {
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

struct Export
{
   bool isFunction;
   std::string name;
   int ordinal;
   int argCount;
};

static inline std::string ltrim(std::string s)
{
   s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
   return s;
}

static inline std::string rtrim(std::string s)
{
   s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
   return s;
}

static inline std::string trim(std::string s)
{
   return ltrim(rtrim(s));
}

bool parseHeader(std::string path, ast_libdef &libdef, std::vector<Export> &exports)
{
   std::ifstream in;
   bool mlc = false;
   std::string::size_type mlcStart;
   std::string decl;
   std::vector<std::string> decls;
   bool inDecl = false;

   in.open(path, std::ifstream::in);

   if (!in.is_open()) {
      xDebug() << "Could not open " << path << " for reading";
      return false;
   }

   for (std::string line; std::getline(in, line);) {
      /* Single Line Comment */
      auto p = line.rfind("//");

      if (p != std::string::npos) {
         line.erase(line.begin() + p, line.end());
      }

      /* Multi-line Comment */
      p = line.find("/*");

      if (p != std::string::npos) {
         mlc = true;
         mlcStart = p;
      }
      
      if (mlc) {
         p = line.find("*/");

         if (p != std::string::npos) {
            mlc = false;

            if (mlcStart == -1) {
               line.erase(line.begin(), line.begin() + p + 2);
            } else {
               line.erase(line.begin() + mlcStart, line.begin() + p + 2);
            }
         } else {
            if (mlcStart == -1) {
               line = std::string();
            } else {
               line.erase(line.begin() + mlcStart, line.end());
            }
         }
      }

      mlcStart = -1;

      /* Trim whitespace */
      line = trim(line);

      if (!line.size()) {
         continue;
      }

      /* Define */
      if (line.at(0) == '#') {
         continue;
      }

      /* Export Declaration */
      if (line.find("XBXKRNL") == 0) {
         inDecl = true;
      }

      if (inDecl) {
         p = line.find(';');

         if (p != std::string::npos) {
            line.erase(line.begin() + p, line.end());
            inDecl = false;
         }

         decl.append(line);

         if (inDecl) {
            decl += " ";
         }

         if (!inDecl) {
            decls.push_back(decl);
            decl.clear();
         }
      }
   }

   for (auto &decl : decls) {
      Export exp;

      auto argStart = decl.find('(');
      auto argEnd = decl.find(')');

      if (argStart == std::string::npos || argEnd == std::string::npos) {
         /* Export Variable */
         exp.isFunction = false;
         exp.argCount = 0;

         auto vNameStart = std::find_if(decl.rbegin(), decl.rend(), std::ptr_fun<int, int>(std::isspace));
         auto vNameEnd = decl.end();

         if (vNameStart == decl.rend()) {
            xDebug() << "Expected to find a variable name in declaration " << decl;
            continue;
         }

         auto name = std::string(decl.begin() + (decl.rend() - vNameStart), vNameEnd);
         exp.name = name;
      } else {
         /* Export Function */
         exp.isFunction = true;
         
         auto fNameStart = std::find_if(decl.rbegin() + decl.size() - argStart, decl.rend(), std::ptr_fun<int, int>(std::isspace));
         auto fNameEnd = argStart;

         if (fNameStart == decl.rend()) {
            xDebug() << "Expected to find a function name in declaration " << decl;
            continue;
         }

         auto name = std::string(decl.begin() + (decl.rend() - fNameStart), decl.begin() + fNameEnd);
         auto args = trim(std::string(decl.begin() + argStart + 1, decl.begin() + argEnd));

         exp.name = name;

         if (args.length() == 0) {
            exp.argCount = 0;
         } else {
            exp.argCount = 1 + static_cast<int>(std::count(args.begin(), args.end(), ','));
         }
      }

      /* Find ordinal */
      auto itr = libdef.exports.find(exp.name);

      if (itr == libdef.exports.end()) {
         xDebug() << "Could not find ordinal for export " << exp.name;
      } else {
         exp.ordinal = itr->second;
      }

      exports.push_back(exp);
   }

   in.close();
   return true;
}

bool dumpExports(std::tr2::sys::path path, std::vector<Export> &exports)
{
   std::ofstream out;

   out.open(path.string(), std::ofstream::out);

   if (!out.is_open()) {
      xError() << "Could not open " << path << " for writing";
      return false;
   }

   for (auto &exp : exports) {
      out << exp.ordinal << " : " << exp.name;

      if (exp.isFunction) {
         out << " : " << exp.argCount;
      }

      out << std::endl;
   }

   out.close();
   return true;
}

std::vector<std::tr2::sys::path> findFilesByType(std::tr2::sys::path root, const std::string& type)
{
   std::vector<std::tr2::sys::path> result;

   for (auto itr = std::tr2::sys::recursive_directory_iterator(root); itr != std::tr2::sys::recursive_directory_iterator(); ++itr) {
      if (std::tr2::sys::is_regular_file(itr->status()) && itr->path().extension().compare(type) == 0) {
         result.push_back(itr->path());
      }
   }

   return result;
}

bool checkKernelDirectory(std::tr2::sys::path root)
{
   std::vector<Export> exports;
   ast_libdef libdef;

   auto def = findFilesByType(root, ".def");

   if (def.size() != 1) {
      xDebug() << "Expected one .def file in " << root.string();
      return false;
   }

   if (!parseDef(def.at(0), libdef)) {
      xDebug() << "Error parsing " << def.at(0).string();
      return false;
   }

   auto headers = findFilesByType(root, ".h");

   for (auto &header : headers) {
      if (!parseHeader(header, libdef, exports)) {
         xDebug() << "Error parsing " << header;
      }
   }

   if (!dumpExports(root / std::tr2::sys::path(root.leaf() + ".krn"), exports)) {
      xDebug() << "Error saving exports for " << root.string();
      return false;
   }

   return true;
}

int main(int argc, char** argv)
{
   if (argc < 2) {
      xDebug() << argv[0] << "<base kernel dir>";
      return 0;
   }

   g_kernelRoot = argv[1];

   for (auto itr = std::tr2::sys::directory_iterator(g_kernelRoot); itr != std::tr2::sys::directory_iterator(); ++itr) {
      if (std::tr2::sys::is_directory(itr->path())) {
         checkKernelDirectory(itr->path());
      }
   }

   return 0;
}
