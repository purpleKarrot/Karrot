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

namespace karrot
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

class Engine
  {
  public:
    Engine();
    ~Engine();
  public:
    void add_driver(const char* name, Driver* driver);
    void add_request(const char* url, bool source);
    void run();
  private:
    Engine(const Engine&);
    void operator=(const Engine&);
  private:
    class Private;
    Private* self;
  };

} // namespace karrot

#endif /* KARROT_HPP_INCLUDED */
