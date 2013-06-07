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
    return Query::Token{Query::Token::LParen};
    }
  ")"
    {
    return Query::Token{Query::Token::RParen};
    }
  "<"  | "&lt;"
    {
    return Query::Token{Query::Token::Less};
    }
  "<=" | "&lt;="
    {
    return Query::Token{Query::Token::LessEqual};
    }
  ">"  | "&gt;"
    {
    return Query::Token{Query::Token::Greater};
    }
  ">=" | "&gt;="
    {
    return Query::Token{Query::Token::GreaterEqual};
    }
  "=="
    {
    return Query::Token{Query::Token::Equal};
    }
  "!="
    {
    return Query::Token{Query::Token::NotEqual};
    }
  "&&"
    {
    return Query::Token{Query::Token::And};
    }
  "||"
    {
    return Query::Token{Query::Token::Or};
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
