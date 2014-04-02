#ifndef PRS_RULE_H
#define PRS_RULE_H

/**
 * A rule definition which can be used to construct data in ASTs
 */

#include "base_parser.h"
#include <functional>

namespace prs
{

template<typename AstType, typename Iterator, typename Context, bool Atomic = false>
class rule
{
public:
   using value_type = AstType;

   rule()
   {
   }

   template <typename Parser>
   rule(parser<Parser> parser)
   {
      m_impl =
         [parser](Iterator &first, Iterator last, Context &ctx, value_type &myv) {
            typename Parser::value_type v;

            if (parser.derived().template parse<Iterator, Context, Atomic>(first, last, ctx, v)) {
               myv.construct(std::move(v));
               return true;
            }

            return false;
         };
   }

   template <typename Parser>
   void operator=(const parser<Parser> &parser)
   {
      m_impl =
         [&parser](Iterator &first, Iterator last, Context &ctx, value_type &myv) {
            typename Parser::value_type v;

            if (parser.derived().template parse<Iterator, Context, Atomic>(first, last, ctx, v)) {
               myv.construct(std::move(v));
               return true;
            }

            return false;
         };
   }

   template<typename IteratorIn, typename ContextIn, bool AtomicIn = false>
   bool parse(IteratorIn& first, IteratorIn last, ContextIn& ctx, value_type &myv) const
   {
      return m_impl(first, last, ctx, myv);
   }

private:
   std::function<bool(Iterator&,Iterator,Context&,value_type&)> m_impl;
};


/*
 * Our grammar tree is stored by value, however for recursive grammars we need
 * forward declarations, thus we need to store rules by reference in our
 * grammar tree so we have access to the assigned definitions.
 */

template<typename AstType, typename Iterator, typename Context, bool Atomic>
class rule_ref : public parser<rule_ref<AstType, Iterator, Context, Atomic>>
{
public:
   using value_type = typename rule<AstType, Iterator, Context, Atomic>::value_type;

   rule_ref(const rule<AstType, Iterator, Context, Atomic> &rule) :
      m_rule(rule)
   {
   }

   template<typename IteratorIn, typename ContextIn, bool AtomicIn = false>
   bool parse(IteratorIn& first, IteratorIn last, ContextIn& ctx, value_type &myv) const
   {
      return m_rule.template parse<IteratorIn, ContextIn, AtomicIn>(first, last, ctx, myv);
   }

private:
   const rule<AstType, Iterator, Context, Atomic> &m_rule;
};

/*
 * Create a rule_ref or retrieve derived Parser type from T
 */
template<typename AstType, typename Iterator, typename Context, bool Atomic>
inline auto make_rule_or_parser(const rule<AstType, Iterator, Context, Atomic> &rule)
   -> rule_ref<AstType, Iterator, Context, Atomic>
{
   return { rule };
}

template<typename Parser>
inline auto make_rule_or_parser(const parser<Parser> &parser)
   -> Parser
{
   return parser.derived();
}

} // namespace prs

#endif // PRS_RULE_H
