/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "url.hpp"

#include <cctype>
#include <cstring>
#include <stdexcept>
#include <string>
#include <algorithm>

namespace Karrot
{

static int hex_char(unsigned int c)
  {
  return "0123456789abcdef"[c & 0xF];
  }

std::string url_encode(std::string const& input)
  {
  std::string result;
  for (char c : input)
    {
    if (('0' <= c && c <= '9')
     || ('A' <= c && c <= 'Z')
     || ('a' <= c && c <= 'z')
     || (c == '~' || c == '-' || c == '_' || c == '.'))
      {
      result.push_back(c);
      }
    else
      {
      result.push_back('%');
      result.push_back(hex_char(c >> 4));
      result.push_back(hex_char(c));
      }
    }
  return result;
  }

std::string resolve_uri(std::string const& base, std::string const& relative)
  {
  std::size_t scheme_end = base.find("://");
  if (scheme_end == std::string::npos)
    {
    throw std::runtime_error("resolve_uri: base must be absolute");
    }
  if (relative.empty())
    {
    return base;
    }
  if (relative.find("://") != std::string::npos)
    {
    return relative;
    }
  if (relative[0] == '#')
    {
    return base.substr(0, base.find('#', scheme_end + 3)) + relative;
    }
  if (relative[0] == '?')
    {
    return base.substr(0, base.find('?', scheme_end + 3)) + relative;
    }
  if (relative[0] == '/')
    {
    if (relative[1] == '/')
      {
      return base.substr(0, scheme_end) + ':' + relative;
      }
    else
      {
      return base.substr(0, base.find('/', scheme_end + 3)) + relative;
      }
    }
  // TODO: remove dot segments
  return base.substr(0, base.rfind('/')) + '/' + relative;
  }

} // namespace Karrot
