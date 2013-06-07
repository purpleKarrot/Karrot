/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_ENGINE_HPP
#define KARROT_ENGINE_HPP

#include <karrot.h>

#include <string>
#include <vector>
#include <boost/optional.hpp>

#include "closures.hpp"
#include "feed_queue.hpp"

namespace Karrot
{

class Spec;
using Requests = std::vector<Spec>;
using Database = std::vector<KImplementation>;

} // namespace Karrot

struct _KEngine
  {
  private:
    struct Error
      {
      int code;
      std::string domain;
      std::string message;
      };
  public:
    _KEngine(char const *namespace_uri);
  public:
    void set_error_current_exception();
    void set_error(int code, char const *domain, char const *message);
    bool get_error(int *code, char const **domain, char const **message) const;
  public:
    void set_fields_fn(KFields fn, void *target, void (*destroy)(void*));
    void set_filter_fn(KFilter fn, void *target, void (*destroy)(void*));
    void set_handle_fn(KHandle fn, void *target, void (*destroy)(void*));
    void set_depend_fn(KDepend fn, void *target, void (*destroy)(void*));
  public:
    template<typename Add>
    void filter(std::string const& driver, KDictionary const& fields, Add add) const
      {
      filter_fn(driver, fields, add);
      }
    bool fields(std::string const& driver, KDictionary& fields) const;
    void handle(KImplementation const& impl, bool requested) const;
    void depend(KImplementation const& impl, KImplementation const& other) const;
  public:
    std::string namespace_uri;
    Karrot::FeedQueue feed_queue;
    Karrot::Requests requests;
    Karrot::Database database;
    std::string feed_cache = ".";
    bool reload_feeds = false;
    bool ignore_source_conflicts = false;
    bool no_topological_order = false;
    KPrintFun log_function = [](char const*){};
  private:
    boost::optional<Error> error;
    Karrot::Fields fields_fn;
    Karrot::Filter filter_fn;
    Karrot::Handle handle_fn;
    Karrot::Depend depend_fn;
  };

#endif /* KARROT_ENGINE_HPP */
