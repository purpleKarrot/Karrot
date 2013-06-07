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
#include <stdexcept>
#include <string>
#include <cstdio>

class Dictionary;
class Implementation;
class Driver;
class Engine;

typedef std::function<void(const char*, const char*)> Visit;
typedef std::function<void(char const **val, int size, bool native)> AddFun;

class Dictionary
  {
  public:
    std::string operator[](const char *key) const
      {
      char const *value = k_dictionary_get(self, key);
      return value ? value : std::string();
      }
    void foreach(Visit visit) const
      {
      KVisit visit_fn = [](void *target, char const *key, char const *val) -> int
        {
        reinterpret_cast<Visit*>(target)->operator()(key, val);
        return 0;
        };
      k_dictionary_foreach(self, visit_fn, &visit);
      }
  private:
    Dictionary(KDictionary const *self) : self(self)
      {
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
      char const *value = k_implementation_get_name(self);
      return value ? value : std::string();
      }
    std::string component() const
      {
      char const *value = k_implementation_get_component(self);
      return value ? value : std::string();
      }
    std::string version() const
      {
      char const *value = k_implementation_get_version(self);
      return value ? value : std::string();
      }
    Dictionary variant() const
      {
      return Dictionary(k_implementation_get_variant(self));
      }
    Dictionary values() const
      {
      return Dictionary(k_implementation_get_values(self));
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
    virtual const char* name() const = 0;
    virtual void fields(Fields& out) const
      {
      }
    virtual void filter(Dictionary const& fields, AddFun const& add)
      {
      }
    virtual void download(Implementation const& impl, bool requested) = 0;
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
    const char* name() const //override
      {
      return component->name();
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
    static void destroy_fun(void *self)
      {
      delete reinterpret_cast<Driver*>(self);
      }
  public:
    Engine(char const *namespace_uri)
      {
      self = k_engine_new(namespace_uri);
      KPrintFun log_function = [](char const *message)
        {
        std::puts(message);
        };
      k_engine_setopt(self, K_OPT_LOG_FUNCTION, log_function);
      }
    ~Engine()
      {
      k_engine_free(self);
      }
  public:
    void add_request(const char* url, bool source)
      {
      k_engine_add_request(self, url, source);
      }
    bool run()
      {
      return k_engine_run(self) == 0;
      }
  private:
    KEngine *self;
  };

#endif /* KARROT_HPP_INCLUDED */
