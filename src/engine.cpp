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
#include <cstdarg>
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

struct _KEngine
  {
  _KEngine(char const *namespace_uri)
    : namespace_uri(namespace_uri)
    , feed_cache(".")
    , reload_feeds(false)
    , ignore_source_conflicts(false)
    , log_function{[](char const*){}}
    {
    if (this->namespace_uri.back() != '/')
      {
      this->namespace_uri += '/';
      }
    }
  std::string error;
  std::string namespace_uri;
  Karrot::FeedQueue feed_queue;
  Karrot::PackageHandler package_handler;
  Karrot::Requests requests;
  Karrot::Database database;
  std::string dot_filename;
  std::string feed_cache;
  bool reload_feeds;
  bool ignore_source_conflicts;
  KPrintFun log_function;
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

void k_engine_add_request(KEngine *self, char const *url, int source)
  {
  Karrot::Spec spec(url);
  self->feed_queue.push(spec);
  if (source != 0)
    {
    spec.component = "SOURCE";
    }
  self->requests.push_back(spec);
  }

void k_engine_setopt(KEngine *self, KOption option, ...)
  {
  va_list arg;
  va_start(arg, option);
  const char* str;
  switch (option)
    {
    case K_OPT_LOG_FUNCTION:
      self->log_function = va_arg(arg, KPrintFun);
      break;
    case K_OPT_DOT_FILENAME:
      str = va_arg(arg, const char*);
      self->dot_filename = str ? str : "";
      break;
    case K_OPT_FEED_CACHE:
      str = va_arg(arg, const char*);
      self->feed_cache = str ? str : ".";
      break;
    case K_OPT_RELOAD_FEEDS:
      self->reload_feeds = va_arg(arg, int);
      break;
    case K_OPT_IGNORE_SOURCE_CONFLICTS:
      self->ignore_source_conflicts = va_arg(arg, int);
      break;
    }
  va_end(arg);
  }

static bool engine_run(KEngine *self)
  {
  using namespace Karrot;
  while (auto spec = self->feed_queue.get_next())
    {
    Log(self->log_function, "Reading feed '%1%'") % *spec;
    std::string local_path = download(spec->id, self->feed_cache, self->reload_feeds);
    XmlReader xml(local_path);
    if (!xml.start_element())
      {
      BOOST_THROW_EXCEPTION(std::runtime_error("failed to read feed: " + local_path));
      }
    FeedParser parser(
        *spec,
        self->feed_queue,
        self->database,
        self->package_handler,
        self->namespace_uri + "project");
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
      self->ignore_source_conflicts,
      self->log_function,
      model);
  if (!solvable)
    {
    return false;
    }
  if (true /*topological sort enabled*/)
    {
    model = topological_sort(model, self->database);
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

char const *k_engine_error_message(KEngine *self)
  {
  return self->error.c_str();
  }
