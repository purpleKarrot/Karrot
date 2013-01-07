/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_GIT_HPP
#define KARROT_GIT_HPP

#include <karrot/driver.hpp>

namespace karrot
{

class Git: public Driver
  {
  private:
    //Type type() const
    //  {
    //  return SOURCE;
    //  }
    void download(const Deliverable& deliverable);
  };

} // namespace karrot

#endif /* KARROT_GIT_HPP */