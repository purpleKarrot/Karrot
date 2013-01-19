/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_QUERY_HPP
#define KARROT_QUERY_HPP

#include <karrot.hpp>
#include <vector>

namespace karrot
{

class Query
  {
  public:
    Query(const std::string& string);
    bool evaluate(const std::string& version, const Dictionary& variants) const;
  private:
    std::vector<int> queryspace;
  };

} // namespace karrot

#endif /* KARROT_QUERY_HPP */
