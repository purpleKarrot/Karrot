/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_VERCMP_HPP
#define KARROT_VERCMP_HPP

#include <karrot/string_pool.hpp>

namespace Karrot
{

int vercmp(int str1, int str2, StringPool const& pool);

} // namespace Karrot

#endif /* KARROT_VERCMP_HPP */
