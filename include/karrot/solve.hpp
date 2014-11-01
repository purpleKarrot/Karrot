/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_SOLVE_HPP
#define KARROT_SOLVE_HPP

#include "implementation.hpp"
#include "string_pool.hpp"
#include "spec.hpp"

namespace Karrot
{

using Requests = std::vector<Spec>;
using Database = std::vector<Implementation>;

inline bool satisfies(const Implementation& impl, const Spec& spec, StringPool const& pool)
  {
  if (impl.id != spec.id)
    {
    return false;
    }
  if (impl.component != spec.component && impl.component != STR_ANY && impl.component != STR_SOURCE)
    {
    return false;
    }
  return spec.query.evaluate(impl.version, impl.values, pool);
  }

bool solve(
    Database const& database,
    Requests const& requests,
    std::vector<int>& model, StringPool& pool);

} // namespace Karrot

#endif /* KARROT_SOLVE_HPP */
