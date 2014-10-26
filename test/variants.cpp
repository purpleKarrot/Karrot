/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "../tool/variants.cpp"
#include <boost/detail/lightweight_test.hpp>

int variants(int argc, char* argv[])
  {
  Karrot::Dictionary dict = Karrot::parse_variant("xx=foo;yy=bar");
  return boost::report_errors();
  }
