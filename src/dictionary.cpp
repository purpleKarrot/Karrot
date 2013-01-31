/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "dictionary.hpp"

void
k_dict_foreach(KDictionary const *self, KMapping mapping, void *target)
  {
  for (const auto& entry : *self)
    {
    mapping(entry.first.c_str(), entry.second.c_str(), target);
    }
  }

char const *
k_dict_lookup(KDictionary const *self, char const *str)
  {
  auto it = self->find(str);
  return it != self->end() ? it->second.c_str() : nullptr;
  }
