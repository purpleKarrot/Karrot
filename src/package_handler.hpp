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
#include <memory>
#include <karrot/driver.hpp>

namespace Karrot
{

class PackageHandler
  {
  public:
    void add(std::unique_ptr<Driver> driver)
      {
      handlers.push_back(std::move(driver));
      }
    Driver const * get(const std::string& name) const
      {
      auto it = std::find_if(begin(handlers), end(handlers),
        [&name](std::unique_ptr<Driver> const& driver) -> bool
        {
        return name == driver->name();
        });
      if (it != handlers.end())
        {
        return it->get();
        }
      return nullptr;
      }
  private:
    std::vector<std::unique_ptr<Driver>> handlers;
  };

} // namespace Karrot

#endif /* KARROT_PACKAGE_HANDLER_HPP */
