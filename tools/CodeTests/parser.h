#ifndef TEST_PARSER_H
#define TEST_PARSER_H

#include <stdint.h>
#include <string>
#include <vector>
#include "parser/optional.h"

namespace parser
{

struct ast_string
{
   std::string value;

   template<typename Type>
   void construct(Type &input)
   {
      value = ast_to_string<std::string>(input);
   }
};

struct ast_number
{
   int64_t value;

   template<typename Type>
   void construct(Type &input)
   {
      auto str_value = ast_to_string<std::string>(input);

      if (str_value.size() >= 18) {
         value = std::stoull(str_value, 0, 0);
      } else if (str_value.size() > 0) {
         value = std::stoll(str_value, 0, 0);
      } else {
         value = 0;
      }
   }
};

struct ast_reg_set
{
   ast_string reg_name;
   std::optional<ast_number> reg_value;
   std::optional<ast_number> mem_addr;

   template<typename Type>
   void construct(Type &input)
   {
      reg_name = input.first.first;

      if (input.second.first) {
         mem_addr = input.second.first->first.second;
      } else if (input.second.second) {
         reg_value = input.second.second;
      }
   }
};

struct ast_mem_set
{
   ast_number mem_addr;
   ast_number mem_value;

   template<typename Type>
   void construct(Type &input)
   {
      mem_addr = input.first.first.first.second;
      mem_value = input.second;
   }
};

struct ast_reg_check
{
   ast_string reg_name;
   std::optional<ast_number> reg_value;
   std::optional<ast_number> mem_addr;

   template<typename Type>
   void construct(Type &input)
   {
      reg_name = input.first.first;

      if (input.second.first) {
         mem_addr = input.second.first->first.second;
      } else if (input.second.second) {
         reg_value = input.second.second;
      }
   }
};

struct ast_precondition
{
   std::optional<ast_reg_set> reg_set;
   std::optional<ast_mem_set> mem_set;

   template<typename Type>
   void construct(Type &input)
   {
      if (input.second.first) {
         reg_set = input.second.first;
      } else {
         mem_set = input.second.second;
      }
   }
};

struct ast_postcondition
{
   std::optional<ast_reg_check> reg_check;

   template<typename Type>
   void construct(Type &input)
   {
      reg_check = input.second;
   }
};

struct ast_test
{
   std::vector<ast_precondition> pre;
   std::vector<ast_postcondition> post;

   template<typename Type>
   void construct(Type &input)
   {
      for (auto &v : input) {
         if (v.first) {
            pre.push_back(*v.first);
         } else {
            post.push_back(*v.second);
         }
      }
   }
};

bool parseTest(const std::string &path, ast_test &result);

}; // namespace parser

#endif // ifndef TEST_PARSER_H
