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

class Driver
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

class DriverDecorator: public Driver
  {
  public:
    DriverDecorator(std::unique_ptr<Driver>&& component)
        : component(std::move(component))
      {
      }
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

class Engine
  {
  public:
    Engine();
    ~Engine();
  public:
    void add_driver(std::string&& name, std::unique_ptr<Driver>&& driver);
    void add_request(const char* url, bool source);
    void run();
  private:
    Engine(const Engine&);
    void operator=(const Engine&);
  private:
    class Private;
    Private* self;
  };

} // namespace Karrot

#endif /* KARROT_HPP_INCLUDED */