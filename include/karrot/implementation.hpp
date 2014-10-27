/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_IMPLEMENTATION_HPP
#define KARROT_IMPLEMENTATION_HPP

#include "dictionary.hpp"
#include "string.hpp"

namespace Karrot
{

class Spec;

struct Implementation
  {
  String id;
  String version;
  String component;
  Dictionary values;
  std::vector<Spec> depends;
  std::vector<Spec> conflicts;
  };

} // namespace Karrot

#endif /* KARROT_IMPLEMENTATION_HPP */
