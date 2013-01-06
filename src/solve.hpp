/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_SOLVE_HPP
#define KARROT_SOLVE_HPP

#include <karrot/deliverable.hpp>
#include <karrot/spec.hpp>
#include <vector>
#include <set>

namespace karrot
{

std::vector<int> solve(
    const std::vector<Deliverable>& database,
    const std::set<Spec>& projects);

} // namespace karrot

#endif /* KARROT_SOLVE_HPP */
