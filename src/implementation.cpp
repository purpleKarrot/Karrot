/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "implementation.hpp"

char const *
k_impl_get_name(KImplementation const *self)
  {
  return self->name.c_str();
  }

char const *
k_impl_get_component(KImplementation const *self)
  {
  return self->component.c_str();
  }

char const *
k_impl_get_version(KImplementation const *self)
  {
  return self->version.c_str();
  }

KDictionary const *
k_impl_get_variant(KImplementation const *self)
  {
  return &self->variant;
  }

KDictionary const *
k_impl_get_values(KImplementation const *self)
  {
  return &self->values;
  }
