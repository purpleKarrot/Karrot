/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "implementation.hpp"
#include "spec.hpp"

_KImplementation::_KImplementation(
  std::string const& id,
  std::string const& name,
  std::string const& component,
  std::string const& version,
  KDictionary const& variant,
  KDictionary const& values)
  : id(id)
  , name(name)
  , component(component)
  , version(version)
  , variant(variant)
  , values(values)
  {
  }

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
k_implementation_get_component(KImplementation const *self)
  {
  return c_str(self->component);
  }

char const *
k_implementation_get_version(KImplementation const *self)
  {
  return c_str(self->version);
  }

KDictionary const *
k_implementation_get_variant(KImplementation const *self)
  {
  return &self->variant;
  }

KDictionary const *
k_implementation_get_values(KImplementation const *self)
  {
  return &self->values;
  }

std::ostream& operator<<(std::ostream &os, _KImplementation const& impl)
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
  return os;
  }
