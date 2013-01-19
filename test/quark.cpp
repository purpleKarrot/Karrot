/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "quark.hpp"
#include <boost/detail/lightweight_test.hpp>

int quark(int argc, char* argv[])
  {
  using Karrot::string_to_quark;
  BOOST_TEST_EQ(string_to_quark("Hello World", 5), string_to_quark("Hello"));
  return boost::report_errors();
  }
