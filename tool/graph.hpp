/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_GRAPH_HPP
#define KARROT_GRAPH_HPP

#include "engine.hpp"

namespace Karrot
{

std::vector<int>
topological_sort(
    std::vector<int> const& model,
    Database const& database);

void
write_graphviz(
    std::string const& filename,
    std::vector<int> const& model,
    Database const& database);

} // namespace Karrot

#endif /* KARROT_GRAPH_HPP */
