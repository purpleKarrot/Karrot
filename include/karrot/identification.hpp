/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_IDENTIFICATION_HPP
#define KARROT_IDENTIFICATION_HPP

namespace karrot
{

class Identification
  {
  public:
    Identification() :
        domain(0), project(0), component(0), version(0), variant(0)
      {
      }
    Identification(int domain, int project) :
        domain(domain), project(project), component(0), version(0), variant(0)
      {
      }
  public:
    int domain;
    int project;
    int component;
    int version;
    int variant;
  };

} // namespace karrot

#endif /* KARROT_IDENTIFICATION_HPP */
