/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "spec.hpp"
#include "url.hpp"
#include "quark.hpp"

namespace karrot
{

Spec::Spec(const Url& url)
    : id(std::string(quark_to_string(url.host)) + quark_to_string(url.path))
    , component(quark_to_string(url.fragment))
    , query(quark_to_string(url.query))
  {
  }

} // namespace karrot
