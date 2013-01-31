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
#include <string>

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
    std::string operator[](const char *key) const
      {
      char const *value = k_dict_lookup(self, key);
      return value ? value : std::string();
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
    std::string name() const
      {
      char const *value = k_impl_get_name(self);
      return value ? value : std::string();
      }
    std::string component() const
      {
      char const *value = k_impl_get_component(self);
      return value ? value : std::string();
      }
    std::string version() const
      {
      char const *value = k_impl_get_version(self);
      return value ? value : std::string();
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
    class Fields
      {
      private:
        Fields(char const * const *&fields, std::size_t &size)
            : fields(fields), size(size)
          {
          }
      public:
        template<std::size_t Size>
        Fields& operator=(const char* const (&fields)[Size])
          {
          this->fields = fields;
          this->size = Size;
          return *this;
          }
      private:
        char const *const *&fields;
        std::size_t &size;
        friend class Engine;
      };
  public:
    virtual ~Driver()
      {
      }
    virtual const char* namespace_uri() const
      {
      return 0;
      }
    virtual void fields(Fields& out) const
      {
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
    void fields(Fields& out) const //override
      {
      component->fields(out);
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
      KDriver kdriver =
        {
        namespace_uri,
        nullptr,
        0,
        download_fun,
        driver.get(),
        destroy_fun,
        filter_fun,
        driver.get(),
        nullptr
        };
      Driver::Fields fields(kdriver.fields, kdriver.fields_size);
      driver->fields(fields);
      k_engine_add_driver(self, name, &kdriver);
      driver.release();
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
