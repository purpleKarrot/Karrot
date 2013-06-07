/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_STRING_HPP
#define KARROT_STRING_HPP

#include <string>
#include <boost/flyweight/flyweight.hpp>
#include <boost/flyweight/hashed_factory.hpp>
#include <boost/flyweight/static_holder.hpp>
#include <boost/flyweight/no_locking.hpp>
#include <boost/flyweight/no_tracking.hpp>

namespace Karrot
{

using String = boost::flyweight
  <
  std::string,
  boost::flyweights::no_locking,
  boost::flyweights::no_tracking
  >;

} // namespace Karrot

#endif /* KARROT_STRING_HPP */
