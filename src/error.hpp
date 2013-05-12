/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_ERROR_HPP
#define KARROT_ERROR_HPP

#include <karrot.h>
#include <exception>

struct _KError: std::exception_ptr
  {
  using std::exception_ptr::operator=;
  };

#endif /* KARROT_ERROR_HPP */
