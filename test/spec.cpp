/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot/spec.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/detail/lightweight_test.hpp>

template<typename T>
static std::string to_str(T const& arg)
  {
  return boost::lexical_cast<std::string>(arg);
  }

int spec(int argc, char* argv[])
  {
  using Karrot::Spec;
  BOOST_TEST_EQ(to_str(Spec{}), "");
  BOOST_TEST_EQ(to_str(Spec{"id?xx==foo#c"}), "id?xx==foo#c");
  BOOST_TEST_EQ(to_str(Spec{"id", "c", "xx==foo"}), "id?xx==foo#c");
  return boost::report_errors();
  }
