/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_DICTIONARY_HPP
#define KARROT_DICTIONARY_HPP

#include <map>
#include "string_pool.hpp"

namespace Karrot
{

using Dictionary = std::map<int, int>;

inline char const *
get (Dictionary const& self, char const *key, StringPool& pool)
  {
  auto it = self.find(pool.from_string(key));
  return it != self.end() ? pool.to_string(it->second) : nullptr;
  }

} // namespace Karrot

#endif /* KARROT_DICTIONARY_HPP */
