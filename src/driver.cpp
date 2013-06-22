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

Driver::Driver(std::string const& name, std::string const& xmlns, KDriver const& impl)
  : name_{name}
  , xmlns_{xmlns_ + name_}
  , kdriver_(impl)
  {
  if (!kdriver_.fields)
    {
    return;
    }
  if (kdriver_.fields(kdriver_.ctx, &fields_))
    {
    }
  auto found = fields_.find(":xmlns");
  if (found != fields_.end())
    {
    xmlns_ = found->second;
    }
  }

Driver::~Driver()
  {
  if (kdriver_.finalize)
    {
    kdriver_.finalize(kdriver_.ctx);
    }
  }

Driver::Driver(Driver&& other) noexcept
  : name_{std::move(other.name_)}
  , xmlns_{std::move(other.xmlns_)}
  , fields_{std::move(other.fields_)}
  , kdriver_(other.kdriver_)
  {
  other.kdriver_ = {};
  }

Driver& Driver::operator=(Driver&& other) noexcept
  {
  if (kdriver_.finalize)
    {
    kdriver_.finalize(kdriver_.ctx);
    }
  name_ = std::move(other.name_);
  xmlns_ = std::move(other.xmlns_);
  fields_ = std::move(other.fields_);
  kdriver_ = other.kdriver_;
  other.kdriver_ = {};
  return *this;
  }

void Driver::filter(KDictionary const& fields, KAdd add, void *add_target) const
  {
  if (!kdriver_.filter)
    {
    return;
    }
  if (kdriver_.filter(kdriver_.ctx, &fields, add, add_target) != 0)
    {
    throw_error();
    }
  }

void Driver::depend(KImplementation const& impl, KImplementation const& other) const
  {
  if (!kdriver_.depend)
    {
    return;
    }
  if (kdriver_.depend(kdriver_.ctx, &impl, &other) != 0)
    {
    throw_error();
    }
  }

void Driver::handle(KImplementation const& impl, bool requested) const
  {
  if (!kdriver_.handle)
    {
    return;
    }
  if (kdriver_.handle(kdriver_.ctx, &impl, requested ? 1 : 0) != 0)
    {
    throw_error();
    }
  }

void Driver::commit() const
  {
  if (!kdriver_.commit)
    {
    return;
    }
  if (kdriver_.commit(kdriver_.ctx) != 0)
    {
    throw_error();
    }
  }

void Driver::throw_error() const
  {
  if (kdriver_.get_error_message)
    {
    throw std::runtime_error(kdriver_.get_error_message(kdriver_.ctx));
    }
  throw std::runtime_error("Unknown Error in '" + name_ + "' driver.");
  }

} // namespace Karrot
