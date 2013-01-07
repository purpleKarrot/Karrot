/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot/engine.hpp>
#include <karrot/deliverable.hpp>
#include <karrot/quark.hpp>
#include <cstring>

#include "solve.hpp"
#include "feed_queue.hpp"
#include "feed_cache.hpp"
#include "feed_parser.hpp"
#include "package_handler.hpp"
#include "xml_reader.hpp"

namespace karrot
{

class Engine::Implementation
  {
  public:
    FeedQueue feed_queue;
    FeedCache feed_cache;
    PackageHandler package_handler;
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
  pimpl->package_handler.add(string_to_quark(name, std::strlen(name)), driver);
  }

void Engine::load_feed(const Url& url)
  {
  pimpl->feed_queue.push(url);
  const Url* purl;
  while ((purl = pimpl->feed_queue.get_next()))
    {
    const Url url(*purl); //explicit copy!
    XmlReader xml(pimpl->feed_cache.local_path(url));
    if (!xml.start_element())
      {
      throw std::runtime_error("failed to read start of feed");
      }
    FeedParser parser(pimpl->feed_queue, pimpl->deliverables, pimpl->package_handler);
    if (!parser.parse(url, xml))
      {
      std::cerr << "not a valid ryppl feed" << std::endl;
      }
    }
  }

std::vector<int> Engine::solve(const std::set<Spec>& projects)
  {
  return karrot::solve(pimpl->deliverables, projects);
  }

const Deliverable& Engine::operator[](int index)
  {
  return pimpl->deliverables[index];
  }

void Engine::finalize()
  {
  pimpl->package_handler.finalize();
  }

} // namespace karrot
