/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_QUERY_HPP
#define KARROT_QUERY_HPP

#include <karrot.h>
#include <iostream>
#include <vector>
#include "string.hpp"

namespace Karrot
{

class Query
  {
  public:
    Query() = default;
    Query(std::string const& string);
  public:
    bool empty() const
      {
      return string.get().empty();
      }
    bool evaluate(std::string const& version, const KDictionary& variants) const;
  private:
    friend std::ostream& operator<<(std::ostream &os, Query const& query)
      {
      return os << query.string;
      }
  private:
    std::vector<int> queryspace;
    String string;
  };

} // namespace Karrot

#endif /* KARROT_QUERY_HPP */
