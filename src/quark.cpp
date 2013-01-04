/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot/quark.hpp>

namespace
{

std::size_t str_hash(const char* str)
  {
  std::size_t r = 0;
  std::size_t c;
  while ((c = *(const unsigned char*) str++) != 0)
    {
    r += (r << 3) + c;
    }
  return r;
  }

std::size_t str_hash(const char* str, std::size_t len)
  {
  std::size_t r = 0;
  std::size_t c;
  while (len-- && (c = *(const unsigned char*) str++) != 0)
    {
    r += (r << 3) + c;
    }
  return r;
  }

} // namespace

#include "quark_impl.hpp"

namespace karrot
{

static QuarkImpl<char> impl;

void Quark::init(const char* str, std::size_t len)
  {
  id = impl.to_int(str, len);
  }

const char* Quark::str() const
  {
  return impl.to_str(id);
  }

} // namespace karrot
