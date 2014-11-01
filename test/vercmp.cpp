/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "../src/vercmp.hpp"
#include <boost/detail/lightweight_test.hpp>

int vercmp(int argc, char* argv[])
  {
  Karrot::StringPool pool;
  int version1 = pool.from_static_string("1.11");
  int version2 = pool.from_static_string("1.12");

  BOOST_TEST(Karrot::vercmp(version1, version1, pool) == 0);
  BOOST_TEST(Karrot::vercmp(version1, version1, pool) >= 0);
  BOOST_TEST(Karrot::vercmp(version1, version1, pool) <= 0);

  BOOST_TEST(Karrot::vercmp(version1, version2, pool) != 0);
  BOOST_TEST(Karrot::vercmp(version1, version2, pool) <= 0);
  BOOST_TEST(Karrot::vercmp(version1, version2, pool) < 0);

  BOOST_TEST(Karrot::vercmp(version2, version1, pool) != 0);
  BOOST_TEST(Karrot::vercmp(version2, version1, pool) >= 0);
  BOOST_TEST(Karrot::vercmp(version2, version1, pool) > 0);

  return boost::report_errors();
  }
