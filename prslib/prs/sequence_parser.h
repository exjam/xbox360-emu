#ifndef PRS_SEQUENCE_PARSER_H
#define PRS_SEQUENCE_PARSER_H

/**
 * Parses a then b
 *  returns std::tuple
 *
 * Create with operator a >> b
 */

#include "base_parser.h"
#include "whitespace_parser.h"
#include "util.h"
#include "rule.h"
#include <tuple>

namespace prs
{

template<typename ...A, typename ...B>
inline auto make_sequence(std::tuple<A...> a, std::tuple<B...> b)
   -> std::tuple<A..., B...>
{
   return std::tuple_cat(a, b);
}

template<typename ...A, typename B>
inline auto make_sequence(std::tuple<A...> a, B b)
   -> std::tuple<A..., B>
{
   return std::tuple_cat(a, std::make_tuple(b));
}

template<typename A, typename ...B>
inline auto make_sequence(A a, std::tuple<B...> b)
   -> std::tuple<A, B...>
{
   return std::tuple_cat(std::make_tuple(a), b);
}

template<typename A, typename B>
inline auto make_sequence(A a, B b)
   -> std::tuple<A, B>
{
   return std::make_tuple(a, b);
}

template<typename Left, typename Right>
struct sequence_parser : public parser<sequence_parser<Left, Right>>
{
   sequence_parser(Left left, Right right) :
      left(left),
      right(right)
   {
   }

   using value_type = decltype(make_sequence(std::declval<typename Left::value_type>(),
                                             std::declval<typename Right::value_type>()));

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, value_type &myv) const
   {
      typename Left::value_type lv;
      typename Right::value_type rv;
      Iterator begin = first;

      if (!Atomic) {
         whitespace_parser::parse(first, last, ctx);
      }

      if (!left.template parse<Iterator, Context, Atomic>(first, last,
                                                          ctx, lv)) {
         first = begin;
         return false;
      }

      if (!Atomic) {
         whitespace_parser::parse(first, last, ctx);
      }

      if (!right.template parse<Iterator, Context, Atomic>(first, last,
                                                           ctx, rv)) {
         first = begin;
         return false;
      }

      myv = make_sequence(lv, rv);
      return true;
   }

   Left left;
   Right right;
};

template<typename Left, typename Right>
inline auto make_sequence_parser(const Left &left, const Right &right)
   -> sequence_parser<decltype(make_rule_or_parser(left)),
                      decltype(make_rule_or_parser(right))>
{
   return {
      make_rule_or_parser(left),
      make_rule_or_parser(right)
   };
}

template<typename Left, typename Right>
inline auto operator >>(const Left &left, const Right &right)
-> typename
      enable_if_both<
         std::identity<decltype(is_parser_or_rule<Left>())>::type::value,
         std::identity<decltype(is_parser_or_rule<Right>())>::type::value,
         decltype(make_sequence_parser(left, right))
      >::type
{
   return make_sequence_parser(left, right);
}

} // namespace prs

#endif // PRS_SEQUENCE_PARSER_H
