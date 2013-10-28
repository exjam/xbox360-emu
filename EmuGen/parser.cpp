#include "parser/parser.h"
#include "emugen.h"
#include "ast.h"

#include <fstream>
#include <iostream>

/* Character Types */
auto whitespace = atomic(*(char_(' ') | char_('\t') | char_('\n') | char_('\r')));
auto letter     = char_range('a', 'z') | char_range('A', 'Z');
auto digit      = char_range('0', '9');

/* Data Types */
auto number     = ast<ast_number>()   >> atomic(+digit);
auto bitrange   = ast<ast_bitrange>() >> atomic(number >> -(string_("...") >> number));
auto chr        = ast<ast_char>()     >> atomic(char_('\'') >> char_any() >> char_('\''));
auto str        = ast<ast_string>()   >> atomic(char_('"') >> +(!char_('"') >> char_any()) >> char_('"'));
auto symbol     = ast<ast_symbol>()   >> atomic((letter | char_('_')) >> *(letter | digit | char_('_')));
auto name       = ast<ast_symbol>()   >> atomic((letter | char_('_')) >> *(letter | digit | char_('_') | char_('.')));

/* Register Types */
auto type_intb     = ast<ast_symbol>()      >> atomic(-char_('u') >> string_("int") >> +digit);
auto type_double   = ast<ast_symbol>()      >> string_("double");
auto type_array    = ast<ast_array>()       >> (char_('[') >> number >> char_(']'));
auto type_bitfield = ast<ast_bitfield>()    >> (char_('{') >> +(symbol >> char_('=') >> bitrange) >> char_('}'));

/* Architecture Specification */
auto cat_arch      = string_("[Architecture]");
auto arch_endian   = ast<ast_arch_endian>() >> (string_("Endian") >> char_('=') >> (string_("BigEndian") | string_("LittleEndian")));
auto arch_root     = ast<ast_arch>()        >> arch_endian;

/* Register Specification */
auto cat_reg       = string_("[Registers]");
auto reg_type      = ast<ast_reg_type>()    >> ((type_intb | type_double) >> -(type_array | type_bitfield));
auto reg_define    = ast<ast_reg_define>()  >> (symbol >> char_('=') >> reg_type);
auto reg_root      = ast<ast_reg>()         >> *reg_define;

/* Instruction Format */
auto cat_insf      = string_("[Instruction Format]");
auto insf_extra    = ast<ast_insf_extra>()  >> (symbol >> char_('=') >> (number | str | chr));
auto insf_field    = ast<ast_insf_field>()  >> (symbol >> char_('=') >> bitrange >> -(char_(':') >> insf_extra >> *(char_(',') >> insf_extra)));
auto insf_root     = ast<ast_insf>()        >> *insf_field;

/* Opcode Definition */
auto cat_opcd      = ast<ast_cat_opcd>()    >> (char_('[') >> symbol >> *(char_('=') >> number >> char_(',') >> symbol) >> char_(']'));
auto opcd_extra    = ast<ast_opcd_extra>()  >> (symbol >> -(char_('=') >> (number | str | chr)));
auto opcd_disasm   = ast<ast_opcd_disasm>() >> (name >> -(symbol >> *((char_(',') | char_('(')) >> symbol >> -char_(')'))));
auto opcd_def      = ast<ast_opcd_def>()    >> (number >> char_(':') >> opcd_disasm >> -(char_(':') >> opcd_extra >> *(char_(',') >> opcd_extra)));
auto opcd_root     = ast<ast_opcd>()        >> (cat_opcd >> *opcd_def);

/* The root parser rule */
auto parser_root   = ast<ast_root>() >> +((cat_arch >> arch_root) | (cat_reg >> reg_root) | (cat_insf >> insf_root) | opcd_root);

bool EmuGen::parseFile(const std::string &path)
{
   std::fstream in;
   std::string file;

   in.open(path, std::ifstream::in);

   if (!in.is_open()) {
      std::cout << "Could not open " << path << " for reading" << std::endl;
      return false;
   }

   /* Read file and remove all comments */
   for (std::string line; std::getline(in, line); ) {
      auto p = line.rfind("//");

      if (p != std::string::npos) {
         file.append(line.begin(), line.begin() + p);
      } else {
         file.append(line.begin(), line.end());
      }

      file += '\n';
   }

   in.close();

   /* Parse file */
   struct ParseContext {
      decltype(whitespace) ws;
   } ctx = { whitespace };

   auto pos = file.begin();

   if (!parser_root.parse(pos, file.end(), ctx, m_ast) || pos != file.end()) {
      decltype(pos) lineStart, lineEnd;
      auto error = pos;

      for (lineStart = pos; lineStart != file.begin() && *lineStart != '\n'; --lineStart);
      for (lineEnd = pos; lineEnd != file.end() && *lineEnd != '\n'; ++lineEnd);

      std::cout << "Syntax Error" << std::endl;
      std::cout << std::string(lineStart, lineEnd) << std::endl;
      std::cout << std::string((size_t)(pos - lineStart), ' ') << '^' << std::endl;

      return false;
   }

   return true;
}

template<typename Iterator>
static inline std::pair<Iterator, Iterator> getLine(Iterator begin, Iterator pos, Iterator end)
{
   typename Iterator lineStart, lineEnd;

   for (lineStart = pos; lineStart != begin && *lineStart != '\n'; --lineStart);
   for (lineEnd = pos; lineEnd != end && *lineEnd != '\n'; ++lineEnd);

   return std::make_pair(++lineStart, lineEnd);
}
