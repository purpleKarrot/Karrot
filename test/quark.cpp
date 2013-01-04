/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot/quark.hpp>
#include <boost/detail/lightweight_test.hpp>

static std::ostream& operator<<(std::ostream& out, karrot::Quark quark)
  {
  return out << quark.str();
  }

int quark(int argc, char* argv[])
  {
  BOOST_TEST_EQ(karrot::Quark("Hello World", 5), "Hello");
  return boost::report_errors();
  }
