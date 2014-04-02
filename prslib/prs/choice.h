#ifndef PRS_CHOICE_H
#define PRS_CHOICE_H

/*
 * A data container which is a combination of std::optional and std::tuple
 */

#include "base_parser.h"
#include "optional.h"
#include <tuple>

namespace prs
{

template<typename ...Ts>
class choice : public std::tuple<Ts...> {
public:
   choice()
   {
   }

   choice(const std::tuple<Ts...> &tuple, unsigned long which) :
      std::tuple<Ts...>(tuple),
      which(which)
   {
   }

   bool is(unsigned long o)
   {
      return which == o;
   }

   unsigned long which;
};

enum Choice {
   Left = 0,
   Right = 1
};

template<typename ...A, typename ...B>
auto make_choice(choice<A...> a, choice<B...> b, Choice which)
   -> choice<A..., B...>
{
   return {
      std::tuple_cat(a, b),
      which == Choice::Left ? a.which
                            : std::tuple_size<std::tuple<A...>>::value + b.which
   };
}

template<typename ...A, typename B>
auto make_choice(choice<A...> a, B b, Choice which)
   -> choice<A..., optional<B>>
{
   if (which == Choice::Left) {
      return {
         std::tuple_cat(static_cast<std::tuple<A...>>(a), std::make_tuple(optional<B> { })),
         a.which
      };
   } else {
      return {
         std::tuple_cat(static_cast<std::tuple<A...>>(a), std::make_tuple(make_optional(b))),
         std::tuple_size<std::tuple<A...>>::value
      };
   }
}

template<typename A, typename ...B>
auto make_choice(A a, choice<B...> b, Choice which)
   -> choice<optional<A>, B...>
{
   if (which == Choice::Left) {
      return {
         std::tuple_cat(std::make_tuple(make_optional(a)), b),
         Choice::Left
      };
   } else {
      return {
         std::tuple_cat(std::make_tuple(optional<A> { }), b),
         b.which + 1
      };
   }
}

// TODO: remove choice<optional<A>, optional<B>> { when VS is not broken
// TODO: change to optional<B> { } when VS not broken
template<typename A, typename B>
auto make_choice(A a, B b, Choice which)
   -> choice<optional<A>, optional<B>>
{
   if (which == Choice::Left) {
      return choice<optional<A>, optional<B>> {
         std::make_tuple(make_optional(a), optional<B>()),
         Choice::Left
      };
   } else {
      return choice<optional<A>, optional<B>> {
         std::make_tuple(optional<A> { }, make_optional(b)),
         Choice::Right
      };
   }
}

template<typename T>
auto make_choice(T a, T b, Choice which)
   -> T
{
   return which == Choice::Left ? a : b;
}

} // namespace prs

#endif // PRS_CHOICE_H
