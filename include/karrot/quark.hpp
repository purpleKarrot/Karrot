/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_QUARK_HPP
#define KARROT_QUARK_HPP

#include <cstddef>
#include <karrot/export.hpp>

namespace karrot
{

KARROT_DECL int string_to_quark(const char* str, std::size_t len);

template<std::size_t N>
int string_to_quark(const char (&str)[N])
  {
  return string_to_quark(str, N - 1);
  }

KARROT_DECL const char* quark_to_string(int quark);

} // namespace karrot

#endif /* KARROT_QUARK_HPP */
