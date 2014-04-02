#ifndef PRS_CHAR_PARSER_H
#define PRS_CHAR_PARSER_H

/**
 * Matches a specific character from input
 *  returns char
 *
 * Create with char_('x')
 */

#include "base_parser.h"

namespace prs
{

template<typename Char>
struct char_parser : public parser<char_parser<Char>>
{
   char_parser(Char ch) :
      ch(ch)
   {
   }

   using value_type = Char;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context&, value_type &myv) const
   {
      if (first != last && *first == ch) {
         ++first;
         myv = ch;
         return true;
      }

      return false;
   }

   Char ch;
};

template<typename Char>
inline auto char_(Char ch)
   -> char_parser<Char>
{
   return char_parser<Char> { ch };
}

} // namespace prs

#endif // PRS_CHAR_PARSER_H
