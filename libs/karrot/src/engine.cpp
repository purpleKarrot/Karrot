/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot/engine.hpp>

#include <cstring>
#include <fstream>
#include <algorithm>
#include <stdexcept>

#include "url.hpp"
#include "solve.hpp"
#include "types.hpp"
#include "feed_queue.hpp"
#include "feed_parser.hpp"
#include "xml_reader.hpp"

namespace Karrot
{

struct Engine::Private
  {
  FeedQueue feed_queue;
  std::vector<Driver> drivers;
  Requests requests;
  Database database;
  Solution solution;
  };

Engine::Engine() : self(new Private)
  {
  }

Engine::~Engine()
  {
  delete self;
  }

void Engine::add_filter(std::string name, std::string xmlns, Filter filter)
  {
  using std::move;
  self->drivers.push_back({move(name), move(xmlns), move(filter)});
  }

void Engine::add_request(std::string const& url, bool source)
  {
  Karrot::Spec spec(url.c_str());
  if (source)
    {
    spec.component = "SOURCE";
    }
  self->feed_queue.push(spec);
  self->requests.push_back(spec);
  }

void Engine::load(std::string const& cache, bool force)
  {
  while (auto spec = self->feed_queue.get_next())
    {
    std::clog << "Reading feed '" << spec->id << "'\n";
    auto const local_path = download(spec->id, cache, force);
    XmlReader xml(local_path);
    if (!xml.start_element())
      {
      throw std::runtime_error("failed to read feed: " + local_path);
      }
    FeedParser parser{*spec, self->drivers, self->database, self->feed_queue};
    try
      {
      parser.parse(xml);
      }
    catch (XmlParseError& error)
      {
      std::stringstream stream;
      stream
        << error.what()
        << " in '" << local_path << "' at line " << error.line << ".\n"
        << error.current_line << '\n'
        << std::string(error.column, ' ') << "^\n"
        << error.message << '\n'
        ;
      throw std::runtime_error(stream.str());
      }
    }
  }

bool Engine::solve()
  {
  self->solution.clear();
  std::clog << "Solving SAT with " << self->database.size() << " variables.\n";
  return Karrot::solve(self->database, self->requests, self->solution);
  }

std::size_t Engine::num_modules() const
  {
  return self->solution.size();
  }

Module const& Engine::get_module(std::size_t index) const
  {
  return self->database[self->solution[index]];
  }

std::vector<int> Engine::get_depends(std::size_t index) const
  {
  std::set<int> result;
  for (auto& spec : self->database[self->solution[index]].depends)
    {
    for (std::size_t i = 0; i < num_modules(); ++i)
      {
      if (satisfies(get_module(i), spec))
        {
        result.insert(i);
        }
      }
    }
  return std::vector<int>(result.begin(), result.end());
  }

bool Engine::is_requested(std::size_t index) const
  {
  auto const& module = get_module(index);
  return std::any_of(self->requests.begin(), self->requests.end(),
    [&module](const Spec& spec)
    {
    return satisfies(module, spec);
    });
  }

} // namespace Karrot
