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
#include <iosfwd>

namespace Karrot
{

class Spec
  {
  public:
    Spec() = default;
    Spec(
      const std::string& id,
      const std::string& component,
      const std::string& query);
    explicit Spec(char const* url);
  public:
    String id;
    String component;
    Query query;
  };

std::ostream& operator<<(std::ostream &os, Spec const& spec);

} // namespace Karrot

#endif /* KARROT_SPEC_HPP */
