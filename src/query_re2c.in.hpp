/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <stdexcept>

namespace Karrot
{

#define YYFILL                                                                 \
  {                                                                            \
  if (cursor != marker)                                                        \
    {                                                                          \
    return Query::Token{String(&(*marker), cursor - marker)};                  \
    }                                                                          \
  }                                                                            \

static Query::Token query_tokenize(
    std::string::const_iterator& cursor,
    std::string::const_iterator limit)
  {
  std::string::const_iterator marker = cursor;
  /*!re2c
  re2c:define:YYCTYPE  = char;
  re2c:define:YYCURSOR = cursor;
  re2c:define:YYMARKER = marker;
  re2c:define:YYLIMIT  = limit;
  re2c:define:YYFILL:naked = 1;
  "("
    {
    return Query::Token{Query::Token::LPAREN};
    }
  ")"
    {
    return Query::Token{Query::Token::RPAREN};
    }
  "<"  | "&lt;"
    {
    return Query::Token{Query::Token::LESS};
    }
  "<=" | "&lt;="
    {
    return Query::Token{Query::Token::LESS_EQUAL};
    }
  ">"  | "&gt;"
    {
    return Query::Token{Query::Token::GREATER};
    }
  ">=" | "&gt;="
    {
    return Query::Token{Query::Token::GREATER_EQUAL};
    }
  "=="
    {
    return Query::Token{Query::Token::EQUAL};
    }
  "!="
    {
    return Query::Token{Query::Token::NOT_EQUAL};
    }
  "&&"
    {
    return Query::Token{Query::Token::AND};
    }
  "||"
    {
    return Query::Token{Query::Token::OR};
    }
  [^()<>!=&|]+
    {
    return Query::Token{String(&(*marker), cursor - marker)};
    }
  [^]
    {
    throw std::runtime_error("Invalid query");
    }
  */
  }

} // namespace Karrot
