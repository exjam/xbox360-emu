#ifndef TEST_PARSER_H
#define TEST_PARSER_H

#include <stdint.h>
#include <string>
#include <vector>
#include <prs/optional.h>

struct ast_string
{
   std::string value;

   template<typename Result>
   void construct(Result &&result)
   {
      value = ast_to_string<std::string>(result);
   }
};

struct ast_number
{
   int64_t value;

   template<typename Result>
   void construct(Result &&result)
   {
      auto str_value = ast_to_string<std::string>(result);

      if (str_value.size() >= 18) {
         value = std::stoull(str_value, 0, 0);
      } else if (str_value.size() > 0) {
         value = std::stoll(str_value, 0, 0);
      } else {
         value = 0;
      }
   }
};

struct ast_mem_address
{
   int64_t value;

   template<typename Result>
   void construct(Result &&result)
   {
      value = std::get<1>(result).value;
   }
};

struct ast_reg_set
{
   std::string reg_name;
   prs::optional<int64_t> reg_value;
   prs::optional<int64_t> mem_addr;

   template<typename Result>
   void construct(Result &&result)
   {
      reg_name = std::get<0>(result).value;

      auto &value = std::get<2>(result);

      switch (value.which) {
      case 0:
         mem_addr = std::get<0>(value)->value;
         break;
      case 1:
         reg_value = std::get<1>(value)->value;
         break;
      }
   }
};

struct ast_mem_set
{
   int64_t mem_addr;
   int64_t mem_value;

   template<typename Result>
   void construct(Result &&result)
   {
      mem_addr = std::get<0>(result).value;
      mem_value = std::get<2>(result).value;
   }
};

struct ast_reg_check
{
   std::string reg_name;
   prs::optional<int64_t> reg_value;
   prs::optional<int64_t> mem_addr;

   template<typename Result>
   void construct(Result &&result)
   {
      reg_name = std::get<0>(result).value;

      auto &value = std::get<2>(result);

      switch(value.which)
      {
      case 0:
         mem_addr = std::get<0>(value)->value;
         break;
      case 1:
         reg_value = std::get<1>(value)->value;
         break;
      }
   }
};

struct ast_precondition
{
   prs::optional<ast_reg_set> reg_set;
   prs::optional<ast_mem_set> mem_set;

   template<typename Result>
   void construct(Result &&result)
   {
      auto &value = std::get<3>(result);

      switch (value.which) {
      case 0:
         reg_set = std::get<0>(value);
         break;
      case 1:
         mem_set = std::get<1>(value);
         break;
      }
   }
};

struct ast_postcondition
{
   prs::optional<ast_reg_check> reg_check;

   template<typename Result>
   void construct(Result &&result)
   {
      reg_check = std::get<3>(result);
   }
};

struct ast_test
{
   std::vector<ast_precondition> pre;
   std::vector<ast_postcondition> post;

   template<typename Result>
   void construct(Result &&result)
   {
      for (auto &value : result) {
         switch (value.which) {
         case 0:
            pre.push_back(*std::get<0>(value));
            break;
         case 1:
            post.push_back(*std::get<1>(value));
            break;
         }
      }
   }
};

bool parseTest(const std::string &path, ast_test &result);

#endif // ifndef TEST_PARSER_H
