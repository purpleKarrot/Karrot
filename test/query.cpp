/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot.h>
#include "../tool/variants.hpp"
#include <boost/detail/lightweight_test.hpp>

int query(int argc, char* argv[])
  {
  Karrot::StringPool pool;
  Karrot::Implementation impl(0);
  impl.version = pool.from_string("3.4");
  impl.set(pool.from_static_string("xx"), pool.from_static_string("foo"));
  impl.set(pool.from_static_string("yy"), pool.from_static_string("bar"));
  Karrot::Query qq("version==3.4&&xx==foo&&yy==bar", pool);
  BOOST_TEST(qq.evaluate(impl, pool));
  return boost::report_errors();
  }
