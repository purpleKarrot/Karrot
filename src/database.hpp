/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_DATABASE_HPP
#define KARROT_DATABASE_HPP

#include <karrot.hpp>
#include <vector>

namespace karrot
{

class Spec;
class Driver;

struct DatabaseEntry
  {
  int domain;
  int project;
  Implementation base;
  std::vector<Spec> depends;
  std::vector<Spec> conflicts;
  Driver* driver;
  };

typedef std::vector<DatabaseEntry> Database;

} // namespace karrot

#endif /* KARROT_DATABASE_HPP */
