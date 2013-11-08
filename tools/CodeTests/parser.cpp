#include "parser.h"
#include "util/log.h"

#include <fstream>

namespace parser
{
#include "parser/parser.h"

auto whitespace  = atomic(*(char_(' ') | char_('\t') | char_('\n') | char_('\r')));
auto letter      = char_range('a', 'z') | char_range('A', 'Z');
auto digit       = char_range('0', '9');

auto number      = ast<ast_number>() >> atomic(-char_('-') >> *digit);
auto hex         = ast<ast_number>() >> atomic(-char_('-') >> string_("0x") >> +(digit | char_range('a', 'f') | char_range('A', 'F')));

auto reg_name    = ast<ast_string>() >> atomic(char_('%') >> +(letter | digit | char_('[') | char_(']')));
auto reg_value   = hex | number;

auto mem_address = char_('[') >> hex >> char_(']');

auto reg_set     = ast<ast_reg_set>() >> (reg_name >> char_('=') >> (mem_address | reg_value));
auto reg_check   = ast<ast_reg_check>() >> (reg_name >> string_("==") >> (mem_address | reg_value));

auto mem_set     = ast<ast_mem_set>() >> (mem_address >> char_('=') >> reg_value);

auto pre_condition  = ast<ast_precondition>() >> (char_('#') >> string_("pre") >> char_(':') >> (reg_set | mem_set));
auto post_condition = ast<ast_postcondition>() >> (char_('#') >> string_("post") >> char_(':') >> reg_check);

auto test_parser    = ast<ast_test>() >> +(pre_condition | post_condition);

bool parseTest(const std::string &path, ast_test &result)
{
   std::fstream in;
   std::string file;

   in.open(path, std::ifstream::in);

   if (!in.is_open()) {
      xDebug() << "Could not open " << path << " for reading";
      return false;
   }

   /* Read only lines beginning with # */
   for (std::string line; std::getline(in, line);) {
      if (line.size() && line.at(0) == '#') {
         file.append(line.begin(), line.end());
         file += '\n';
      }
   }

   in.close();

   /* Parse file */
   struct ParseContext
   {
      decltype(whitespace) ws;
   } ctx = { whitespace };

   auto pos = file.begin();

   if (!test_parser.parse(pos, file.end(), ctx, result) || pos != file.end()) {
      decltype(pos) lineStart, lineEnd;
      auto error = pos;

      for (lineStart = pos; lineStart != file.begin() && *lineStart != '\n'; --lineStart);
      for (lineEnd = pos; lineEnd != file.end() && *lineEnd != '\n'; ++lineEnd);

      xDebug() << "Syntax Error";
      std::cout << std::endl;
      std::cout << std::string(lineStart, lineEnd) << std::endl;
      std::cout << std::string((size_t)(pos - lineStart), ' ') << '^' << std::endl;

      return false;
   }

   return true;
}

}; // namespace parser
