#ifndef PRS_WHITESPACE_PARSER_H
#define PRS_WHITESPACE_PARSER_H

/**
 * Parses with the .whitespace_parser variable in Context
 */

namespace prs
{

struct whitespace_parser
{
   template<typename Iterator, typename Context>
   static bool parse(Iterator &first, Iterator last, Context &ctx)
   {
      typename decltype(ctx.whitespace_parser)::value_type val;

      ctx.whitespace_parser.template parse<Iterator, Context, true>(first,
                                                                    last,
                                                                    ctx,
                                                                    val);
      return true;
   }
};

} // namespace prs

#endif // PRS_WHITESPACE_PARSER_H

