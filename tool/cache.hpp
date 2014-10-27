/*
 * Copyright (C) 2014 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_CACHE_HPP
#define KARROT_CACHE_HPP

#include <string>
#include <vector>


namespace Karrot
{

struct Implementation;
using Database = std::vector<Implementation>;

void
write_cache(
    std::string const& filename,
    std::vector<int> const& model,
    Database const& database);

} // namespace Karrot

#endif /* KARROT_CACHE_HPP */
