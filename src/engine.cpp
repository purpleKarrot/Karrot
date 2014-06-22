/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "engine.hpp"

#include <cstring>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <boost/throw_exception.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <boost/format.hpp>
#include "url.hpp"
#include "graph.hpp"
#include "solve.hpp"
#include "feed_queue.hpp"
#include "feed_parser.hpp"
#include "package_handler.hpp"
#include "xml_reader.hpp"

namespace Karrot
{

void Engine::set_global(char const *key, char const *value)
  {
  if (std::strcmp(key, ":feed-cache") == 0)
    {
    this->feed_cache = value ? value : ".";
    }
  else if (std::strcmp(key, ":reload-feeds") == 0)
    {
    this->reload_feeds = value != nullptr;
    }
  else if (std::strcmp(key, ":no-topological-order") == 0)
    {
    this->no_topological_order = value != nullptr;
    }
  else
    {
    Karrot::set(this->globals, key, value);
    }
  }

void Engine::add_driver(char const *name, char const *xmlns, KDriver const *driver)
  {
  assert(name);
  assert(xmlns);
  assert(driver);
  this->package_handler.add(name, xmlns, driver);
  }

void Engine::add_request(char const *url, int source)
  {
  Karrot::Spec spec(url);
  if (source != 0)
    {
    spec.component = "SOURCE";
    }
  this->feed_queue.push(spec);
  this->requests.push_back(spec);
  }

bool Engine::run()
  {
  while (auto spec = this->feed_queue.get_next())
    {
    std::clog << boost::format("Reading feed '%1%'\n") % spec->id;
    std::string local_path = download(spec->id, this->feed_cache, this->reload_feeds);
    XmlReader xml(local_path);
    if (!xml.start_element())
      {
      BOOST_THROW_EXCEPTION(std::runtime_error("failed to read feed: " + local_path));
      }
    FeedParser parser{*spec, *this};
    try
      {
      parser.parse(xml);
      }
    catch (XmlParseError& error)
      {
      error.filename = local_path;
      throw;
      }
    }
  std::vector<int> model;
  std::clog << boost::format("Solving SAT with %1% variables\n") % this->database.size();
  bool solvable = solve(this->database, this->requests, model);
  if (!solvable)
    {
    return false;
    }
  if (!this->no_topological_order)
    {
    model = topological_sort(model, this->database);
    }
  for (int i : model)
    {
    const KImplementation& impl = this->database[i];
    std::clog << boost::format("Handling '%1% %2%'\n") % impl.name % impl.version;
    bool requested = std::any_of(this->requests.begin(), this->requests.end(),
      [&impl](const Spec& spec)
      {
      return satisfies(impl, spec);
      });
    impl.driver->handle(impl, requested);
    for (auto& spec : impl.depends)
      {
      for (int k : model)
        {
        const KImplementation& other = this->database[k];
        if (satisfies(other, spec))
          {
          impl.driver->depend(impl, other);
          }
        }
      }
    }
  return true;
  }

} // namespace Karrot
