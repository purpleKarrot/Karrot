/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_FEED_QUEUE_HPP
#define KARROT_FEED_QUEUE_HPP

#include "url.hpp"
#include "quark.hpp"
#include <vector>
#include <cstdio>

namespace Karrot
{

class FeedQueue
  {
  public:
    FeedQueue() :
        next(0)
      {
      }
    void push(const Url& url)
      {
      for (const Url& cur : urls)
        {
        if (cur.path == url.path && cur.host == url.host)
          {
          return;
          }
        }
      urls.push_back(url);
      }
    void current(const Url& url)
      {
      urls[next] = url;
      }
    Url* get_next()
      {
      if (next < urls.size())
        {
        return &urls[next++];
        }
      return 0;
      }
  private:
    std::vector<Url> urls;
    std::size_t next;
  };

} // namespace Karrot

#endif /* KARROT_FEED_QUEUE_HPP */
