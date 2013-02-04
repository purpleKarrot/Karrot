/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot.h>
#include "delegate.hpp"
#include "dictionary.hpp"
#include <functional>

namespace Karrot
{

typedef std::function<void(DictView const&, bool)> AddFun;

class Driver
  {
  public:
    Driver(KDriver *driver)
      : name_(driver->name ? driver->name : "")
      , ns_uri_(driver->namespace_uri ? driver->namespace_uri : "")
      , fields_(make_dict(driver->fields, driver->fields_size))
      , download_(driver->download, driver->download_target, driver->download_target_destroy_notify)
      , filter_(driver->filter, driver->filter_target, driver->filter_target_destroy_notify)
      {
      }
    std::string name() const
      {
      return name_;
      }
    std::string namespace_uri() const
      {
      return ns_uri_;
      }
    KDictionary fields() const
      {
      return fields_;
      }
    void download(const KImplementation& impl, bool requested) const
      {
      download_(&impl, requested);
      }
    void filter(const KDictionary& fields, AddFun add) const
      {
      filter_(&fields, add_fun, &add);
      }
  private:
    static KDictionary make_dict(char const * const *fields, std::size_t size)
      {
      KDictionary result;
      assert(size % 2 == 0);
      for (std::size_t i = 0; i < size;)
        {
        const char *key = fields[i++];
        const char *val = fields[i++];
        assert(key);
        result[key] = val ? val : "";
        }
      return result;
      }
    static void add_fun(char const **val, int size, int native, void *self)
      {
      AddFun const &function = *reinterpret_cast<AddFun*>(self);
      function(DictView(val, size), native != 0);
      }
    std::string name_;
    std::string ns_uri_;
    KDictionary fields_;
    Delegate<void, KImplementation const*, int> download_;
    Delegate<void, KDictionary const*, KAddFun, void*> filter_;
  };

} // namespace Karrot
