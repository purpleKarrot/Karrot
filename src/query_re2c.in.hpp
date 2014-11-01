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
    return pool.from_string(std::string(&(*marker), cursor - marker).c_str()); \
    }                                                                          \
  }                                                                            \

static int query_tokenize(
    std::string::const_iterator& cursor,
    std::string::const_iterator limit, StringPool& pool)
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
    return STR_LPAREN;
    }
  ")"
    {
    return STR_RPAREN;
    }
  "<"  | "&lt;"
    {
    return STR_LESS;
    }
  "<=" | "&lt;="
    {
    return STR_LESS_EQUAL;
    }
  ">"  | "&gt;"
    {
    return STR_GREATER;
    }
  ">=" | "&gt;="
    {
    return STR_GREATER_EQUAL;
    }
  "=="
    {
    return STR_EQUAL;
    }
  "!="
    {
    return STR_NOT_EQUAL;
    }
  "&&" | "&amp;&amp;"
    {
    return STR_AND;
    }
  "||"
    {
    return STR_OR;
    }
  [^()<>!=&|]+
    {
    return pool.from_string(std::string(&(*marker), cursor - marker).c_str());
    }
  [^]
    {
    throw std::runtime_error("Invalid query");
    }
  */
  }

} // namespace Karrot
