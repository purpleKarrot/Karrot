/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "implementation.hpp"
#include "driver.hpp"
#include "spec.hpp"

static inline const char *
c_str(std::string const& str)
  {
  return str.c_str();
  }

char const *
k_implementation_get_id(KImplementation const *self)
  {
  return c_str(self->id);
  }

char const *
k_implementation_get_name(KImplementation const *self)
  {
  return c_str(self->name);
  }

char const *
k_implementation_get_version(KImplementation const *self)
  {
  return c_str(self->version);
  }

void
k_implementation_set_version(KImplementation *self, char const *value)
  {
  self->version = value;
  }

char const *
k_implementation_get_component(KImplementation const *self)
  {
  return c_str(self->component);
  }

void
k_implementation_set_component(KImplementation *self, char const *value)
  {
  self->component = value;
  }

char const *
k_implementation_get_driver (KImplementation const *self)
  {
  self->driver->name().c_str();
  }

char const *
k_implementation_get_meta(KImplementation const *self, char const *key)
  {
  return k_dictionary_get(&*self->meta, key);
  }

char const *
k_implementation_get_variant(KImplementation const *self, char const *key)
  {
  return k_dictionary_get(&self->variant, key);
  }

void
k_implementation_foreach_variant(KImplementation const *self, KVisit visit, void *target)
  {
  k_dictionary_foreach(&self->variant, visit, target);
  }

char const *
k_implementation_get_value(KImplementation const *self, char const *key)
  {
  return k_dictionary_get(&self->values, key);
  }

void
k_implementation_set_value(KImplementation *self, char const *key, char const *value)
  {
  k_dictionary_set(&self->values, key, value);
  }

char const *
k_implementation_get_global(KImplementation const *self, char const *key)
  {
  return k_dictionary_get(self->globals, key);
  }

std::ostream& operator<<(std::ostream &os, KImplementation const& impl)
  {
  os << impl.id;
  if (!impl.version.get().empty())
    {
    os << "?version==" << impl.version;
    }
  if (!impl.component.get().empty())
    {
    os << '#' << impl.component;
    }
  os << " (" << impl.driver->name() << ')';
  return os;
  }
