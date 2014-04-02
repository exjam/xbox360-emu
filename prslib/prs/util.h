#ifndef PRS_UTIL_H
#define PRS_UTIL_H

#include "base_parser.h"
#include "rule.h"

namespace prs
{

template<typename Parser>
std::true_type is_parser_or_rule(const parser<Parser>& = parser<Parser>());
template<typename A, typename B, typename C, bool D>
std::true_type is_parser_or_rule(const rule<A, B, C, D>& = rule<A, B, C, D>());
std::false_type is_parser_or_rule(...);

template<typename Parser>
std::true_type is_parser(const parser<Parser>& = parser<Parser>());
std::false_type is_parser(...);

template<typename A, typename B, typename C, bool D>
std::true_type is_rule(const rule<A, B, C, D>& = rule<A, B, C, D>());
std::false_type is_rule(...);

/*
 * Needed because VS2013 is messy...
 */
template<bool _Test1, bool _Test2, typename _Ty = void>
struct enable_if_both
{
};

template<typename _Ty>
struct enable_if_both<true, true, _Ty>
{
   typedef _Ty type;
};

} // namespace prs

#endif // PRS_UTIL_H
