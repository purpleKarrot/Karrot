/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot/url.hpp>
#include <karrot/quark.hpp>
#include <boost/detail/lightweight_test.hpp>

int url(int argc, char* argv[])
  {
  karrot::Url url1("scheme://host/path?query#fragment");
  BOOST_TEST_EQ(url1.scheme, karrot::string_to_quark("scheme"));
  BOOST_TEST_EQ(url1.host, karrot::string_to_quark("host"));
  BOOST_TEST_EQ(url1.path, karrot::string_to_quark("/path"));
  BOOST_TEST_EQ(url1.query, karrot::string_to_quark("query"));
  BOOST_TEST_EQ(url1.fragment, karrot::string_to_quark("fragment"));

  karrot::Url url2("?query2#fragment2", &url1);
  BOOST_TEST_EQ(url2.scheme, karrot::string_to_quark("scheme"));
  BOOST_TEST_EQ(url2.host, karrot::string_to_quark("host"));
  BOOST_TEST_EQ(url2.path, karrot::string_to_quark("/path"));
  BOOST_TEST_EQ(url2.query, karrot::string_to_quark("query2"));
  BOOST_TEST_EQ(url2.fragment, karrot::string_to_quark("fragment2"));

  return boost::report_errors();
  }
