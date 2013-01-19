/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_PACKAGE_HANDLER_HPP
#define KARROT_PACKAGE_HANDLER_HPP

#include <map>
#include <karrot.hpp>

namespace Karrot
{

class PackageHandler
  {
  public:
    void add(std::string&& name, std::unique_ptr<Driver>&& driver)
      {
      handlers.insert(std::make_pair(std::move(name), std::move(driver)));
      }
    Driver* get(const std::string& name) const
      {
      auto it = handlers.find(name);
      if (it != handlers.end())
        {
        return it->second.get();
        }
      return 0;
      }
  private:
    std::map<std::string, std::unique_ptr<Driver>> handlers;
  };

} // namespace Karrot

#endif /* KARROT_PACKAGE_HANDLER_HPP */
