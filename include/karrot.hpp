/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_HPP_INCLUDED
#define KARROT_HPP_INCLUDED

#include <map>
#include <string>
#include <memory>

#if defined(_WIN32)
#  define KARROT_IMPORT __declspec(dllimport)
#  define KARROT_EXPORT __declspec(dllexport)
#else
#  define KARROT_IMPORT __attribute__ ((visibility("default")))
#  define KARROT_EXPORT __attribute__ ((visibility("default")))
#endif

#if defined(KARROT_STATIC)
#  define KARROT_API
#elif defined(KARROT_BUILD)
#  define KARROT_API KARROT_EXPORT
#else
#  define KARROT_API KARROT_IMPORT
#endif

namespace Karrot
{

typedef std::map<std::string, std::string> Dictionary;

class Implementation
  {
  public:
    std::string name;
    std::string component;
    std::string version;
    Dictionary variant;
    Dictionary values;
  };

class KARROT_API Driver
  {
  public:
    virtual ~Driver();
  public:
    virtual const char* namespace_uri() const
      {
      return nullptr;
      }
    virtual Dictionary fields() const
      {
      return Dictionary();
      }
    virtual int filter(const Dictionary& fields, Implementation& implementation)
      {
      return 0;
      }
    virtual void download(const Implementation& implementation, bool requested)
      {
      }
  public:
    static const int INCOMPATIBLE  = 0;
    static const int NORMAL        = 1;
    static const int SYS_INSTALLED = 2;
    static const int SYS_AVAILABLE = 3;
  };

#if defined _MSC_VER
template class __declspec(dllexport) std::unique_ptr<Driver>;
#endif

class KARROT_API DriverDecorator: public Driver
  {
  protected:
    DriverDecorator(std::unique_ptr<Driver>&& component)
        : component(std::move(component))
      {
      }
    virtual ~DriverDecorator();
  protected:
    const char* namespace_uri() const //override
      {
      return component->namespace_uri();
      }
    Dictionary fields() const //override
      {
      return component->fields();
      }
    int filter(const Dictionary& fields, Implementation& impl) //override
      {
      return component->filter(fields, impl);
      }
    void download(const Implementation& impl, bool requested) //override
      {
      component->download(impl, requested);
      }
  private:
    std::unique_ptr<Driver> component;
  };

class KARROT_API Engine
  {
  public:
    Engine();
    ~Engine();
  public:
    void add_driver(std::string&& name, std::unique_ptr<Driver>&& driver);
    void add_request(const char* url, bool source);
    bool run();
  private:
    class Private;
    Private* self;
  };

} // namespace Karrot

#endif /* KARROT_HPP_INCLUDED */
