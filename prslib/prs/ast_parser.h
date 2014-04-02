#ifndef PRS_AST_PARSER_H
#define PRS_AST_PARSER_H

/**
* Parses into a structure by calling AstType::construct(
*  returns char
*
* Create with char_('x')
*/

#include "base_parser.h"
#include "rule.h"

namespace prs
{

template <typename AstType, typename Parser>
struct ast_parser : parser<ast_parser<AstType, Parser>>
{
   ast_parser(const Parser &parser)
      : parser(parser)
   {
   }

   using value_type = AstType;

   template <typename Iterator, typename Context, bool Atomic = false>
   bool parse(Iterator& first, Iterator last, Context& ctx, value_type &myv) const
   {
      typename Parser::value_type v;

      if (parser.parse<Iterator, Context, Atomic>(first, last, ctx, v)) {
         myv.construct(std::move(v));
         return true;
      }

      return false;
   }

   Parser parser;
};

template <typename AstType>
struct ast
{
   using value_type = AstType;
};

template<typename AstType, typename Parser>
inline auto make_ast_parser(const parser<Parser> &parser)
   -> ast_parser<AstType, Parser>
{
   return { make_rule_or_parser(parser) };
}

template<typename AstType, typename Parser>
inline auto operator >>(const ast<AstType> &left, const parser<Parser> &parser)
   -> decltype(make_ast_parser<AstType>(parser))
{
   return make_ast_parser<AstType>(parser);
}

} // namespace prs

#endif // PRS_AST_PARSER_H

