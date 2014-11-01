/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_QUERY_HPP
#define KARROT_QUERY_HPP

#include <ostream>
#include <vector>
#include "dictionary.hpp"
#include "string_pool.hpp"

namespace Karrot
{

class Query
  {
  public:
    Query() = default;
    Query(std::string string, StringPool& pool);
  public:
    explicit operator bool() const
      {
      return !compiled.empty();
      }
    bool evaluate(int version, const Dictionary& variants, StringPool const& pool) const;
  private:
    friend std::ostream& operator<<(std::ostream &os, Query const& query)
      {
      return os << query.str;
      }
    friend bool operator!=(Query const& q1, Query const& q2)
      {
      return q1.str != q2.str;
      }
  private:
    std::string str;
    std::vector<int> compiled;
  };

} // namespace Karrot

#endif /* KARROT_QUERY_HPP */
