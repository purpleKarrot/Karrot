/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_QUERY_HPP
#define KARROT_QUERY_HPP

namespace karrot
{

int parse_query(int quark);

bool evaluate(int query, int version, int variants);

} // namespace karrot

#endif /* KARROT_QUERY_HPP */
