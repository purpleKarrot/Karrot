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
      , query_str(query)
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
        query_str = std::string(url, length);
        query = Query(query_str);
        url += length;
        }
      if (*url == '#')
        {
        ++url;
        component = std::string(url, strlen(url));
        }
      }
    friend std::ostream& operator<<(std::ostream &os, Spec const& spec)
      {
      os << spec.id;
      if (!spec.query_str.empty())
        {
        os << '?' << spec.query_str;
        }
      if (!spec.component.empty())
        {
        os << '#' << spec.component;
        }
      return os;
      }
  public:
    std::string id;
    std::string component;
    std::string query_str;
    Query query;
  };

} // namespace Karrot

#endif /* KARROT_SPEC_HPP */
