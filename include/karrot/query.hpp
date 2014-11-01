/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_QUERY_HPP
#define KARROT_QUERY_HPP

#include <vector>
#include "dictionary.hpp"
#include "string_pool.hpp"

namespace Karrot
{

class Query
  {
  public:
    Query() = default;
    Query(std::string const& string, StringPool& pool);
  public:
    explicit operator bool() const
      {
      return !compiled.empty();
      }
    std::string to_string(StringPool const& pool) const;
    bool evaluate(int version, const Dictionary& variants, StringPool const& pool) const;
  private:
    friend bool operator!=(Query const& q1, Query const& q2)
      {
      return q1.compiled != q2.compiled;
      }
  private:
    std::vector<int> compiled;
  };

} // namespace Karrot

#endif /* KARROT_QUERY_HPP */
