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
#include <string>

namespace Karrot
{

int string_to_quark(const char* str, std::size_t len);

template<std::size_t N>
int string_to_quark(const char (&str)[N])
  {
  return string_to_quark(str, N - 1);
  }

static inline int to_quark(const std::string& string)
  {
  return string_to_quark(string.c_str(), string.length());
  }

const char* quark_to_string(int quark);

} // namespace Karrot

#endif /* KARROT_QUARK_HPP */
