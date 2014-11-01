/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_SOLVE_HPP
#define KARROT_SOLVE_HPP

#include <vector>

namespace Karrot
{

class Spec;
class StringPool;
struct Implementation;

using Requests = std::vector<Spec>;
using Database = std::vector<Implementation>;

bool solve(
    Database const& database,
    Requests const& requests,
    std::vector<int>& model, StringPool& pool);

} // namespace Karrot

#endif /* KARROT_SOLVE_HPP */
