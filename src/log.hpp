/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_LOG_HPP
#define KARROT_LOG_HPP

#include <functional>
#include <boost/format.hpp>

namespace Karrot
{

class Log
  {
  public:
    using Function = std::function<void(std::string const&)>;
  public:
    Log(Function& fun, const char* format)
        : fun(fun), format(format)
      {
      }
    ~Log()
      {
      fun(format.str());
      }
    template<typename Arg>
    Log& operator%(Arg&& arg)
      {
      format % std::forward<Arg>(arg);
      return *this;
      }
  private:
    Function& fun;
    boost::format format;
  };

} // namespace Karrot

#endif /* KARROT_LOG_HPP */
