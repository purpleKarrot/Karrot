/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "variants.hpp"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace Karrot
{

KDictionary parse_variant(const std::string& string)
  {
  if (string.empty())
    {
    return KDictionary();
    }
  const char* str = string.c_str();
  KDictionary builder;
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

static void r_variants_recurse(
    const KDictionary::const_iterator& cur,
    const KDictionary::const_iterator& end,
    const KDictionary& dict,
    const std::function<void(KDictionary)>& func)
  {
  assert(cur != end);
  std::vector < std::string > values;
  split(values, cur->second, boost::is_any_of(";"), boost::token_compress_on);
  for (const std::string& val : values)
    {
    KDictionary copy(dict);
    copy.insert(std::make_pair(cur->first, val));
    if (std::next(cur) == end)
      {
      func(copy);
      }
    else
      {
      r_variants_recurse(std::next(cur), end, copy, func);
      }
    }
  }

void foreach_variant(
    const KDictionary& variants,
    const std::function<void(KDictionary)>& func)
  {
  if (variants.empty())
    {
    func(KDictionary());
    }
  else
    {
    r_variants_recurse(variants.begin(), variants.end(), KDictionary(), func);
    }
  }

} // namespace Karrot
