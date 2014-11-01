// Copyright (c) 2014, Daniel Pfeifer <daniel@pfeifer-mail.de>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#ifndef KARROT_PRINT_INCLUDED
#define KARROT_PRINT_INCLUDED

#include <karrot/implementation.hpp>
#include <karrot/spec.hpp>
#include <karrot/string_pool.hpp>
#include <ostream>

namespace Karrot
{

struct ImplPrinter
{
  friend std::ostream& operator<<(std::ostream &os, ImplPrinter const& p)
    {
    os << p.pool.to_string(p.impl.id);
    if (p.impl.component)
      {
      os << '#' << p.pool.to_string(p.impl.component);
      }
    if (p.impl.version)
      {
      os << ' ' << p.pool.to_string(p.impl.version);
      }
    return os;
    }

  Implementation const& impl;
  StringPool const& pool;
};

struct SpecPrinter
{
  friend std::ostream& operator<<(std::ostream &os, SpecPrinter const& p)
    {
    os << p.pool.to_string(p.spec.id);
    if (p.spec.query)
      {
      os << '?' << p.spec.query.to_string(p.pool);
      }
    if (p.spec.component)
      {
      os << '#' << p.pool.to_string(p.spec.component);
      }
    return os;
    }

  Spec const& spec;
  StringPool const& pool;
};

} // namespace Karrot

#endif /* KARROT_PRINT_INCLUDED */
