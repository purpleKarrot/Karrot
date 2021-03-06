/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <sstream>

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

void XmlReader::parse_name(Name& name)
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
    throw_error();
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
    return;
    }
  */
  /*!re2c
  name
    {
    name.prefix = quark;
    name.local = std::string(marker, cursor);
    return;
    }
  else
    {
    throw_error();
    }
  */
  }

void XmlReader::parse_attribute(Attribute& attribute)
  {
  parse_name(attribute.name);
  /*!re2c
  space* '=' space*
    {
    }
  else
    {
    throw_error();
    }
  */
  marker = cursor;
  /*!re2c
  ["] [^"]* ["] | ['] [^']* [']
    {
    attribute.value = std::string(marker.base() + 1, cursor.base() - 1);
    return;
    }
  else
    {
    throw_error();
    }
  */
  }

void XmlReader::parse_pi()
  {
  parse_name(current_name);
  attributes.clear();
  token_ = token_pi;
instruction:
  /*!re2c
  space* "?>"
    {
    return;
    }
  space+
    {
    Attribute attr;
    parse_attribute(attr);
    attributes.push_back(attr);
    goto instruction;
    }
  else
    {
    throw_error();
    }
  */
  }

void XmlReader::parse_comment()
  {
  token_ = token_comment;
comment:
  /*!re2c
  "-->"
    {
    return;
    }
  "--" | [^]
    {
    goto comment;
    }
  */
  }

void XmlReader::parse_element()
  {
  parse_name(current_name);
  token_ = token_element;
  attributes.clear();
attribute:
  /*!re2c
  space* "/>"
    {
    is_empty_element = true;
    pop_namespaces(push_namespaces());
    return;
    }
  space* ">"
    {
    is_empty_element = false;
    push_tag();
    return;
    }
  space+
    {
    Attribute attr;
    parse_attribute(attr);
    attributes.push_back(attr);
    goto attribute;
    }
  else
    {
    throw_error();
    }
  */
  }

void XmlReader::parse_end_element()
  {
  const Name& expected = open_tags.back().name;
  parse_name(current_name);
  /*!re2c
  space* ">"
    {
    }
  else
    {
    throw_error();
    }
  */
  if (open_tags.empty())
    {
    throw_error();
    }
  if (current_name.prefix != expected.prefix || current_name.local != expected.local)
    {
    std::stringstream error;
    error << "Expected end tag: '";
    if (!expected.prefix.empty())
      {
      error << expected.prefix << ':';
      }
    error << expected.local << "'.\n";
    throw_error(error.str());
    }
  token_ = token_end_element;
  is_empty_element = false;
  pop_tag();
  }

bool XmlReader::parse_text()
  {
  do
    {
    ++cursor;
    if (cursor.base() == buffer.end())
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
  if (cursor.base() == buffer.end())
    {
    return false;
    }
  /*!re2c
  "<?"
    {
    parse_pi();
    return true;
    }
  "<!--"
    {
    parse_comment();
    return true;
    }
  "<"
    {
    parse_element();
    return true;
    }
  "</"
    {
    parse_end_element();
    return true;
    }
  else
    {
    return parse_text();
    }
  */
  }

} // namespace Karrot
