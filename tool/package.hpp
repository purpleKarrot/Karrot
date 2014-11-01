/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_PACKAGE_HPP
#define KARROT_PACKAGE_HPP

#include <karrot.h>
#include <map>

namespace Karrot
{

class Driver;

class Package
  {
  public:
    int component;
    int version;
    std::map<int, int> variant;
    std::map<int, int> values;
    std::map<int, int> fields;
    Driver const *driver = nullptr;
  };

} // namespace Karrot

#endif /* KARROT_PACKAGE_HPP */
