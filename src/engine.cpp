/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "engine.hpp"

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

struct ClientError
  {
  };

_KEngine::_KEngine(char const *namespace_uri)
  : namespace_uri{namespace_uri}
  {
  if (this->namespace_uri.back() != '/')
    {
    this->namespace_uri += '/';
    }
  }

void KEngine::set_error(int code, char const *domain, char const *message)
  {
  error_ =  Error{code};
  if (domain)
    {
    error_->domain = domain;
    }
  if (message)
    {
    error_->message = message;
    }
  }

bool KEngine::get_error(int *code, char const **domain, char const **message) const
  {
  if (!error_)
    {
    return 0;
    }
  if (code)
    {
    *code = error_->code;
    }
  if (domain)
    {
    *domain = error_->domain.c_str();
    }
  if (message)
    {
    *message = error_->message.c_str();
    }
  return 1;
  }

void KEngine::set_handle_fn(KHandle fn, void *target, void (*destroy)(void*))
  {
  handle_fn = Karrot::Handle{fn, target, destroy};
  }

void KEngine::set_depend_fn(KDepend fn, void *target, void (*destroy)(void*))
  {
  depend_fn = Karrot::Depend{fn, target, destroy};
  }

void KEngine::handle(KImplementation const& impl, bool requested) const
  {
  if (!handle_fn)
    {
    return;
    }
  if (!handle_fn(impl, requested))
    {
    throw ClientError{};
    }
  }

void KEngine::depend(KImplementation const& impl, KImplementation const& other) const
  {
  if (!depend_fn)
    {
    return;
    }
  if (!depend_fn(impl, other))
    {
    throw ClientError{};
    }
  }

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
  if (source != 0)
    {
    spec.component = "SOURCE";
    }
  self->feed_queue.push(spec);
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
    case K_OPT_NO_TOPOLOGICAL_ORDER:
      self->no_topological_order = va_arg(arg, int);
      break;
    default:
      break;
    }
  va_end(arg);
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
      self->ignore_source_conflicts,
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
    const KImplementation& impl = self->database[i];
    bool requested = std::any_of(self->requests.begin(), self->requests.end(),
      [&impl](const Spec& spec)
      {
      return satisfies(impl, spec);
      });
    if (impl.driver)
      {
      Log(self->log_function, "Handling '%1% %2%'") % impl.name % impl.version;
      impl.driver->download(impl, requested);
      }
    self->handle(impl, requested);
    for (int k : model)
      {
      const KImplementation& other = self->database[k];
      for (auto& spec : other.depends)
        {
        if (satisfies(impl, spec))
          {
          self->depend(impl, other);
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
  catch (ClientError)
    {
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

void k_engine_set_error(KEngine *self, int code, char const *domain, char const *message)
  {
  self->set_error(code, domain, message);
  }

int k_engine_get_error(KEngine const *self, int *code, char const **domain, char const **message)
  {
  return self->get_error(code, domain, message) ? 1 : 0;
  }

void k_engine_set_handle_fn(KEngine *self, KHandle fn, void *target, void (*destroy)(void*))
  {
  self->set_handle_fn(fn, target, destroy);
  }

void k_engine_set_depend_fn(KEngine *self, KDepend fn, void *target, void (*destroy)(void*))
  {
  self->set_depend_fn(fn, target, destroy);
  }
