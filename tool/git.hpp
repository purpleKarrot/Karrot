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

namespace Karrot
{

class Git: public Driver
  {
  private:
    void download(const Implementation& impl, bool requested) override;
  };

} // namespace Karrot

#endif /* KARROT_GIT_HPP */
