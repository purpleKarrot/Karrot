/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_DRIVER_HPP
#define KARROT_DRIVER_HPP

#include <karrot.h>

namespace Karrot { struct Implementation; }
struct KDriver;
typedef void (*KAdd) (Karrot::Implementation *impl, int native, void *self);

namespace Karrot
{

class Driver
  {
  public:
    Driver(std::string const& name, std::string const& xmlns, KDriver const* driver);
  public:
    std::string const& name() const
      {
      return name_;
      }
    std::string const& xmlns() const
      {
      return xmlns_;
      }
    template<typename Add>
    void filter (Implementation& impl, Add add) const
      {
      KAdd add_fn = [](Implementation *impl, int native, void *self)
        {
        auto add = *reinterpret_cast<Add*>(self);
        add(*impl, native != 0);
        };
      filter(impl, add_fn, &add);
      }
    void handle (Implementation const& impl, bool requested) const;
    void commit() const;
  private:
    void filter (Implementation& impl, KAdd add, void *add_target) const;
    void throw_error() const;
  private:
    std::string name_;
    std::string xmlns_;
    KDriver const* driver_;
  };

} // namespace Karrot

#endif /* KARROT_DRIVER_HPP */
