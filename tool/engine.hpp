/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_ENGINE_HPP
#define KARROT_ENGINE_HPP

#include <string>
#include <vector>
#include <functional>
#include <karrot/string_pool.hpp>

#include "feed_queue.hpp"
#include "package_handler.hpp"

namespace Karrot
{

class Spec;
using Requests = std::vector<Spec>;
using Database = std::vector<Implementation>;

class Engine
  {
  public:
    StringPool string_pool;
    FeedQueue feed_queue;
    PackageHandler package_handler;
    Requests requests;
    Database database;
    Dictionary globals;
    bool no_topological_order = false;
  };

} // namespace Karrot

#endif /* KARROT_ENGINE_HPP */
