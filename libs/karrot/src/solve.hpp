/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_SOLVE_HPP
#define KARROT_SOLVE_HPP

#include <karrot/engine.hpp>
#include <karrot/implementation.hpp>
#include "types.hpp"
#include "spec.hpp"

namespace Karrot
{

inline bool satisfies(const Implementation& impl, const Spec& spec)
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
    Solution& model);

} // namespace Karrot

#endif /* KARROT_SOLVE_HPP */
