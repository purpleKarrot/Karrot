/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_VARIANTS_HPP
#define KARROT_VARIANTS_HPP

#include "dictionary.hpp"
#include <functional>
#include <vector>

namespace Karrot
{

Dictionary parse_variant(const std::string& quark);

void foreach_variant(const Dictionary& variants,
    const std::function<void(Dictionary)>& function);

} // namespace Karrot

#endif /* KARROT_VARIANTS_HPP */
