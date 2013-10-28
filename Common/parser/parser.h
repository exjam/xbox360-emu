#ifndef PARSER_H
#define PARSER_H

/*
 * Expression template parser combinator, AKA, C++ is love, C++ is magic
 */

#include <string>
#include <vector>
#include <utility>
#include "optional.h"

#pragma warning(disable:4503)

/*
 * Base class for a parser so our operator overloads only affect parser classes
 */
template<typename Derived>
struct parser {
   Derived const& derived() const
   {
      return *static_cast<Derived const*>(this);
   }
};


/* Returns the value type for a Parser */
template<typename Parser>
static inline typename Parser::val_type ast_type(const parser<Parser> &p)
{
   return Parser::val_type();
}


/*
 * Parses with the .ws variable in Context
 */
struct whitespace_parser {
   template<typename Iterator, typename Context>
   static bool parse(Iterator &first, Iterator last, Context &ctx) 
   {
      decltype(ast_type(ctx.ws)) v;
      ctx.ws.parse<Iterator, Context, true>(first, last, ctx, v);
      return true;
   }
};


/*
 * Matches an specific character ch == Char
 */
template<typename Char>
struct char_parser : public parser<char_parser<Char>> {
   char_parser(Char ch) : ch(ch) {}

   typedef Char val_type;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, val_type &myv) const
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


/*
 * Matches any character in a range begin <= ch <= end
 */
template<typename Char>
struct char_range_parser : public parser<char_range_parser<Char>> {
   char_range_parser(Char begin, Char end) : begin(begin), end(end) {}

   typedef Char val_type;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, val_type &myv) const
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


/*
 * Matches a whole string
 */
template<typename String>
struct string_parser : public parser<string_parser<String>> {
   string_parser(String str) : str(str) {}

   typedef String val_type;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, val_type &myv) const
   {
      Iterator begin = first;

      for (String strItr = str; *strItr != 0; ++strItr, ++first) {
         if (first == last || *first != *strItr) {
            first = begin;
            return false;
         }
      }

      myv = str;
      return true;
   }

   String str;
};


/*
 * Parses a sequence (a >> b), a followed by b
 */
template<typename Left, typename Right>
struct sequence_parser : public parser<sequence_parser<Left, Right>> {
   sequence_parser(Left left, Right right) : left(left), right(right) {}

   typedef std::pair<typename Left::val_type, typename Right::val_type> val_type;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, val_type &myv) const
   {
      Iterator begin = first;

      if (!Atomic) {
         whitespace_parser::parse(first, last, ctx);
      }

      if (!left.parse<Iterator, Context, Atomic>(first, last, ctx, myv.first)) {
         first = begin;
         return false;
      }

      if (!Atomic) {
         whitespace_parser::parse(first, last, ctx);
      }

      if (!right.parse<Iterator, Context, Atomic>(first, last, ctx, myv.second)) {
         first = begin;
         return false;
      }

      return true;
   }

   Left left;
   Right right;
};


/*
 * Returns a std::pair<A, B> when A != B
 */
template<typename A, typename B>
struct need_pair {
   typedef std::pair<std::optional<A>, std::optional<B>> result_type;
};


/*
 * Returns the common type A or B when A == B
 */
template<typename T>
struct need_pair<T, T> {
   typedef T result_type;
};


/*
 * Match (a | b) a OR b, returns a std::pair<A, B> if A != B
 */
template<typename Left, typename Right>
struct alternative_parser : public parser<alternative_parser<Left, Right>> {
   alternative_parser(Left left, Right right) : left(left), right(right) {}

   typedef typename std::pair<std::optional<typename Left::val_type>, std::optional<typename Right::val_type>> pair_type;
   typedef typename Left::val_type single_type;

   typedef typename need_pair<typename Left::val_type, typename Right::val_type>::result_type val_type;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, pair_type &myv) const
   {
      typename Left::val_type lv;
      typename Right::val_type rv;

      if (!Atomic) {
         whitespace_parser::parse(first, last, ctx);
      }

      if (left.parse<Iterator, Context, Atomic>(first, last, ctx, lv)) {
         myv.first = lv;
         return true;
      }

      if (right.parse<Iterator, Context, Atomic>(first, last, ctx, rv)) {
         myv.second = rv;
         return true;
      }

      return false;
   }

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, single_type &myv) const
   {
      if (!Atomic) {
         whitespace_parser::parse(first, last, ctx);
      }

      if (left.parse<Iterator, Context, Atomic>(first, last, ctx, myv)) {
         return true;
      }

      if (right.parse<Iterator, Context, Atomic>(first, last, ctx, myv)) {
         return true;
      }

      return false;
   }

   Left left;
   Right right;
};


/*
 * Optionally matches a parser
 */
template<typename Option>
struct optional_parser : public parser<optional_parser<Option>> {
   optional_parser(Option option) : option(option) {}

   typedef std::optional<typename Option::val_type> val_type;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, val_type &myv) const
   {
      typename Option::val_type v;

      if (!Atomic) {
         whitespace_parser::parse(first, last, ctx);
      }

      if (option.parse<Iterator, Context, Atomic>(first, last, ctx, v)) {
         myv = v;
      }

      return true;
   }

   Option option;
};


/*
 * Kleene* parser, matches 0...n of a parser
 */
template<typename Repeat>
struct kleene_parser : public parser<kleene_parser<Repeat>> {
   kleene_parser(Repeat repeat) : repeat(repeat) {}

   typedef std::vector<typename Repeat::val_type> val_type;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, val_type &myv) const
   {
      typename Repeat::val_type v;
      myv = val_type();

      if (!Atomic) {
         whitespace_parser::parse(first, last, ctx);
      }

      while(repeat.parse<Iterator, Context, Atomic>(first, last, ctx, v)) {
         myv.push_back(v);
         v = Repeat::val_type();

         if (!Atomic) {
            whitespace_parser::parse(first, last, ctx);
         }
      }

      return true;
   }

   Repeat repeat;
};

/*
 * + parser, matches 1...n of a parser
 */
template<typename Repeat>
struct plus_parser : public parser<plus_parser<Repeat>> {
   plus_parser(Repeat repeat) : repeat(repeat) {}

   typedef std::vector<typename Repeat::val_type> val_type;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, val_type &myv) const
   {
      typename Repeat::val_type v;
      myv = val_type();

      if (!Atomic) {
         whitespace_parser::parse(first, last, ctx);
      }

      if (!repeat.parse<Iterator, Context, Atomic>(first, last, ctx, v)) {
         return false;
      }

      myv.push_back(v);
      v = Repeat::val_type();

      while(repeat.parse<Iterator, Context, Atomic>(first, last, ctx, v)) {
         myv.push_back(v);
         v = Repeat::val_type();

         if (!Atomic) {
            whitespace_parser::parse(first, last, ctx);
         }
      }

      return true;
   }

   Repeat repeat;
};

/*
 * Atomic parser, all child parsers will NOT consume whitespace
 */
template<typename Parser>
struct atomic_parser : public parser<atomic_parser<Parser>> {
   atomic_parser(Parser parser) : parser(parser) {}

   typedef typename Parser::val_type val_type;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, val_type &myv) const
   {
      return parser.parse<Iterator, Context, true>(first, last, ctx, myv);
   }

   Parser parser;
};


/*
 * A logical NOT lookahead parser which does not consume characters,
 * merely returns a bool representing whether the following parser
 * does NOT match
 */
template<typename Check>
struct not_parser : public parser<not_parser<Check>> {
   not_parser(Check check) : check(check) {}

   typedef bool val_type;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, val_type &myv) const
   {
      Iterator begin = first;
      Check::val_type v;

      if (!Atomic) {
         whitespace_parser::parse(first, last, ctx);
      }

      myv = !check.parse<Iterator, Context, Atomic>(first, last, ctx, v);
      first = begin;

      return myv;
   }

   Check check;
};


/*
 * Contains an ast and a parser
 */
template <typename Rule, typename RHS>
struct ast_definition : parser<ast_definition<Rule, RHS>>
{
   ast_definition(RHS rhs)
      : rhs(rhs) {}

   typedef typename Rule::Structure val_type;

   template <typename Iterator, typename Context, bool Atomic = false>
   bool parse(Iterator& first, Iterator last, Context& ctx, val_type &myv) const
   {
      RHS::val_type v;

      if (rhs.parse<Iterator, Context, Atomic>(first, last, ctx, v)) {
         myv.construct(v);
         return true;
      }

      return false;
   }

   RHS rhs;
};

/*
 * Some helper functions to convert built-in AST types to a string
 */
template<typename String, typename Option>
String ast_to_string(std::optional<Option> &opt)
{
   if (opt) {
      return ast_to_string<String>(*opt);
   } else {
      return String();
   }
}

template<typename String, typename A, typename B>
String ast_to_string(std::pair<A, B> &pair)
{
   return ast_to_string<String>(pair.first) + ast_to_string<String>(pair.second);
}

template<typename String, typename Char>
String ast_to_string(std::vector<Char> &chs)
{
   String str;

   for (auto ch : chs) {
      str.append(ast_to_string<String>(ch));
   }

   return str;
}

template<typename String>
String ast_to_string(char &val)
{
   String str;
   str.push_back(val);
   return str;
}

template<typename String>
String ast_to_string(int &val)
{
   String str;
   str.push_back(val);
   return str;
}

/* Comes from a NOT parser */
template<typename String>
String ast_to_string(bool &)
{
   return String();
}

template<typename String, typename Value>
String ast_to_string(Value &val)
{
   return String(val);
}


/*
 * Contains target ast structure to parse into
 */
template <typename AST>
struct ast
{
   typedef AST Structure;
};


/* Helper function to return the AST value type */
template<typename AST, typename Right>
static inline typename AST ast_type(const ast_definition<ast<AST>, Right> &ast_def)
{
   return AST();
}


/* Matches any character */
static inline char_range_parser<int> char_any()
{
   return char_range_parser<int>(0, 999999);
}


/* Creates an atomic parser */
template<typename Parser>
static inline atomic_parser<Parser> atomic(Parser parser)
{
   return atomic_parser<Parser>(parser);
}


/* Matches a single character */
template<typename Char>
static inline char_parser<Char> char_(Char ch)
{
   return char_parser<Char>(ch);
}


/* Matches a range of characters begin <= ch <= end */
template<typename Char>
static inline char_range_parser<Char> char_range(Char begin, Char end)
{
   return char_range_parser<Char>(begin, end);
}


/* Matches a whole string */
template<typename String>
static inline string_parser<String> string_(String ch)
{
   return string_parser<String>(ch);
}


/* Combines a user ast<Type> with a parser */
template<typename ID, typename Right>
inline ast_definition<ast<ID>, Right> operator >>(ast<ID> const &left, parser<Right> const &right)
{
   return ast_definition<ast<ID>, Right>(right.derived());
}


/* Create a sequence parser (a >> b): a followed by b */
template<typename Left, typename Right>
static inline sequence_parser<Left, Right> operator >>(parser<Left> const &left, parser<Right> const &right)
{
   return sequence_parser<Left, Right>(left.derived(), right.derived());
}


/* Create a alternative parser (a | b): a OR b */
template<typename Left, typename Right>
inline alternative_parser<Left, Right> operator |(parser<Left> const &left, parser<Right> const &right)
{
   return alternative_parser<Left, Right>(left.derived(), right.derived());
}


/* Create a kleene* parser (*a): 0...n of a */
template<typename Repeat>
kleene_parser<Repeat> operator*(parser<Repeat> const &repeat)
{
   return kleene_parser<Repeat>(repeat.derived());
}


/* Create a + parser (+a): 1...n of a */
template<typename Repeat>
plus_parser<Repeat> operator+(parser<Repeat> const &option)
{
   return plus_parser<Repeat>(option.derived());
}


/* Create an optional parser (-a): 0 or 1 of a */
template<typename Option>
optional_parser<Option> operator-(parser<Option> const &option)
{
   return optional_parser<Option>(option.derived());
}


/* Create a lookahead non consuming logical NOT parser */
template<typename Check>
not_parser<Check> operator!(parser<Check> const &check)
{
   return not_parser<Check>(check.derived());
}

#endif // ifdef PARSER_H
