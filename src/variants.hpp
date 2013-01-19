/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_VARIANTS_HPP
#define KARROT_VARIANTS_HPP

#include <karrot.hpp>
#include <functional>
#include <vector>

namespace karrot
{

Dictionary parse_variant(const std::string& quark);

int lookup(int variant, int var);

void foreach_variant(const std::vector<std::string>& variants,
    const std::function<void(Dictionary)>& function);

} // namespace karrot

#endif /* KARROT_VARIANTS_HPP */
