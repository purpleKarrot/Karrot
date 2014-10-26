/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot/implementation.hpp>
#include <karrot/spec.hpp>

std::ostream& operator<<(std::ostream &os, KImplementation const& impl)
  {
  os << impl.id;
  if (!impl.component.get().empty())
    {
    os << '#' << impl.component;
    }
  if (!impl.version.get().empty())
    {
    os << ' ' << impl.version;
    }
  return os;
  }
