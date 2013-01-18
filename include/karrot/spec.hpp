/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_SPEC_HPP
#define KARROT_SPEC_HPP

namespace karrot
{

class Url;

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
    bool operator<(const Spec& other) const
      {
      if (domain < other.domain)
        {
        return true;
        }
      if (domain > other.domain)
        {
        return false;
        }
      if (project < other.project)
        {
        return true;
        }
      if (project > other.project)
        {
        return false;
        }
      if (component < other.component)
        {
        return true;
        }
      if (component > other.component)
        {
        return false;
        }
      return query < other.query;
      }
  public:
    int domain;
    int project;
    int component;
    int query;
  };

} // namespace karrot

#endif /* KARROT_SPEC_HPP */
