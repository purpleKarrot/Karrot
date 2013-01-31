/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_IMPLEMENTATION_HPP
#define KARROT_IMPLEMENTATION_HPP

#include <karrot.h>
#include "dictionary.hpp"

struct _KImplementation
  {
  std::string name;
  std::string component;
  std::string version;
  KDictionary variant;
  KDictionary values;
  };

#endif /* KARROT_IMPLEMENTATION_HPP */
