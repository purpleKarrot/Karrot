/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_SPEC_HPP
#define KARROT_SPEC_HPP

#include <karrot/url.hpp>

namespace karrot
{

class Spec
  {
  public:
    Spec() :
        domain(0), project(0), component(0), query(0)
      {
      }
    Spec(int domain, int project, int component, int query) :
        domain(domain), project(project), component(component), query(query)
      {
      }
    explicit Spec(const Url& url);
  public:
    int domain;
    int project;
    int component;
    int query;
  };

} // namespace karrot

#endif /* KARROT_SPEC_HPP */
