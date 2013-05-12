/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "error.hpp"
#include <stdexcept>

void k_error_set(KError *self, char const *what)
  {
  if (!self)
    {
    return;
    }
  *self = std::make_exception_ptr(std::runtime_error(what));
  }
