/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_TYPES_HPP
#define KARROT_TYPES_HPP

#include <vector>

namespace Karrot
{

class Spec;
class Implementation;

using Requests = std::vector<Spec>;
using Database = std::vector<Implementation>;
using Solution = std::vector<int>;

} // namespace Karrot

#endif /* KARROT_TYPES_HPP */
