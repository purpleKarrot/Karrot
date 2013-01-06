/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_DELIVERABLE_HPP
#define KARROT_DELIVERABLE_HPP

#include <vector>
#include <karrot/identification.hpp>
#include <karrot/spec.hpp>

namespace karrot
{

class Driver;

class Deliverable
  {
  public:
    Deliverable() :
        driver(0), folder(0), href(0), hash(0)
      {
      }
  public:
    Identification id;
    std::vector<Spec> depends;
    std::vector<Spec> conflicts;
    Driver* driver;
    int folder;
    int href;
    int hash;
  };

} // namespace karrot

#endif /* KARROT_DELIVERABLE_HPP */
