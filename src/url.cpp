/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "url.hpp"
#include "quark.hpp"

#include <cctype>
#include <cstring>
#include <stdexcept>
#include <string>

namespace Karrot
{

Url::Url() :
    scheme(0), user_info(0), host(0), port(0), path(0), query(0), fragment(0)
  {
  }

// TODO: relative paths are not supported.
// TODO: replace by C++1y URI Proposal (http://github.com/glynos/uri_c--1y)

Url::Url(const char* str, Url* base) :
    scheme(0), user_info(0), host(0), port(0), path(0), query(0), fragment(0)
  {
  std::size_t length;
  if (base)
    {
    if (str[0] == '/')
      {
      if (str[1] == '/')
        {
        str += 2;
        this->scheme = base->scheme;
        goto scheme_specific;
        }
      else
        {
        this->scheme = base->scheme;
        this->user_info = base->user_info;
        this->host = base->host;
        this->port = base->port;
        goto path;
        }
      }
    if (str[0] == '?')
      {
      this->scheme = base->scheme;
      this->user_info = base->user_info;
      this->host = base->host;
      this->port = base->port;
      this->path = base->path;
      goto query;
      }
    if (str[0] == '#')
      {
      this->scheme = base->scheme;
      this->user_info = base->user_info;
      this->host = base->host;
      this->port = base->port;
      this->path = base->path;
      this->query = base->query;
      goto fragment;
      }
    }
  length = std::strcspn(str, ":");
  this->scheme = string_to_quark(str, length);
  str += length;
  if (*str++ != ':')
    {
    goto error;
    }
  if (*str++ != '/')
    {
    goto error;
    }
  if (*str++ != '/')
    {
    goto error;
    }
scheme_specific:
  length = std::strcspn(str, "@:[/?#");
  if (str[length] == '@')
    {
    this->user_info = string_to_quark(str, length);
    str += length + 1;
    }
  else if (str[length] == ':')
    {
    std::size_t length2 = std::strcspn(str + length, "@/?#");
    if (str[length + length2] == '@')
      {
      this->user_info = string_to_quark(str, length + length2);
      str += length + length2 + 1;
      }
    }
  if (*str == '[')
    {
    length = std::strcspn(str, "]");
    if (str[length] != ']')
      {
      goto error;
      }
    this->host = string_to_quark(str, length + 1);
    str += length + 1;
    if (std::strcspn(str, ":/?#") != 0)
      {
      goto error;
      }
    }
  else
    {
    length = std::strcspn(str, ":/?#");
    this->host = string_to_quark(str, length);
    str += length;
    }
  if (*str == ':')
    {
    length = std::strcspn(++str, "/?#");
    if (length == 0)
      {
      goto error;
      }
    for (std::size_t i = 0; i < length; ++i)
      {
      if (!std::isdigit(str[i]))
        {
        goto error;
        }
      }
    this->port = string_to_quark(str, length);
    str += length;
    }
  if (*str == '/')
    {
path:
    length = std::strcspn(str, "?#");
    this->path = string_to_quark(str, length);
    str += length;
    }
  else
    {
    this->path = string_to_quark("/", 1);
    }
  if (*str == '?')
    {
query:
    length = std::strcspn(++str, "#");
    this->query = string_to_quark(str, length);
    str += length;
    }
  if (*str == '#')
    {
fragment:
    ++str;
    this->fragment = string_to_quark(str, strlen(str));
    }
  return;
error:
  throw std::runtime_error(std::string("invalid url: ") + str);
  }

} // namespace Karrot
