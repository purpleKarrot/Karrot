/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <cstdio>

namespace Karrot
{

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
  std::string quark;
  marker = cursor;
  /*!re2c
  name
    {
    quark = std::string(marker, cursor);
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
    name.prefix.clear();
    name.local = quark;
    return true;
    }
  */
  /*!re2c
  name
    {
    name.prefix = quark;
    name.local = std::string(marker, cursor);
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
    attribute.value = std::string(marker + 1, cursor - 1);
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
    std::printf("Wrong end tag! %s:%s != %s:%s\n",
      current_name.prefix.c_str(),
      current_name.local.c_str(),
      expected.prefix.c_str(),
      expected.local.c_str());
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
    if (cursor == buffer.end())
      {
      return false;
      }
    }
  while (*cursor != '<');
  token_ = token_text;
  return true;
  }

bool XmlReader::read()
  {
  if (cursor == buffer.end())
    {
    return false;
    }
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

} // namespace Karrot
