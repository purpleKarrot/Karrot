/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot/spec.hpp>
#include "query.hpp"

namespace karrot
{

Spec::Spec(const Url& url) :
    domain(url.host),
    project(url.path),
    component(url.fragment),
    query(parse_query(url.query))
  {
  }

} // namespace karrot
