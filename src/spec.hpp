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

namespace karrot
{

class Url;

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
    explicit Spec(const Url& url);
  public:
    std::string id;
    std::string component;
    Query query;
  };

} // namespace karrot

#endif /* KARROT_SPEC_HPP */
