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

namespace Karrot
{

class Driver;

class Package
  {
  public:
    String component;
    String version;
    Dictionary variant;
    Dictionary values;
    Dictionary fields;
    Driver const *driver = nullptr;
  };

} // namespace Karrot

#endif /* KARROT_PACKAGE_HPP */