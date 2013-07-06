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

#include "feed_queue.hpp"
#include "package_handler.hpp"

namespace Karrot
{

class Spec;
using Requests = std::vector<Spec>;
using Database = std::vector<KImplementation>;

} // namespace Karrot

struct KEngine
  {
  public:
    KEngine(char const *namespace_uri)
      : namespace_uri{namespace_uri}
      {
      if (this->namespace_uri.back() != '/')
        {
        this->namespace_uri += '/';
        }
      }
  public:
    std::string error;
    std::string namespace_uri;
    Karrot::FeedQueue feed_queue;
    Karrot::PackageHandler package_handler;
    Karrot::Requests requests;
    Karrot::Database database;
    KDictionary globals;
    std::string feed_cache = ".";
    bool reload_feeds = false;
    bool ignore_source_conflicts = false;
    bool no_topological_order = false;
    KPrintFun log_function = [](char const*){};
  };

#endif /* KARROT_ENGINE_HPP */
