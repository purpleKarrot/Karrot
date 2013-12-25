/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot/engine.hpp>
#include "engine.hpp"

#include <cstring>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <boost/throw_exception.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include "log.hpp"
#include "url.hpp"
#include "graph.hpp"
#include "solve.hpp"
#include "feed_queue.hpp"
#include "feed_parser.hpp"
#include "package_handler.hpp"
#include "xml_reader.hpp"

namespace Karrot
{

struct Engine::Implementation: KEngine
  {
  };

Engine::Engine() : self(new Implementation)
  {
  }

Engine::~Engine()
  {
  delete self;
  }

void Engine::set_logger(KPrint print, void *target)
  {
  self->log_function = [print,target](std::string const& message)
    {
    print(target, 0, message.c_str());
    };
  }

void Engine::add_driver(std::unique_ptr<Karrot::Driver> driver)
  {
  self->package_handler.add(std::move(driver));
  }

void Engine::add_request(char const *url, int source)
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
    Log(self->log_function, "Reading feed '%1%'") % spec->id;
    std::string local_path = download(spec->id, self->feed_cache, self->reload_feeds);
    XmlReader xml(local_path);
    if (!xml.start_element())
      {
      BOOST_THROW_EXCEPTION(std::runtime_error("failed to read feed: " + local_path));
      }
    FeedParser parser{*spec, *self};
    try
      {
      parser.parse(xml, self->log_function);
      }
    catch (XmlParseError& error)
      {
      error.filename = local_path;
      throw;
      }
    }
  std::vector<int> model;
  Log(self->log_function, "Solving SAT with %1% variables") % self->database.size();
  bool solvable = solve(
      self->database,
      self->requests,
      self->log_function,
      model);
  if (!solvable)
    {
    return false;
    }
  if (!self->no_topological_order)
    {
    model = topological_sort(model, self->database);
    }
  for (int i : model)
    {
    Implementation const& impl = self->database[i];
    Log(self->log_function, "Handling '%1% %2%'") % impl.name % impl.version;
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
        Implementation const& other = self->database[k];
        if (satisfies(other, spec))
          {
          impl.driver->depend(impl, other);
          }
        }
      }
    }
  return true;
  }

bool Engine::run()
  {
  try
    {
    return engine_run(self);
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
    throw std::runtime_error(stream.str());
    }
  }

} // namespace Karrot
