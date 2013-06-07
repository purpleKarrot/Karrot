/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_DICTIONARY_HPP
#define KARROT_DICTIONARY_HPP

#include <karrot.h>
#include <cassert>
#include <string>
#include <map>

struct _KDictionary: std::map<std::string, std::string>
  {
  };

#endif /* KARROT_DICTIONARY_HPP */
