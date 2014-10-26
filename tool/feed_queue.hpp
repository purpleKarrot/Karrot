/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_FEED_QUEUE_HPP
#define KARROT_FEED_QUEUE_HPP

#include <karrot/spec.hpp>
#include <vector>
#include <boost/optional.hpp>

namespace Karrot
{

class FeedQueue
  {
  public:
    void push(Spec const& spec)
      {
      for (Spec& cur : urls)
        {
        if (cur.id == spec.id)
          {
          if (cur.component.get().empty())
            {
            cur.component = spec.component;
            }
          if (cur.query.empty())
            {
            cur.query = spec.query;
            }
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
    std::size_t next = 0;
  };

class FeedPreQueue
  {
  public:
    FeedPreQueue(FeedQueue& feed_queue)
      : feed_queue(feed_queue)
      {
      }
    ~FeedPreQueue()
      {
      for (auto& url: urls)
        {
        feed_queue.push(url);
        }
      }
    void push(Spec const& spec)
      {
      for (Spec& cur : urls)
        {
        if (cur.id == spec.id)
          {
          if (cur.component != spec.component)
            {
            cur.component = String{};
            }
          if (cur.query != spec.query)
            {
            cur.query = Query{};
            }
          return;
          }
        }
      urls.push_back(spec);
      }
    void current_id(std::string const& id)
      {
      feed_queue.current_id(id);
      }
  private:
    FeedQueue& feed_queue;
    std::vector<Spec> urls;
  };

} // namespace Karrot

#endif /* KARROT_FEED_QUEUE_HPP */
