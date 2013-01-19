/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_QUARK_INTERNAL_HPP
#define KARROT_QUARK_INTERNAL_HPP

#include "quark.hpp"

namespace karrot
{

int array_to_quark(const int* array, std::size_t size);

const int* quark_to_array(int quark);

} // namespace karrot

#endif /* KARROT_QUARK_INTERNAL_HPP */
