/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <stdexcept>
#include "quark.hpp"

namespace karrot
{

#define YYFILL                                                                 \
  {                                                                            \
  if (cursor != marker)                                                        \
    {                                                                          \
    return string_to_quark(&(*marker), cursor - marker);                       \
    }                                                                          \
  }                                                                            \

static int query_tokenize(std::string::const_iterator& cursor,std::string::const_iterator limit)
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
    return LPAREN;
    }
  ")"
    {
    return RPAREN;
    }
  "<"  | "&lt;"
    {
    return LESS;
    }
  "<=" | "&lt;="
    {
    return LESS_EQUAL;
    }
  ">"  | "&gt;"
    {
    return GREATER;
    }
  ">=" | "&gt;="
    {
    return GREATER_EQUAL;
    }
  "=="
    {
    return EQUAL;
    }
  "!="
    {
    return NOT_EQUAL;
    }
  "&&"
    {
    return AND;
    }
  "||"
    {
    return OR;
    }
  [^()<>!=&|]+
    {
    return string_to_quark(&(*marker), cursor - marker);
    }
  [^]
    {
    throw std::runtime_error("Invalid query");
    }
  */
  }

} // namespace karrot
