/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot/implementation.hpp>
#include <karrot/driver.hpp>
#include "spec.hpp"


namespace Karrot
{

std::ostream& operator<<(std::ostream &os, Implementation const& impl)
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

} // namespace Karrot
