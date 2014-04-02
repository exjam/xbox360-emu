#ifndef PRS_ZERO_OR_MORE_PARSER_H
#define PRS_ZERO_OR_MORE_PARSER_H

/**
 * Matches 0...n of x
 *  returns std::vector<X>
 *
 * Create with operator *x
 */

#include "base_parser.h"
#include "whitespace_parser.h"
#include "util.h"
#include "rule.h"
#include <vector>

namespace prs
{

template<typename Repeat>
struct zero_or_more_parser : public parser<zero_or_more_parser<Repeat>>
{
   zero_or_more_parser(Repeat repeat) :
      repeat(repeat)
   {
   }

   using value_type = std::vector<typename Repeat::value_type>;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, value_type &myv) const
   {
      typename Repeat::value_type v;
      myv = value_type { };

      if (!Atomic) {
         whitespace_parser::parse(first, last, ctx);
      }

      while(repeat.template parse<Iterator, Context, Atomic>(first, last, ctx, v)) {
         myv.push_back(v);
         v = typename Repeat::value_type { };

         if (!Atomic) {
            whitespace_parser::parse(first, last, ctx);
         }
      }

      return true;
   }

   Repeat repeat;
};

template<typename Parser>
inline auto make_zero_or_more_parser(const Parser &parser)
   -> zero_or_more_parser<decltype(make_rule_or_parser(parser))>
{
   return { make_rule_or_parser(parser) };
}

template<typename Parser>
inline auto operator *(const Parser &parser)
   -> typename std::enable_if<
         std::identity<decltype(is_parser_or_rule<Parser>())>::type::value,
         decltype(make_zero_or_more_parser(parser))
      >::type
{
   return make_zero_or_more_parser(parser);
}

} // namespace prs

#endif // PRS_ZERO_OR_MORE_PARSER_H
