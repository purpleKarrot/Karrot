/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_SOLVE_HPP
#define KARROT_SOLVE_HPP

#include "database.hpp"
#include "spec.hpp"
#include "quark.hpp"
#include <vector>
#include <set>

namespace Karrot
{

inline bool satisfies(const DatabaseEntry& entry, const Spec& spec)
  {
  if (entry.id != spec.id)
    {
    return false;
    }
  const KImplementation& impl = entry.impl;
  if (impl.component != spec.component && impl.component != "*" && impl.component != "SOURCE")
    {
    return false;
    }
  return spec.query.evaluate(impl.version, impl.variant);
  }

typedef std::vector<Spec> Requests;

bool solve(const Database& database, const Requests& requests, std::vector<int>& model);

} // namespace Karrot

#endif /* KARROT_SOLVE_HPP */
