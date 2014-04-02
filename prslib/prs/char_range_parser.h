#ifndef PRS_CHAR_RANGE_PARSER_H
#define PRS_CHAR_RANGE_PARSER_H

/**
 * Matches any character in a range begin <= x <= end
 *  returns character
 *
 * Create with char_range('begin', 'end')
 */

#include "base_parser.h"

namespace prs
{

template<typename Char>
struct char_range_parser : public parser<char_range_parser<Char>>
{
   char_range_parser(Char begin, Char end) :
      begin(begin),
      end(end)
   {
   }

   using value_type = Char;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context&, value_type &myv) const
   {
      if (first != last && *first >= begin && *first <= end) {
         myv = *first;
         ++first;
         return true;
      }

      return false;
   }

   Char begin;
   Char end;
};

template<typename Char>
inline auto char_range(Char begin, Char end)
   -> char_range_parser<Char>
{
   return char_range_parser<Char> { begin, end };
}

inline auto char_any()
   -> char_range_parser<int>
{
   return char_range_parser<int> { 0, 999999 };
}

} // namespace prs

#endif // PRS_CHAR_RANGE_PARSER_H
