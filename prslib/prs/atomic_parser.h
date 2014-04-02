#ifndef PRS_ATOMIC_PARSER_H
#define PRS_ATOMIC_PARSER_H

/**
 * Atomic parser, all child parsers will NOT consume whitespace
 *  returns child value
 *
 * Create with atomic(x)
 */

#include "base_parser.h"
#include "util.h"
#include "rule.h"

namespace prs
{

template<typename Parser>
struct atomic_parser : public parser<atomic_parser<Parser>>
{
   atomic_parser(Parser parser) :
      parser(parser)
   {
   }

   using value_type = typename Parser::value_type;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, value_type &myv) const
   {
      return parser.template parse<Iterator, Context, true>(first, last, ctx, myv);
   }

   Parser parser;
};

template<typename Parser>
inline auto make_atomic_parser(const Parser &parser)
   -> atomic_parser<decltype(make_rule_or_parser(parser))>
{
   return {
      make_rule_or_parser(parser)
   };
}

template<typename Parser>
inline auto atomic(const Parser &parser)
   -> typename std::enable_if<
         std::identity<decltype(is_parser_or_rule<Parser>())>::type::value,
         decltype(make_atomic_parser(parser))
   >::type
{
   return make_atomic_parser(parser);
}

} // namespace prs

#endif // PRS_ATOMIC_PARSER_H
