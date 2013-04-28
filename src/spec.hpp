/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_SPEC_HPP
#define KARROT_SPEC_HPP

#include "quark.hpp"
#include "query.hpp"
#include "url.hpp"
#include <iostream>

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
    explicit Spec(const Url& url)
      : id(url_to_string(url))
      , component(quark_to_string(url.fragment))
      , query_str(quark_to_string(url.query))
      , query(query_str)
      {
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
