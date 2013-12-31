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
#include <karrot/string.hpp>

namespace Karrot
{

using Dictionary = std::map<String, String>;

inline void
set (Dictionary& self, char const *key, char const *value)
  {
  if (value)
    {
    self.emplace(String{key}, String{value});
    }
  else
    {
    self.erase(String{key});
    }
  }

inline char const *
get (Dictionary const& self, char const *key)
  {
  auto it = self.find(String{key});
  return it != self.end() ? it->second.get().c_str() : nullptr;
  }

} // namespace Karrot

#endif /* KARROT_DICTIONARY_HPP */