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
#include <string>

using KDictionary = std::map<std::string, std::string>;

inline void
k_dictionary_set (KDictionary *self, char const *key, char const *value)
  {
  self->emplace(key, value ? value : "");
  }

inline char const *
k_dictionary_get (KDictionary const *self, char const *key)
  {
  auto it = self->find(key);
  return it != self->end() ? it->second.c_str() : nullptr;
  }

#endif /* KARROT_DICTIONARY_HPP */
