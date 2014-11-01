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
#include "string_pool.hpp"
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
      const std::string& query, StringPool& pool);
    Spec(char const* url, StringPool& pool);
  public:
    int id = 0;
    int component = 0;
    Query query;
  };

} // namespace Karrot

#endif /* KARROT_SPEC_HPP */
