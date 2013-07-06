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
      char dig1 = (c & 0xF0) >> 4;
      char dig2 = (c & 0x0F);
      if (0 <= dig1 && dig1 <= 9)
        {
        dig1 += 48;
        }
      if (10 <= dig1 && dig1 <= 15)
        {
        dig1 += 65 - 10;
        }
      if (0 <= dig2 && dig2 <= 9)
        {
        dig2 += 48;
        }
      if (10 <= dig2 && dig2 <= 15)
        {
        dig2 += 65 - 10;
        }
      result.push_back('%');
      result.push_back(dig1);
      result.push_back(dig2);
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
