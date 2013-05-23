/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_IMPLEMENTATION_HPP
#define KARROT_IMPLEMENTATION_HPP

#include <karrot.h>
#include "dictionary.hpp"
#include "string.hpp"
#include <iosfwd>

namespace Karrot
{
class Spec;
class Driver;
}

struct _KImplementation
  {
  _KImplementation(
    std::string const& id,
    std::string const& name,
    std::string const& component,
    std::string const& version = std::string(),
    KDictionary const& variant = KDictionary(),
    KDictionary const& values = KDictionary());
  Karrot::String id;
  Karrot::String name;
  Karrot::String component;
  Karrot::String version;
  KDictionary variant;
  KDictionary values;
  std::vector<Karrot::Spec> depends;
  std::vector<Karrot::Spec> conflicts;
  Karrot::Driver const *driver;
  };

std::ostream& operator<<(std::ostream &os, _KImplementation const& impl);

#endif /* KARROT_IMPLEMENTATION_HPP */
