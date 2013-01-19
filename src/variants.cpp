/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "variants.hpp"
#include "quark.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace Karrot
{

Dictionary parse_variant(const std::string& string)
  {
  if (string.empty())
    {
    return Dictionary();
    }
  const char* str = string.c_str();
  Dictionary builder;
next:
  std::size_t length = std::strcspn(str, "=;");
  std::string key(str, str + length);
  if (str[length] != '=')
    {
    throw std::runtime_error(std::string("invalid dict: ") + str);
    }
  str += length + 1;
  length = std::strcspn(str, "=;");
  std::string val(str, str + length);
  if (str[length] == '=')
    {
    throw std::runtime_error(std::string("invalid dict: ") + str);
    }
  builder[key] = val;
  if (str[length] == ';')
    {
    str += length + 1;
    goto next;
    }
  return builder;
  }

static void r_variants_recurse(const std::string *dict, std::size_t size,
    Dictionary* builder, const std::function<void(Dictionary)>& func)
  {
  const char *c, *v;
  for (c = v = dict[1].c_str();; ++c)
    {
    int ee = (*c == '\0');
    int ff = (*c == ';');
    if (ee || ff)
      {
      int qq = string_to_quark(v, c - v);
      if (!qq)
        {
        Dictionary bb(*builder);
        bb[dict[0]] = qq;
        if (size > 2)
          {
          const std::string* dict2 = dict + 2;
          size_t size2 = size - 2;
          r_variants_recurse(dict2, size2, &bb, func);
          }
        else
          {
          func(bb);
          }
        }
      }
    if (ee)
      {
      break;
      }
    if (ff)
      {
      v = c + 1;
      }
    }
  }

void foreach_variant(const std::vector<std::string>& variants,
    const std::function<void(Dictionary)>& func)
  {
  if (variants.empty())
    {
    func(Dictionary());
    }
  else
    {
    Dictionary builder;
    r_variants_recurse(&variants[0], variants.size(), &builder, func);
    }
  }

} // namespace Karrot
