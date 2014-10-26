/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_IMPLEMENTATION_HPP
#define KARROT_IMPLEMENTATION_HPP

#include "dictionary.hpp"
#include "string.hpp"
#include <iosfwd>

namespace Karrot
{
class Spec;
}

struct KImplementation
  {
  Karrot::String id;
  Karrot::String name;
  Karrot::String version;
  Karrot::String component;
  Karrot::String driver;
  Karrot::Dictionary values;
  std::shared_ptr<Karrot::Dictionary> meta;
  Karrot::Dictionary const* globals;
  std::vector<Karrot::Spec> depends;
  std::vector<Karrot::Spec> conflicts;
  };

std::ostream& operator<<(std::ostream &os, KImplementation const& impl);

#endif /* KARROT_IMPLEMENTATION_HPP */
