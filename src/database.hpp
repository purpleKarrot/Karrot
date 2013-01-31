/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_DATABASE_HPP
#define KARROT_DATABASE_HPP

#include "implementation.hpp"
#include <vector>

namespace Karrot
{

class Spec;
class Driver;

class DatabaseEntry
  {
  public:
    DatabaseEntry(const std::string& id) : id(id)
      {
      }
  public:
    std::string id;
    KImplementation impl;
    std::vector<Spec> depends;
    std::vector<Spec> conflicts;
    Driver const *driver;
  };

typedef std::vector<DatabaseEntry> Database;

} // namespace Karrot

#endif /* KARROT_DATABASE_HPP */
