/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "implementation.hpp"

char const *
k_impl_get_name(KImplementation *self)
  {
  return self->name.c_str();
  }

char const *
k_impl_get_component(KImplementation *self)
  {
  return self->component.c_str();
  }

char const *
k_impl_get_version(KImplementation *self)
  {
  return self->version.c_str();
  }

KDictionary *
k_impl_get_variant(KImplementation *self)
  {
  return &self->variant;
  }

KDictionary *
k_impl_get_values(KImplementation *self)
  {
  return &self->values;
  }
