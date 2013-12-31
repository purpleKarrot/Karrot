/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_DRIVER_HPP
#define KARROT_DRIVER_HPP

#include <string>
#include <functional>
#include <karrot/module.hpp>

namespace Karrot
{

class Driver
{
protected:
  Driver(std::string name, std::string xmlns) :
      name_(std::move(name)), xmlns_(std::move(xmlns))
  {
  }

  Driver(Driver const&) = delete;
  Driver& operator=(Driver const&) = delete;

public:
  virtual ~Driver()
  {
  }

  std::string const& name() const
  {
    return name_;
  }

  std::string const& xmlns() const
  {
    return xmlns_;
  }

  using Add = std::function<void(Module, bool)>;
  void filter(Module module, Add add)
  {
    do_filter(std::move(module), std::move(add));
  }

  void handle(Module const& impl)
  {
    do_handle(impl);
  }

private:
  virtual void do_filter(Module impl, Add add)
  {
  }

  virtual void do_handle(Module const& impl) = 0;

private:
  std::string name_;
  std::string xmlns_;
};

} // namespace Karrot

#endif /* KARROT_DRIVER_HPP */
