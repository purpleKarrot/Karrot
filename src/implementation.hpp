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

struct KImplementation
  {
  Karrot::String id;
  Karrot::String name;
  Karrot::String version;
  Karrot::String component;
  Karrot::Driver const *driver;
  KDictionary variant;
  KDictionary values;
  std::shared_ptr<KDictionary> meta;
  KDictionary const* globals;
  std::vector<Karrot::Spec> depends;
  std::vector<Karrot::Spec> conflicts;
  };

std::ostream& operator<<(std::ostream &os, KImplementation const& impl);

#endif /* KARROT_IMPLEMENTATION_HPP */
