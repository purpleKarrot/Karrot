/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_IMPLEMENTATION_HPP
#define KARROT_IMPLEMENTATION_HPP

#include <karrot/module.hpp>
#include <iosfwd>

namespace Karrot
{
class Spec;

struct Implementation: Module
{
  Implementation() = default;
  Implementation(Module module) :
      Module(std::move(module))
  {
  }

  std::vector<Spec> depends;
  std::vector<Spec> conflicts;
};

std::ostream& operator<<(std::ostream &os, Implementation const& impl);

} // namespace Karrot

#endif /* KARROT_IMPLEMENTATION_HPP */
