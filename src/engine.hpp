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
#include <functional>

#include "feed_queue.hpp"
#include "package_handler.hpp"

namespace Karrot
{

class Spec;
using Requests = std::vector<Spec>;
using Database = std::vector<KImplementation>;

class Engine
  {
  public:
    void set_global(char const *key, char const *value);
    void add_driver(char const *name, char const *xmlns, KDriver const *driver);
    void add_request(char const *url, int source);
    bool run();
  public:
    FeedQueue feed_queue;
    PackageHandler package_handler;
    Requests requests;
    Database database;
    Dictionary globals;
    bool no_topological_order = false;
  };

} // namespace Karrot

#endif /* KARROT_ENGINE_HPP */
