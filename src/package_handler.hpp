/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_PACKAGE_HANDLER_HPP
#define KARROT_PACKAGE_HANDLER_HPP

#include <vector>
#include <algorithm>
#include "driver.hpp"

namespace Karrot
{

class PackageHandler
  {
  public:
    void add(std::string const& name, std::string const& xmlns, KDriver const* driver)
      {
      handlers.emplace_back(name, xmlns, driver);
      }
    Driver const * get(const std::string& name) const
      {
      auto it = std::find_if(begin(handlers), end(handlers),
        [&name](Driver const& driver) -> bool
        {
        return name == driver.name();
        });
      if (it != handlers.end())
        {
        return &*it;
        }
      return nullptr;
      }
  private:
    std::vector<Driver> handlers;
  };

} // namespace Karrot

#endif /* KARROT_PACKAGE_HANDLER_HPP */
