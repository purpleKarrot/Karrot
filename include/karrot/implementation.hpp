/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_IMPLEMENTATION_HPP
#define KARROT_IMPLEMENTATION_HPP

#include <vector>
#include <karrot/identification.hpp>
#include <karrot/spec.hpp>

namespace karrot
{

class Driver;

class Implementation
  {
  public:
    Implementation() :
        driver(0), name(0), href(0), hash(0)
      {
      }
  public:
    Identification id;
    std::vector<Spec> depends;
    std::vector<Spec> conflicts;
    Driver* driver;
    int name;
    int href;
    int hash;
  };

} // namespace karrot

#endif /* KARROT_IMPLEMENTATION_HPP */
