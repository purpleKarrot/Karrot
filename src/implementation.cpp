/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot/implementation.hpp>
#include "driver.hpp"
#include <karrot/spec.hpp>

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
  return self->driver->name().c_str();
  }

char const *
k_implementation_get_meta(KImplementation const *self, char const *key)
  {
  return Karrot::get(*self->meta, key);
  }

char const *
k_implementation_get_variant(KImplementation const *self, char const *key)
  {
  return Karrot::get(self->variant, key);
  }

void
k_implementation_foreach_variant(KImplementation const *self, KVisit visit, void *target)
  {
  for (auto& entry : self->variant)
    {
    visit(target, c_str(entry.first), c_str(entry.second));
    }
  }

char const *
k_implementation_get_value(KImplementation const *self, char const *key)
  {
  return Karrot::get(self->values, key);
  }

void
k_implementation_set_value(KImplementation *self, char const *key, char const *value)
  {
  Karrot::set(self->values, key, value);
  }

char const *
k_implementation_get_global(KImplementation const *self, char const *key)
  {
  return Karrot::get(*self->globals, key);
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
