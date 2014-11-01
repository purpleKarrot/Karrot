/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_FEED_QUEUE_HPP
#define KARROT_FEED_QUEUE_HPP

#include <string>
#include <vector>
#include <boost/optional.hpp>

namespace Karrot
{

class FeedQueue
  {
  public:
    void push(int id)
      {
      for (int u : urls)
        {
        if (u == id)
          {
          return;
          }
        }
      urls.push_back(id);
      }
    void current_id(int id)
      {
      urls[next - 1] = id;
      }
    boost::optional<int> get_next()
      {
      if (next < urls.size())
        {
        return urls[next++];
        }
      return boost::none;
      }
  private:
    std::vector<int> urls;
    std::size_t next = 0;
  };

} // namespace Karrot

#endif /* KARROT_FEED_QUEUE_HPP */