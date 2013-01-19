/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_PACKAGE_HPP
#define KARROT_PACKAGE_HPP

#include <karrot/dictionary.hpp>
#include <karrot/identification.hpp>
#include <map>

namespace karrot
{

class Driver;

class Package
  {
  public:
    Package(int domain, int project) :
        id(domain, project), driver(0)
      {
      }
  public:
    Identification id;
    Dictionary fields;
    Driver* driver;
  };

} // namespace karrot

#endif /* KARROT_PACKAGE_HPP */
