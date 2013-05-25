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

std::string url_to_filename(std::string url)
  {
  std::size_t scheme = url.find("://");
  if (scheme != std::string::npos)
    {
    url.erase(0, scheme + 3);
    }
  replace(begin(url), end(url), '/', '-');
  return url;
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
