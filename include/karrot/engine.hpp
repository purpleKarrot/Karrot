/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_ENGINE_HPP
#define KARROT_ENGINE_HPP

#include <vector>

namespace karrot
{

class Url;
class Spec;
class Driver;
class Deliverable;

class Engine
  {
  public:
    Engine();
    ~Engine();
  public:
    void add_driver(const char* name, Driver* driver);
    void load_feed(const Url& url);
    std::vector<int> solve(const std::vector<Spec>& projects);
    const Deliverable& operator[](int index);
  private:
    Engine(const Engine&);
    void operator=(const Engine&);
  private:
    class Implementation;
    Implementation* pimpl;
  };

} // namespace karrot

#endif /* KARROT_ENGINE_HPP */
