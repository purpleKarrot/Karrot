/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "../src/version.cpp"
#include <boost/detail/lightweight_test.hpp>
#include <cstring>

int version(int argc, char* argv[])
  {
  int major, minor, patch;
  const char* str = k_version(&major, &minor, &patch);
  BOOST_TEST_EQ(major, KARROT_VER_MAJOR);
  BOOST_TEST_EQ(minor, KARROT_VER_MINOR);
  BOOST_TEST_EQ(patch, KARROT_VER_PATCH);
  BOOST_TEST_EQ(std::strcmp(str, KARROT_VERSION), 0);
  return boost::report_errors();
  }
