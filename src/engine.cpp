/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot.hpp>
#include "implementation2.hpp"
#include "quark.hpp"
#include <cstring>

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
    std::vector<Implementation2> implementations;
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

void Engine::add_request(const char* uri)
  {
  self->feed_queue.push(Url(uri));
  }

void Engine::run()
  {
  const Url* purl;
  std::set<Spec> requests;
  while ((purl = self->feed_queue.get_next()))
    {
    const Url url(*purl); //explicit copy!
    XmlReader xml(self->feed_cache.local_path(url));
    if (!xml.start_element())
      {
      throw std::runtime_error("failed to read start of feed");
      }
    FeedParser parser(self->feed_queue, self->implementations, self->package_handler);
    if (!parser.parse(url, xml))
      {
      std::cerr << "not a valid ryppl feed" << std::endl;
      }
    Spec spec(url);
    spec.component = string_to_quark("SOURCE");
    requests.insert(spec);
    }
  for (int i : karrot::solve(self->implementations, requests))
    {
    const Implementation2& impl = self->implementations[i];
    if (impl.driver)
      {
      impl.driver->download(impl.base);
      }
    }
  }

} // namespace karrot
