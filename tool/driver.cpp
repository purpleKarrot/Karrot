/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "driver.hpp"
#include <karrot.h>

struct KDriver
  {
  int (*filter) (KDriver const *self, Karrot::Implementation *impl, KAdd add, void *target);
  int (*handle) (KDriver const *self, Karrot::Implementation const *impl, int requested);
  int (*commit) (KDriver const *self);
  char const* (*get_error) (KDriver const *self);
  };

namespace Karrot
{

Driver::Driver(std::string const& name, std::string const& xmlns, KDriver const* impl)
    : name_{name}
    , xmlns_{xmlns}
    , driver_(impl)
  {
  }

void Driver::filter(Implementation& impl, KAdd add, void *add_target) const
  {
  if (!driver_->filter)
    {
    return;
    }
  if (driver_->filter(driver_, &impl, add, add_target) != 0)
    {
    throw_error();
    }
  }

void Driver::handle(Implementation const& impl, bool requested) const
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
