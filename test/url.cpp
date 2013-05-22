/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "../src/url.hpp"
#include "../src/quark.hpp"
#include <boost/detail/lightweight_test.hpp>

int url(int argc, char* argv[])
  {
  using Karrot::resolve_uri;
  std::string base("scheme://host/path/file?query#fragment");

  BOOST_TEST_EQ(resolve_uri(base, "http://host/path"), "http://host/path");
  BOOST_TEST_EQ(resolve_uri(base, "//host2/path"), "scheme://host2/path");
  BOOST_TEST_EQ(resolve_uri(base, "/path2/file2"), "scheme://host/path2/file2");
  BOOST_TEST_EQ(resolve_uri(base, "?query2"), "scheme://host/path/file?query2");
  BOOST_TEST_EQ(resolve_uri(base, "#fragment2"), "scheme://host/path/file?query#fragment2");
  BOOST_TEST_EQ(resolve_uri(base, "file2"), "scheme://host/path/file2");

  return boost::report_errors();
  }
