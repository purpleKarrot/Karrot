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

#define sanity_check(url1, url2, elem) do {                                    \
  if (url1.elem != url2.elem)                                                  \
    {                                                                          \
    std::printf("'%s/%s' first seen with " #elem " '%s', now with '%s'!\n",    \
      quark_to_string(url1.host),                                              \
      quark_to_string(url1.path),                                              \
      quark_to_string(url1.elem),                                              \
      quark_to_string(url2.elem));                                             \
    }                                                                          \
  } while (0)                                                                  \

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
          sanity_check(cur, url, scheme);
          sanity_check(cur, url, user_info);
          sanity_check(cur, url, port);
          return;
          }
        }
      //std::cout << url.host << url.path << std::endl;
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
