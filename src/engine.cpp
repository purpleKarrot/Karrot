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
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <boost/throw_exception.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include "solve.hpp"
#include "feed_queue.hpp"
#include "feed_parser.hpp"
#include "package_handler.hpp"
#include "xml_reader.hpp"

struct _KEngine
  {
  _KEngine(char const *namespace_uri)
      : namespace_uri(namespace_uri)
    {
    if (this->namespace_uri.back() != '/')
      {
      this->namespace_uri += '/';
      }
    }
  std::string namespace_uri;
  Karrot::FeedQueue feed_queue;
  Karrot::PackageHandler package_handler;
  Karrot::Requests requests;
  Karrot::Database database;
  std::string dot_filename;
  std::string error;
  };

KEngine *
k_engine_new(char const *namespace_base)
  {
  assert(namespace_base);
  return new KEngine(namespace_base);
  }

void k_engine_free(KEngine *self)
  {
  delete self;
  }

void k_engine_add_driver(KEngine *self, KDriver *driver)
  {
  assert(driver);
  assert(driver->name);
  self->package_handler.add(driver, self->namespace_uri);
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

void k_engine_dot_filename(KEngine *self, char const *filename)
  {
  assert(filename);
  self->dot_filename = filename;
  }

static void write_graphviz(std::string const& filename,
    std::vector<int> const& model, Karrot::Database const& database)
  {
  std::ofstream dot_file(filename);
  dot_file << "digraph G {\n";
  for (std::size_t i = 0; i < model.size(); ++i)
    {
    auto& entry = database[model[i]];
    dot_file << "  " << i << " ["
      << "label=\"" << entry.name << ' ' << entry.version << "\", "
      << "URL=\"" << entry.id << "\""
      << "];" << std::endl;
      ;
    for (std::size_t k = 0; k < model.size(); ++k)
      {
      for (auto& spec : database[model[k]].depends)
        {
        if (satisfies(entry, spec))
          {
          dot_file << "  " << k << " -> " << i << ";\n";
          }
        }
      }
    }
  dot_file << "}\n";
  }

static bool engine_run(KEngine *self)
  {
  using namespace Karrot;
  const Url* purl;
  while ((purl = self->feed_queue.get_next()))
    {
    const Url url(*purl); //explicit copy!
    std::string local_path = download(url);
    XmlReader xml(local_path);
    if (!xml.start_element())
      {
      BOOST_THROW_EXCEPTION(std::runtime_error("failed to read feed: " + local_path));
      }
    FeedParser parser(
        self->feed_queue,
        self->database,
        self->package_handler,
        self->namespace_uri + "project");
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
  if (!self->dot_filename.empty())
    {
    write_graphviz(self->dot_filename, model, self->database);
    }
  for (int i : model)
    {
    const KImplementation& impl = self->database[i];
    if (impl.driver)
      {
      bool requested = std::any_of(self->requests.begin(), self->requests.end(),
        [&impl](const Spec& spec)
        {
        return satisfies(impl, spec);
        });
      impl.driver->download(impl, requested);
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
  catch (...)
    {
    self->error = boost::current_exception_diagnostic_information();
    }
  return -1;
  }

char const *k_engine_error_message(KEngine *self)
  {
  return self->error.c_str();
  }
