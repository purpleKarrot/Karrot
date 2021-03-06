/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_FEED_QUEUE_HPP
#define KARROT_FEED_QUEUE_HPP

#include "spec.hpp"
#include <vector>
#include <boost/optional.hpp>

namespace Karrot
{

class FeedQueue
  {
  public:
    FeedQueue()
      : next(0)
      {
      }
    void push(Spec const& spec)
      {
      for (const Spec& cur : urls)
        {
        if (cur.id == spec.id)
          {
          return;
          }
        }
      urls.push_back(spec);
      }
    void current_id(std::string const& id)
      {
      urls[next - 1].id = id;
      }
    boost::optional<Spec> get_next()
      {
      if (next < urls.size())
        {
        return urls[next++];
        }
      return boost::none;
      }
  private:
    std::vector<Spec> urls;
    std::size_t next;
  };

} // namespace Karrot

#endif /* KARROT_FEED_QUEUE_HPP */
