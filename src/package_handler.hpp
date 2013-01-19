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

namespace karrot
{

class PackageHandler
  {
  public:
    void add(int name, Driver* driver)
      {
      handlers.insert(std::make_pair(name, driver));
      }
    Driver* get(int name) const
      {
      auto it = handlers.find(name);
      if (it != handlers.end())
        {
        return it->second;
        }
      return 0;
      }
  private:
    std::map<int, Driver*> handlers;
  };

} // namespace karrot

#endif /* KARROT_PACKAGE_HANDLER_HPP */
