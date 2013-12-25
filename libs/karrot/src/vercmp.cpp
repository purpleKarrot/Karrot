/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "vercmp.hpp"
#include <cctype>
#include <cstring>

namespace Karrot
{

int vercmp(std::string const& str1, std::string const& str2)
  {
  int r = 0;
  const char *s1 = str1.c_str();
  const char *s2 = str2.c_str();
  const char *e1, *e2;
  while (*s1 && *s2)
    {
    while (*s1 && !std::isalnum(*s1))
      {
      s1++;
      }
    while (*s2 && !std::isalnum(*s2))
      {
      s2++;
      }
    if (std::isdigit(*s1) || std::isdigit(*s2))
      {
      while (*s1 == '0' && std::isdigit(s1[1]))
        {
        s1++;
        }
      while (*s2 == '0' && std::isdigit(s2[1]))
        {
        s2++;
        }
      e1 = s1;
      e2 = s2;
      while (std::isdigit(*e1))
        {
        e1++;
        }
      while (std::isdigit(*e2))
        {
        e2++;
        }
      r = (e1 - s1) - (e2 - s2);
      if (r == 0)
        {
        r = std::strncmp(s1, s2, e1 - s1);
        }
      if (r != 0)
        {
        return r > 0 ? 1 : -1;
        }
      }
    else
      {
      e1 = s1;
      e2 = s2;
      while (std::isalpha(*e1))
        {
        e1++;
        }
      while (std::isalpha(*e2))
        {
        e2++;
        }
      r = (e1 - s1) - (e2 - s2);
      if (r > 0)
        {
        r = std::strncmp(s1, s2, e2 - s2);
        return r >= 0 ? 1 : -1;
        }
      if (r < 0)
        {
        r = std::strncmp(s1, s2, e1 - s1);
        return r <= 0 ? -1 : 1;
        }
      r = std::strncmp(s1, s2, e1 - s1);
      if (r)
        {
        return r > 0 ? 1 : -1;
        }
      }
    s1 = e1;
    s2 = e2;
    }
  return *s1 ? 1 : *s2 ? -1 : 0;
  }

} /* namespace Karrot */
