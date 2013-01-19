/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_IMPLEMENTATION2_HPP
#define KARROT_IMPLEMENTATION2_HPP

#include <karrot.hpp>
#include "spec.hpp"
#include <vector>

namespace karrot
{

class Driver;

class Implementation2
  {
  public:
    int domain;
    int project;
    Implementation base;
    std::vector<Spec> depends;
    std::vector<Spec> conflicts;
    Driver* driver;
  };

} // namespace karrot

#endif /* KARROT_IMPLEMENTATION2_HPP */
