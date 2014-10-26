/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_SPEC_HPP
#define KARROT_SPEC_HPP

#include "query.hpp"
#include "string.hpp"
#include <iostream>
#include <cstring>

namespace Karrot
{

class Spec
  {
  public:
    Spec() = default;
    Spec(
      const std::string& id,
      const std::string& component,
      const std::string& query)
      : id(id)
      , component(component)
      , query(query)
      {
      }
    explicit Spec(char const* url)
      {
      std::size_t length = std::strcspn(url, "?#");
      id = std::string(url, length);
      url += length;
      if (*url == '?')
        {
        ++url;
        length = std::strcspn(url, "#");
        query = Query(String(url, length));
        url += length;
        }
      if (*url == '#')
        {
        ++url;
        component = String(url);
        }
      }
    friend std::ostream& operator<<(std::ostream &os, Spec const& spec)
      {
      os << spec.id;
      if (!spec.query.empty())
        {
        os << '?' << spec.query;
        }
      if (!spec.component.get().empty())
        {
        os << '#' << spec.component;
        }
      return os;
      }
  public:
    String id;
    String component;
    Query query;
  };

} // namespace Karrot

#endif /* KARROT_SPEC_HPP */
