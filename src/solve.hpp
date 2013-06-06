/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_SOLVE_HPP
#define KARROT_SOLVE_HPP

#include "engine.hpp"
#include "implementation.hpp"
#include "spec.hpp"

namespace Karrot
{

inline bool satisfies(const KImplementation& impl, const Spec& spec)
  {
  if (impl.id != spec.id)
    {
    return false;
    }
  if (impl.component != spec.component && impl.component != "*" && impl.component != "SOURCE")
    {
    return false;
    }
  return spec.query.evaluate(impl.version, impl.variant);
  }

bool solve(
    Database const& database,
    Requests const& requests,
    bool ignore_source_conflicts,
    KPrintFun log,
    std::vector<int>& model);

} // namespace Karrot

#endif /* KARROT_SOLVE_HPP */
