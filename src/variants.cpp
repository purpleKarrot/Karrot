/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "variants.hpp"
#include "quark_internal.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace karrot
{

static int __sort_lt(int a, int b)
  {
  return std::strcmp(quark_to_string(a), quark_to_string(b)) < 0;
  }

static inline void builder_sort(std::vector<int>& builder)
  {
  std::vector<int>::iterator s = builder.begin();
  std::vector<int>::iterator t = builder.end();
  std::vector<int>::iterator i, j;
  for (i = s + 2; i < t; i += 2)
    {
    for (j = i; j > s && __sort_lt(*j, *(j - 2)); j -= 2)
      {
      std::swap(*(j + 0), *(j - 2));
      std::swap(*(j + 1), *(j - 1));
      }
    }
  }

int parse_variant(int quark)
  {
  if (!quark)
    {
    return 0;
    }
  // make a copy because string_to_quark may realloc
  std::string string(quark_to_string(quark));
  const char* str = string.c_str();
  std::vector<int> builder;
next:
  std::size_t length = std::strcspn(str, "=;");
  int key = string_to_quark(str, length);
  if (str[length] != '=')
    {
    throw std::runtime_error(std::string("invalid dict: ") + str);
    }
  str += length + 1;
  length = std::strcspn(str, "=;");
  int val = string_to_quark(str, length);
  if (str[length] == '=')
    {
    throw std::runtime_error(std::string("invalid dict: ") + str);
    }
  builder.push_back(key);
  builder.push_back(val);
  if (str[length] == ';')
    {
    str += length + 1;
    goto next;
    }
  builder_sort(builder);
  return array_to_quark(&builder[0], builder.size());
  }

int lookup(int variant, int var)
  {
  for (const int* quark = quark_to_array(variant); *quark; quark += 2)
    {
    if (var == *quark)
      {
      return *(quark + 1);
      }
    }
  std::cout << "warning: unset variable: " << quark_to_string(var) << '!' << std::endl;
  return 0;
  }

static void
r_variants_recurse(
    const int *dict, std::size_t size,
    std::vector<int>* builder,
    const std::function<void(int)>& func)
  {
  const char *c, *v;
  for (c = v = quark_to_string(dict[1]);; ++c)
    {
    int ee = (*c == '\0');
    int ff = (*c == ';');
    if (ee || ff)
      {
      int qq = string_to_quark(v, c - v);
      if (!qq)
        {
        std::vector<int> bb(*builder);
        bb.push_back(dict[0]);
        bb.push_back(qq);
        if (size > 2)
          {
          const int* dict2 = dict + 2;
          size_t size2 = size - 2;
          r_variants_recurse(dict2, size2, &bb, func);
          }
        else
          {
          builder_sort(bb);
          func(array_to_quark(&bb[0], bb.size()));
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

void foreach_variant(const std::vector<int>& variants,
    const std::function<void(int)>& func)
  {
  if (variants.empty())
    {
    func(0);
    }
  else
    {
    std::vector<int> builder;
    r_variants_recurse(&variants[0], variants.size(), &builder, func);
    }
  }

} // namespace karrot
