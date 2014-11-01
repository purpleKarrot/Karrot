/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot.h>
#include <ostream>
#include <cstring>

namespace Karrot
{

Spec::Spec(
  const std::string& id,
  const std::string& component,
  const std::string& query, StringPool& pool)
  : id(pool.from_string(id.c_str()))
  , component(pool.from_string(component.c_str()))
  , query(query, pool)
  {
  }

Spec::Spec(char const* url, StringPool& pool)
  {
  std::size_t length = std::strcspn(url, "?#");
  id = pool.from_string(std::string(url, length).c_str());
  url += length;
  if (*url == '?')
    {
    ++url;
    length = std::strcspn(url, "#");
    query = Query(std::string(url, length), pool);
    url += length;
    }
  if (*url == '#')
    {
    ++url;
    component = pool.from_string(url);
    }
  }

bool Spec::satisfies(const Implementation& impl, const StringPool& pool) const
  {
  if (id != impl.id)
    {
    return false;
    }
  if (component != impl.component && impl.component != STR_ANY && impl.component != STR_SOURCE)
    {
    return false;
    }
  return query.evaluate(impl, pool);
  }

} // namespace Karrot
