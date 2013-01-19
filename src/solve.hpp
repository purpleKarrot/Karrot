/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_SOLVE_HPP
#define KARROT_SOLVE_HPP

#include "implementation2.hpp"
#include "spec.hpp"
#include "quark.hpp"
#include <vector>
#include <set>

namespace karrot
{

inline bool satisfies(const Implementation2& entry, const Spec& spec)
  {
  if (entry.domain != spec.domain)
    {
    return false;
    }
  if (entry.project != spec.project)
    {
    return false;
    }
  if (entry.base.component != quark_to_string(spec.component) && entry.base.component != "*" && entry.base.component != "SOURCE")
    {
    return false;
    }
  return spec.query.evaluate(entry.base.version, entry.base.variant);
  }

std::vector<int> solve(
    const std::vector<Implementation2>& database,
    const std::vector<Spec>& projects);

} // namespace karrot

#endif /* KARROT_SOLVE_HPP */
