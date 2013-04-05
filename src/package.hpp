/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_PACKAGE_HPP
#define KARROT_PACKAGE_HPP

#include "dictionary.hpp"
#include "implementation.hpp"

namespace Karrot
{

class Driver;

class Package
  {
  public:
    Package() : driver(nullptr)
      {
      }
  public:
    KImplementation impl;
    KDictionary fields;
    Driver const *driver;
    std::string namespace_uri;
  };

} // namespace Karrot

#endif /* KARROT_PACKAGE_HPP */
