/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot/engine.hpp>
#include <karrot/deliverable.hpp>

namespace karrot
{

class Engine::Implementation
  {
  public:
    std::vector<Deliverable> deliverables;
  };

Engine::Engine() :
    pimpl(new Implementation)
  {
  }

Engine::~Engine()
  {
  delete pimpl;
  }

void Engine::add_driver(const char* name, Driver* driver)
  {
  }

void Engine::load_feed(const Url& url)
  {
  }

std::vector<int> Engine::solve(const std::vector<Spec>& projects)
  {
  return std::vector<int>();
  }

const Deliverable& Engine::operator[](int index)
  {
  return pimpl->deliverables[index];
  }

} // namespace karrot
