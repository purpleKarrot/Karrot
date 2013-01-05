/*
 * Copyright (C) 2012 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <cstdio>
#include <karrot/quark.hpp>

namespace karrot
{

typedef std::vector<char>::iterator iter_t;
static int parse_quark(iter_t begin, iter_t end)
  {
  return string_to_quark(&(*begin), end - begin);
  }

/*!re2c
re2c:indent:string   = "  ";
re2c:indent:top      = 1;
re2c:yyfill:enable   = 0;
re2c:define:YYCTYPE  = char;
re2c:define:YYCURSOR = cursor;
re2c:define:YYMARKER = marker;
alpha                = [a-zA-Z];
alnum                = alpha | [0-9];
space                = [\t\n\v\f\r ];
name                 = alpha (alnum | '-' | '_')*;
else                 = [];
*/

bool XmlReader::parse_name(Name& name)
  {
  int quark;
  name.namespace_uri = 0;
  marker = cursor;
  /*!re2c
  name
    {
    quark = parse_quark(marker, cursor);
    }
  else
    {
    return false;
    }
  */
  /*!re2c
  ":"
    {
    marker = cursor;
    }
  else
    {
    name.prefix = 0;
    name.local = quark;
    return true;
    }
  */
  /*!re2c
  name
    {
    name.prefix = quark;
    name.local = parse_quark(marker, cursor);
    return true;
    }
  else
    {
    return false;
    }
  */
  }

bool XmlReader::parse_attribute(Attribute& attribute)
  {
  if (!parse_name(attribute.name))
    {
    return false;
    }
  /*!re2c
  space* '=' space*
    {
    }
  else
    {
    return false;
    }
  */
  marker = cursor;
  /*!re2c
  ["] [^"]* ["] | ['] [^']* [']
    {
    attribute.value = parse_quark(marker + 1, cursor - 1);
    return true;
    }
  else
    {
    return false;
    }
  */
  }

bool XmlReader::parse_pi()
  {
  if (!parse_name(current_name))
    {
    return false;
    }
  attributes.clear();
  token_ = token_pi;
instruction:
  /*!re2c
  space* "?>"
    {
    return true;
    }
  space+
    {
    Attribute attr;
    if (parse_attribute(attr))
      {
      attributes.push_back(attr);
      goto instruction;
      }
    return false;
    }
  else
    {
    return false;
    }
  */
  }

bool XmlReader::parse_comment()
  {
  token_ = token_comment;
comment:
  /*!re2c
  "-->"
    {
    return true;
    }
  "--" | [^]
    {
    goto comment;
    }
  */
  }

bool XmlReader::parse_element()
  {
  if (!parse_name(current_name))
    {
    return false;
    }
  token_ = token_element;
  attributes.clear();
attribute:
  /*!re2c
  space* "/>"
    {
    is_empty_element = true;
    pop_namespaces(push_namespaces());
    return true;
    }
  space* ">"
    {
    is_empty_element = false;
    push_tag();
    return true;
    }
  space+
    {
    Attribute attr;
    if (parse_attribute(attr))
      {
      attributes.push_back(attr);
      goto attribute;
      }
    return false;
    }
  else
    {
    return false;
    }
  */
  }

bool XmlReader::parse_end_element()
  {
  const Name& expected = open_tags.back().name;
  if (!parse_name(current_name))
    {
    return false;
    }
  /*!re2c
  space* ">"
    {
    }
  else
    {
    return false;
    }
  */
  if (open_tags.empty())
    {
    return false;
    }
  if (current_name.prefix != expected.prefix || current_name.local != expected.local)
    {
    printf("Wrong end tag! %s:%s != %s:%s\n",
      quark_to_string(current_name.prefix),
      quark_to_string(current_name.local),
      quark_to_string(expected.prefix),
      quark_to_string(expected.local));
    return false;
    }
  token_ = token_end_element;
  is_empty_element = false;
  pop_tag();
  return true;
  }

bool XmlReader::parse_text()
  {
  do
    {
    ++cursor;
    }
  while (*cursor != '<');
  token_ = token_text;
  return true;
  }

bool XmlReader::read()
  {
  /*!re2c
  "<?"
    {
    return parse_pi();
    }
  "<!--"
    {
    return parse_comment();
    }
  "<"
    {
    return parse_element();
    }
  "</"
    {
    return parse_end_element();
    }
  else
    {
    return parse_text();
    }
  */
  }

} // namespace karrot
