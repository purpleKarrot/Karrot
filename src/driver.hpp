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
#include "dictionary.hpp"

namespace Karrot
{

class Driver
  {
  public:
    Driver(std::string const& name, std::string const& xmlns, KDriver const& driver);
    ~Driver();
  public:
    Driver(Driver&& other) noexcept;
    Driver& operator=(Driver&& other) noexcept;
  public:
    Driver(Driver const&) = delete;
    Driver& operator=(Driver const&) = delete;
  public:
    std::string const& name() const
      {
      return name_;
      }
    std::string const& xmlns() const
      {
      return xmlns_;
      }
    KDictionary fields() const
      {
      return fields_;
      }
    template<typename Add>
    void filter (KDictionary const& fields, Add add) const
      {
      KAdd add_fn = [](KDictionary const *dict, int native, void *self)
        {
        auto add = *reinterpret_cast<Add*>(self);
        add(*dict, native != 0);
        };
      filter(fields, add_fn, &add);
      }
    void depend (KImplementation const& impl, KImplementation const& other) const;
    void handle (KImplementation const& impl, bool requested) const;
    void commit() const;
  private:
    void filter (KDictionary const& fields, KAdd add, void *add_target) const;
    void throw_error() const;
  private:
    std::string name_;
    std::string xmlns_;
    KDictionary fields_;
    KDriver kdriver_;
  };

} // namespace Karrot

#endif /* KARROT_DRIVER_HPP */
