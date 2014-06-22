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

KEngine *
k_engine_new()
  {
  return new KEngine;
  }

void k_engine_free(KEngine *self)
  {
  delete self;
  }

void k_engine_set_global(KEngine *self, char const *key, char const *value)
  {
  if (std::strcmp(key, ":xmlns") == 0)
    {
    self->xmlns = value;
    }
  else if (std::strcmp(key, ":feed-cache") == 0)
    {
    self->feed_cache = value ? value : ".";
    }
  else if (std::strcmp(key, ":reload-feeds") == 0)
    {
    self->reload_feeds = value != nullptr;
    }
  else if (std::strcmp(key, ":no-topological-order") == 0)
    {
    self->no_topological_order = value != nullptr;
    }
  else
    {
    Karrot::set(self->globals, key, value);
    }
  }

void
k_engine_add_driver(KEngine *self, char const *name, char const *xmlns, KDriver const *driver)
  {
  assert(name);
  assert(xmlns);
  assert(driver);
  self->package_handler.add(name, xmlns, driver);
  }

void k_engine_add_request(KEngine *self, char const *url, int source)
  {
  Karrot::Spec spec(url);
  if (source != 0)
    {
    spec.component = "SOURCE";
    }
  self->feed_queue.push(spec);
  self->requests.push_back(spec);
  }

static bool engine_run(KEngine *self)
  {
  using namespace Karrot;
  while (auto spec = self->feed_queue.get_next())
    {
    std::clog << boost::format("Reading feed '%1%'\n") % spec->id;
    std::string local_path = download(spec->id, self->feed_cache, self->reload_feeds);
    XmlReader xml(local_path);
    if (!xml.start_element())
      {
      BOOST_THROW_EXCEPTION(std::runtime_error("failed to read feed: " + local_path));
      }
    FeedParser parser{*spec, *self};
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
  std::clog << boost::format("Solving SAT with %1% variables\n") % self->database.size();
  bool solvable = solve(self->database, self->requests, model);
  if (!solvable)
    {
    self->error = "Not solvable!";
    return false;
    }
  if (!self->no_topological_order)
    {
    model = topological_sort(model, self->database);
    }
  for (int i : model)
    {
    const KImplementation& impl = self->database[i];
    std::clog << boost::format("Handling '%1% %2%'\n") % impl.name % impl.version;
    bool requested = std::any_of(self->requests.begin(), self->requests.end(),
      [&impl](const Spec& spec)
      {
      return satisfies(impl, spec);
      });
    impl.driver->handle(impl, requested);
    for (auto& spec : impl.depends)
      {
      for (int k : model)
        {
        const KImplementation& other = self->database[k];
        if (satisfies(other, spec))
          {
          impl.driver->depend(impl, other);
          }
        }
      }
    }
  return true;
  }

int k_engine_run(KEngine *self)
  {
  try
    {
    return engine_run(self) ? 0 : 1;
    }
  catch (Karrot::XmlParseError& error)
    {
    std::stringstream stream;
    stream
      << error.what()
      << " in '" << error.filename << "' at line " << error.line << ".\n"
      << error.current_line << '\n'
      << std::string(error.column, ' ') << "^\n"
      << error.message << '\n'
      ;
    self->error = stream.str();
    }
  catch (...)
    {
    self->error = boost::current_exception_diagnostic_information();
    }
  return -1;
  }

char const *k_engine_get_error(KEngine *self)
  {
  return self->error.c_str();
  }
