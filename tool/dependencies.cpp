/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "dependencies.hpp"
#include <karrot.h>
#include "feed_queue.hpp"
#include <boost/logic/tribool.hpp>

namespace Karrot
{

void Dependencies::replay(Implementation& impl, StringPool& pool) const
  {
  auto ignore = [&]
    {
    if (impl.component == name || impl.component == STR_SOURCE)
      {
      return false;
      }
    if (impl.component == STR_ANY && name != STR_SOURCE)
      {
      return false;
      }
    return true;
    };
  if (ignore())
    {
    return;
    }
  std::vector<boost::tribool> stack{true};
  for (const Entry& entry : deps)
    {
    switch (entry.first)
      {
      case IF:
        if (stack.back() == true)
          {
          stack.push_back(entry.second.query.evaluate(impl.version, impl.values, pool));
          }
        else
          {
          stack.push_back(boost::indeterminate);
          }
        break;
      case ELSE:
        if (stack.back() == true)
          {
          stack.back() = boost::indeterminate;
          }
        else if (stack.back() == false)
          {
          stack.back() = true;
          }
        break;
      case ELSEIF:
        if (stack.back() == true)
          {
          stack.back() = boost::indeterminate;
          }
        else if (stack.back() == false)
          {
          stack.back() = entry.second.query.evaluate(impl.version, impl.values, pool);
          }
        break;
      case ENDIF:
        stack.pop_back();
        break;
      case DEPENDS:
        if (stack.back())
          {
          feed_queue->push(entry.second.id);
          impl.depends.push_back(entry.second);
          }
        break;
      case CONFLICTS:
        if (stack.back())
          {
          impl.conflicts.push_back(entry.second);
          }
        break;
      }
    }
  }

} // namespace Karrot
