#ifndef PRS_OPTIONAL_PARSER_H
#define PRS_OPTIONAL_PARSER_H

/**
 * Matches 0...1 of x
 *  returns optional<X>
 *
 * Create with operator -x
 */

#include "base_parser.h"
#include "whitespace_parser.h"
#include "util.h"
#include "rule.h"

namespace prs
{

template<typename Option>
struct optional_parser : public parser<optional_parser<Option>>
{
   optional_parser(Option option) :
      option(option)
   {
   }

   using value_type = optional<typename Option::value_type>;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, value_type &myv) const
   {
      typename Option::value_type v;

      if (!Atomic) {
         whitespace_parser::parse(first, last, ctx);
      }

      if (option.template parse<Iterator, Context, Atomic>(first, last, ctx, v)) {
         myv = v;
      }

      return true;
   }

   Option option;
};

template<typename Parser>
inline auto make_optional_parser(const Parser &parser)
   -> optional_parser<decltype(make_rule_or_parser(parser))>
{
   return { make_rule_or_parser(parser) };
}

template<typename Parser>
inline auto operator -(const Parser &parser)
   -> typename std::enable_if<
         std::identity<decltype(is_parser_or_rule<Parser>())>::type::value,
         decltype(make_optional_parser(parser))
      >::type
{
   return make_optional_parser(parser);
}

} // namespace prs

#endif // PRS_OPTIONAL_PARSER_H
