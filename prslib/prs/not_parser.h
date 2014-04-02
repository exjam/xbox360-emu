#ifndef PRS_NOT_PARSER_H
#define PRS_NOT_PARSER_H

/**
 * Matches !x, does not consume input
 *  returns nothing
 *
 * Create with operator !x
 */

#include "base_parser.h"
#include "whitespace_parser.h"
#include "util.h"
#include "rule.h"

namespace prs
{

template<typename Check>
struct not_parser : public parser<not_parser<Check>>
{
   not_parser(Check check) :
      check(check)
   {
   }

   using value_type = bool;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, value_type &myv) const
   {
      Iterator begin = first;
      typename Check::value_type v;

      if (!Atomic) {
         whitespace_parser::parse(first, last, ctx);
      }

      myv = !check.template parse<Iterator, Context, Atomic>(first, last, ctx, v);
      first = begin;

      return myv;
   }

   Check check;
};

template<typename Parser>
inline auto make_not_parser(const Parser &parser)
   -> not_parser<decltype(make_rule_or_parser(parser))>
{
   return { make_rule_or_parser(parser) };
}

template<typename Parser>
inline auto operator !(const Parser &parser)
   -> typename std::enable_if<
         std::identity<decltype(is_parser_or_rule<Parser>())>::type::value,
         decltype(make_not_parser(parser))
      >::type
{
   return make_not_parser(parser);
}

} // namespace prs

#endif // PRS_NOT_PARSER_H
