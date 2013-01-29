/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot.h>
#include "database.hpp"
#include <cstring>
#include <algorithm>

#include "solve.hpp"
#include "feed_queue.hpp"
#include "feed_cache.hpp"
#include "feed_parser.hpp"
#include "package_handler.hpp"
#include "xml_reader.hpp"

struct _KEngine
  {
  Karrot::FeedQueue feed_queue;
  Karrot::FeedCache feed_cache;
  Karrot::PackageHandler package_handler;
  Karrot::Requests requests;
  Karrot::Database database;
  };

KEngine *
k_engine_new(void)
  {
  return new KEngine();
  }

void k_engine_free(KEngine *self)
  {
  delete self;
  }

void k_engine_add_driver(KEngine *self, char const *name, KDriver *driver)
  {
  self->package_handler.add(name, driver);
  }

void k_engine_add_request(KEngine *self, char const *url_string, int source)
  {
  using namespace Karrot;
  Url url(url_string);
  self->feed_queue.push(url);
  Spec spec(url);
  if (source != 0)
    {
    spec.component = "SOURCE";
    }
  self->requests.push_back(std::move(spec));
  }

int k_engine_run(KEngine *self)
  {
  using namespace Karrot;
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
      std::cerr << "not a valid project feed" << std::endl;
      }
    }
  std::vector<int> model;
  if (!solve(self->database, self->requests, model))
    {
    return false;
    }
  for (int i : model)
    {
    const DatabaseEntry& entry = self->database[i];
    if (entry.driver)
      {
      bool requested = std::any_of(self->requests.begin(), self->requests.end(),
        [&entry](const Spec& spec)
        {
        return satisfies(entry, spec);
        });
      entry.driver->download(entry.impl, requested);
      }
    }
  return true;
  }
