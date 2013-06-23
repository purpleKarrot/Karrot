/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "dictionary.hpp"

KDictionary *
k_dictionary_new ()
  {
  return new KDictionary;
  }

void
k_dictionary_free (KDictionary *self)
  {
  delete self;
  }

void
k_dictionary_set (KDictionary *self, char const *key, char const *value)
  {
  self->emplace(key, value ? value : "");
  }

char const *
k_dictionary_get (KDictionary const *self, char const *key)
  {
  auto it = self->find(key);
  return it != self->end() ? it->second.c_str() : nullptr;
  }

void
k_dictionary_foreach (KDictionary const *self, KVisit visit, void *target)
  {
  for (auto& entry : *self)
    {
    if (visit(target, entry.first.c_str(), entry.second.c_str()))
      {
      return;
      }
    }
  }
