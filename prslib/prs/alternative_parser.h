#ifndef PRS_ALTERNATIVE_PARSER_H
#define PRS_ALTERNATIVE_PARSER_H

/**
 * Match x OR y
 *  returns common type if std::is_same<A, B>
 *  returns choice<A..., B...> otherwise
 *
 * Create with operator x | y
 */

#include "base_parser.h"
#include "whitespace_parser.h"
#include "choice.h"
#include "util.h"
#include "rule.h"

namespace prs
{

template<typename Left, typename Right>
struct alternative_parser : public parser<alternative_parser<Left, Right>>
{
   alternative_parser(Left left_, Right right_) :
      left(left_),
      right(right_)
   {
   }

   using value_type = decltype(make_choice(std::declval<typename Left::value_type>(),
                                           std::declval<typename Right::value_type>(),
                                           Choice::Left));

   /* Returning tuple of results */
   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, value_type &myv) const
   {
      typename Left::value_type lv { };
      typename Right::value_type rv { };

      if (!Atomic) {
         whitespace_parser::parse(first, last, ctx);
      }

      if (left.template parse<Iterator, Context, Atomic>(first, last, ctx, lv)) {
         myv = make_choice(lv, rv, Choice::Left);
         return true;
      }

      if (right.template parse<Iterator, Context, Atomic>(first, last, ctx, rv)) {
         myv = make_choice(lv, rv, Choice::Right);
         return true;
      }

      return false;
   }

   Left left;
   Right right;
};

template<typename Left, typename Right>
inline auto make_alternative_parser(const Left &left, const Right &right)
   -> alternative_parser<decltype(make_rule_or_parser(left)),
                         decltype(make_rule_or_parser(right))>
{
   return {
      make_rule_or_parser(left),
      make_rule_or_parser(right)
   };
}

template<typename Left, typename Right>
inline auto operator |(const Left &left, const Right &right)
-> typename
      enable_if_both<
         std::identity<decltype(is_parser_or_rule<Left>())>::type::value,
         std::identity<decltype(is_parser_or_rule<Right>())>::type::value,
         decltype(make_alternative_parser(left, right))
      >::type
{
   return make_alternative_parser(left, right);
}

} // namespace prs

#endif // PRS_ALTERNATIVE_PARSER_H
