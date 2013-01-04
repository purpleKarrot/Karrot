/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot/url.hpp>
#include <boost/detail/lightweight_test.hpp>

static std::ostream& operator<<(std::ostream& out, karrot::Quark quark)
  {
  return out << quark.str();
  }

int url(int argc, char* argv[])
  {
  karrot::Url url1("scheme://host/path?query#fragment");
  BOOST_TEST_EQ(url1.scheme, "scheme");
  BOOST_TEST_EQ(url1.host, "host");
  BOOST_TEST_EQ(url1.path, "/path");
  BOOST_TEST_EQ(url1.query, "query");
  BOOST_TEST_EQ(url1.fragment, "fragment");

  karrot::Url url2("?query2#fragment2", &url1);
  BOOST_TEST_EQ(url2.scheme, "scheme");
  BOOST_TEST_EQ(url2.host, "host");
  BOOST_TEST_EQ(url2.path, "/path");
  BOOST_TEST_EQ(url2.query, "query2");
  BOOST_TEST_EQ(url2.fragment, "fragment2");

  return boost::report_errors();
  }
