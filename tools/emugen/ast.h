#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <prs/optional.h>

struct ast_symbol
{
   std::string value;

   template<typename Result>
   void construct(Result &&result)
   {
      value = ast_to_string<decltype(value)>(result);
   }
};

struct ast_string
{
   std::string value;

   template<typename Result>
   void construct(Result &&result)
   {
      value = ast_to_string<decltype(value)>(std::get<1>(result));
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

struct ast_char
{
   int value;

   template<typename Result>
   void construct(Result &&result)
   {
      value = std::get<1>(result);
   }
};

struct ast_bitrange
{
   ast_bitrange() : end(-1)
   {
   }

   int start;
   int end;

   int size()
   {
      return 1 + end - start;
   }

   template<typename Result>
   void construct(Result &&result)
   {
      start = std::get<0>(result).value;

      if (std::get<1>(result)) {
         end = std::get<1>(*std::get<1>(result)).value;
      } else {
         end = start;
      }
   }
};

struct ast_cat_opcd
{
   std::string primary;
   std::vector<std::pair<int, std::string>> secondary;

   template<typename Result>
   void construct(Result &&result)
   {
      primary = std::get<1>(result).value;

      for (auto &sub : std::get<2>(result)) {
         secondary.push_back(std::make_pair(std::get<1>(sub).value, std::get<3>(sub).value));
      }
   }
};

struct ast_arch_endian
{
   std::string endian;

   template<typename Sequence>
   void construct(Sequence &&xs)
   {
      endian = std::get<2>(xs);
   }
};

struct ast_bitfield
{
   std::vector<std::pair<std::string, ast_bitrange>> bitfield;

   template<typename Result>
   void construct(Result &&result)
   {
      for (auto &fields : std::get<1>(result)) {
         bitfield.push_back(std::make_pair(std::get<0>(fields).value, std::get<2>(fields)));
      }
   }
};

struct ast_array
{
   int size;

   template<typename Result>
   void construct(Result &&result)
   {
      size = std::get<1>(result).value;
   }
};

struct ast_reg_type
{
   std::string type;
   prs::optional<ast_array> array;
   prs::optional<ast_bitfield> bitfield;

   template<typename Result>
   void construct(Result &&result)
   {
      type = std::get<0>(result).value;

      if (type.find("int") != std::string::npos) {
         type.append("_t");
      }

      auto &extra = std::get<1>(result);

      if (extra) {
         if (extra->which == 0) {
            array = std::get<0>(*extra);
         }

         if (extra->which == 1) {
            bitfield = std::get<1>(*extra);
         }
      }
   }
};

struct ast_reg_define
{
   std::string name;
   ast_reg_type type;

   template<typename Result>
   void construct(Result &&result)
   {
      name = std::get<0>(result).value;
      type = std::get<2>(result);
   }
};

struct ast_insf_extra
{
   std::string name;
   prs::optional<int> number;
   prs::optional<std::string> string;
   prs::optional<char> chr;

   template<typename Result>
   void construct(Result &&result)
   {
      name = std::get<0>(result).value;

      auto &value = std::get<2>(result);

      switch (value.which) {
      case 0:
         number = std::get<0>(value)->value;
         break;
      case 1:
         string = std::get<1>(value)->value;
         break;
      case 2:
         chr = std::get<2>(value)->value;
         break;
      }
   }
};

struct ast_insf_field
{
   std::string name;
   ast_bitrange bitrange;
   std::map<std::string, ast_insf_extra> extras;

   template<typename Result>
   void construct(Result &&result)
   {
      name = std::get<0>(result).value;
      bitrange = std::get<2>(result);

      if (std::get<3>(result)) {
         auto &extra = *std::get<3>(result);

         extras[std::get<1>(extra).name] = std::get<1>(extra);

         for (auto &ext : std::get<2>(extra)) {
            extras[std::get<1>(ext).name] = std::get<1>(ext);
         }
      }
   }
};

struct ast_opcd_disasm_reg
{
   char prefix;
   std::string name;

   template<typename Result>
   void construct(Result &&result)
   {
      if (std::get<0>(result)) {
         prefix = *std::get<0>(result);
      } else {
         prefix = 0;
      }

      name = std::get<1>(result).value;
   }
};

struct ast_opcd_disasm
{
   std::string name;
   std::vector<ast_opcd_disasm_reg> operands;

   template<typename Result>
   void construct(Result &&result)
   {
      name = std::get<0>(result).value;

      if (std::get<1>(result)) {
         auto &regs = *std::get<1>(result);

         operands.push_back(std::get<0>(regs));

         for (auto &reg : std::get<1>(regs)) {
            if (std::get<0>(reg) == '(') {
               std::get<1>(reg).prefix = '(';
            }

            operands.push_back(std::get<1>(reg));
         }
      }
   }
};

struct ast_opcd_extra
{
   std::string name;
   prs::optional<int> number;
   prs::optional<std::string> string;
   prs::optional<char> chr;

   template<typename Result>
   void construct(Result &&result)
   {
      name = std::get<0>(result).value;

      auto &extra = std::get<1>(result);

      if (extra) {
         auto &value = std::get<1>(*extra);

         switch (value.which) {
         case 0:
            number = std::get<0>(value)->value;
            break;
         case 1:
            string = std::get<1>(value)->value;
            break;
         case 2:
            chr = std::get<2>(value)->value;
            break;
         }
      }
   }
};

struct ast_opcd_def
{
   ast_cat_opcd *category;

   int id;
   ast_opcd_disasm disasm;
   std::map<std::string, ast_opcd_extra> extras;

   std::size_t fieldsLen;

   template<typename Result>
   void construct(Result &&result)
   {
      category = nullptr;

      id = std::get<0>(result).value;
      disasm = std::get<2>(result);

      if (std::get<3>(result)) {
         auto &ext = *std::get<3>(result);

         extras[std::get<1>(ext).name] = std::get<1>(ext);

         for (auto &ex : std::get<2>(ext)) {
            extras[std::get<1>(ex).name] = std::get<1>(ex);
         }
      }
   }
};

struct ast_arch
{
   enum Endian
   {
      Invalid,
      BigEndian,
      LittleEndian
   };

   Endian endian;

   template<typename Result>
   void construct(Result &&result)
   {
      if (result.endian.compare("BigEndian") == 0) {
         endian = BigEndian;
      } else if (result.endian.compare("LittleEndian") == 0) {
         endian = LittleEndian;
      } else {
         endian = Invalid;
      }
   }
};

struct ast_reg
{
   std::vector<ast_reg_define> registers;

   template<typename Result>
   void construct(Result &&result)
   {
      registers = result;
   }
};

struct ast_insf
{
   std::vector<ast_insf_field> fields;

   template<typename Result>
   void construct(Result &&result)
   {
      fields = result;
   }
};

struct ast_opcd
{
   ast_cat_opcd declaration;
   std::vector<ast_opcd_def> opcodes;

   template<typename Result>
   void construct(Result &&result)
   {
      declaration = std::get<0>(result);
      opcodes = std::get<1>(result);
   }
};

struct ast_root
{
   ast_arch arch;
   ast_reg reg;
   ast_insf insf;
   std::vector<ast_opcd> opcodes;

   template<typename Result>
   void construct(Result &&result)
   {
      for (auto &value : result) {
         switch (value.which) {
         case 0:
            arch = std::get<1>(*std::get<0>(value));
            break;
         case 1:
            reg = std::get<1>(*std::get<1>(value));
            break;
         case 2:
            insf = std::get<1>(*std::get<2>(value));
            break;
         case 3:
            opcodes.push_back(*std::get<3>(value));
            break;
         };
      }
   }
};

#endif // ifdef AST_H
