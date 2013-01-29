/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_HPP_INCLUDED
#define KARROT_HPP_INCLUDED

#include <karrot.h>
#include <functional>
#include <memory>
#include <vector>

namespace Karrot
{

class Dictionary;
class Implementation;
class Driver;
class Engine;

typedef std::function<void(const char*, const char*)> Mapping;
typedef std::function<void(char const **val, int size, bool native)> AddFun;

class Dictionary
  {
  public:
    const char *operator[](const char *key) const
      {
      return k_dict_lookup (self, key);
      }
    void foreach(Mapping mapping) const
      {
      k_dict_foreach(self, &mapping_fun, &mapping);
      }
  private:
    Dictionary(KDictionary const *self) : self(self)
      {
      }
    static void mapping_fun(char const *key, char const *val, void *self)
      {
      reinterpret_cast<Mapping*>(self)->operator()(key, val);
      }
    KDictionary const *self;
    friend class Implementation;
    friend class Engine;
  };

class Implementation
  {
  public:
    const char *name() const
      {
      return k_impl_get_name(self);
      }
    const char *component() const
      {
      return k_impl_get_component(self);
      }
    const char *version() const
      {
      return k_impl_get_version(self);
      }
    Dictionary variant() const
      {
      return Dictionary(k_impl_get_variant(self));
      }
    Dictionary values() const
      {
      return Dictionary(k_impl_get_values(self));
      }
  private:
    Implementation(KImplementation const *self) : self(self)
      {
      }
    KImplementation const *self;
    friend class Engine;
  };

class Driver
  {
  public:
    typedef std::vector<const char*> Fields;
  public:
    virtual ~Driver()
      {
      }
    virtual const char* namespace_uri() const
      {
      return 0;
      }
    virtual Fields fields() const
      {
      return std::vector<const char*>();
      }
    virtual void filter(Dictionary const& fields, AddFun const& add)
      {
      }
    virtual void download(Implementation const& impl, bool requested)
      {
      }
  };

class DriverDecorator: public Driver
  {
  protected:
    DriverDecorator(std::unique_ptr<Driver>&& component)
        : component(std::move(component))
      {
      }
    virtual ~DriverDecorator()
      {
      }
  protected:
    const char* namespace_uri() const //override
      {
      return component->namespace_uri();
      }
    Fields fields() const //override
      {
      return component->fields();
      }
    void filter(Dictionary const& fields, AddFun const& add) //override
      {
      component->filter(fields, add);
      }
    void download(const Implementation& impl, bool requested) //override
      {
      component->download(impl, requested);
      }
  private:
    std::unique_ptr<Driver> component;
  };

class Engine
  {
  private:
    static void download_fun(KImplementation const *impl, int requested, void *self)
      {
      Driver* driver = reinterpret_cast<Driver*>(self);
      driver->download(Implementation(impl), requested != 0);
      }
    static void filter_fun(KDictionary const *fields, KAddFun fun, void *target, void *self)
      {
      using namespace std::placeholders;
      Driver* driver = reinterpret_cast<Driver*>(self);
      driver->filter(Dictionary(fields), std::bind(fun, _1, _2, _3, target));
      }
    static void destroy_fun(void *self)
      {
      delete reinterpret_cast<Driver*>(self);
      }
  public:
    Engine()
      {
      self = k_engine_new();
      }
    ~Engine()
      {
      k_engine_free(self);
      }
  public:
    void add_driver(char const *name, std::unique_ptr<Driver>&& driver)
      {
      const char *namespace_uri = driver->namespace_uri();
      const std::vector<const char*> fields = driver->fields();
      Driver *pdriver = driver.release();
      KDriver kdriver =
        {
        namespace_uri,
        &fields[0],
        fields.size(),
        download_fun,
        pdriver,
        destroy_fun,
        filter_fun,
        pdriver,
        nullptr
        };
      k_engine_add_driver(self, name, &kdriver);
      }
    void add_request(const char* url, bool source)
      {
      k_engine_add_request(self, url, source);
      }
    bool run()
      {
      return k_engine_run(self) != 0;
      }
  private:
    KEngine *self;
  };

} // namespace Karrot

#endif /* KARROT_HPP_INCLUDED */
