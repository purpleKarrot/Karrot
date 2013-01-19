/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot.hpp>
#include "database.hpp"
#include "quark.hpp"
#include <cstring>
#include <algorithm>

#include "solve.hpp"
#include "feed_queue.hpp"
#include "feed_cache.hpp"
#include "feed_parser.hpp"
#include "package_handler.hpp"
#include "xml_reader.hpp"

namespace karrot
{

class Engine::Private
  {
  public:
    FeedQueue feed_queue;
    FeedCache feed_cache;
    PackageHandler package_handler;
    Requests requests;
    Database database;
  };

Engine::Engine() :
    self(new Private)
  {
  }

Engine::~Engine()
  {
  delete self;
  }

void Engine::add_driver(const char* name, Driver* driver)
  {
  self->package_handler.add(string_to_quark(name, std::strlen(name)), driver);
  }

void Engine::add_request(const char* url_string, bool source)
  {
  Url url(url_string);
  self->feed_queue.push(url);
  Spec spec(url);
  if (source)
    {
    spec.component = string_to_quark("SOURCE");
    }
  self->requests.push_back(std::move(spec));
  }

void Engine::run()
  {
  const Url* purl;
  while ((purl = self->feed_queue.get_next()))
    {
    const Url url(*purl); //explicit copy!
    XmlReader xml(self->feed_cache.local_path(url));
    if (!xml.start_element())
      {
      throw std::runtime_error("failed to read start of feed");
      }
    FeedParser parser(self->feed_queue, self->database, self->package_handler);
    if (!parser.parse(url, xml))
      {
      std::cerr << "not a valid ryppl feed" << std::endl;
      }
    }
  for (int i : karrot::solve(self->database, self->requests))
    {
    const DatabaseEntry& entry = self->database[i];
    if (entry.driver)
      {
      bool requested = std::any_of(self->requests.begin(), self->requests.end(),
        [&entry](const Spec& spec)
        {
        return satisfies(entry, spec);
        });
      entry.driver->download(entry.base, requested);
      }
    }
  }

} // namespace karrot
