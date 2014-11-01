/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_VARIANTS_HPP
#define KARROT_VARIANTS_HPP

#include <karrot.h>
#include <functional>
#include <vector>
#include <map>

namespace Karrot
{

std::map<int, int> parse_variant(const std::string& quark, StringPool& pool);

} // namespace Karrot

#endif /* KARROT_VARIANTS_HPP */