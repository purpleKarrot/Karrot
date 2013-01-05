/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_DELIVERABLE_HPP
#define KARROT_DELIVERABLE_HPP

#include <vector>
#include <karrot/identification.hpp>
#include <karrot/plug.hpp>

namespace karrot
{

class Deliverable
  {
  public:
    Deliverable() :
        channel(0), folder(0), href(0), hash(0)
      {
      }
  public:
    Identification id;
    std::vector<Reference> depends;
    std::vector<Reference> conflicts;
    Plug* channel;
    int folder;
    int href;
    int hash;
  };

} // namespace karrot

#endif /* KARROT_DELIVERABLE_HPP */
