/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "driver.hpp"
#include "implementation.hpp"

namespace Karrot
{

Driver::Driver(std::string const& name, std::string const& xmlns, KDriver const* impl)
  : name_{name}, xmlns_{xmlns + name}, driver_(impl)
  {
  if (!driver_->fields)
    {
    return;
    }
  if (driver_->fields(driver_, &fields_))
    {
    }
  auto found = fields_.find(":xmlns");
  if (found != fields_.end())
    {
    xmlns_ = found->second;
    }
  }

void Driver::filter(KImplementation& impl, KAdd add, void *add_target) const
  {
  if (!driver_->filter)
    {
    add(&impl, 0, add_target);
    return;
    }
  if (driver_->filter(driver_, &impl, add, add_target) != 0)
    {
    throw_error();
    }
  }

void Driver::depend(KImplementation const& impl, KImplementation const& other) const
  {
  if (!driver_->depend)
    {
    return;
    }
  if (driver_->depend(driver_, &impl, &other) != 0)
    {
    throw_error();
    }
  }

void Driver::handle(KImplementation const& impl, bool requested) const
  {
  if (!driver_->handle)
    {
    return;
    }
  if (driver_->handle(driver_, &impl, requested ? 1 : 0) != 0)
    {
    throw_error();
    }
  }

void Driver::commit() const
  {
  if (!driver_->commit)
    {
    return;
    }
  if (driver_->commit(driver_) != 0)
    {
    throw_error();
    }
  }

void Driver::throw_error() const
  {
  if (driver_->get_error)
    {
    throw std::runtime_error(driver_->get_error(driver_));
    }
  throw std::runtime_error("Unknown Error in '" + name_ + "' driver.");
  }

} // namespace Karrot
