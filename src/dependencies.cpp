/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "dependencies.hpp"
#include "feed_queue.hpp"
#include <boost/logic/tribool.hpp>

namespace Karrot
{

void Dependencies::replay(
    const std::string& component,
    const std::string& version,
    const KDictionary& values,
    std::vector<Spec>& depends,
    std::vector<Spec>& conflicts) const
  {
  if (component != name && component != "*")
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
          stack.push_back(entry.second.query.evaluate(version, values));
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
          stack.back() = entry.second.query.evaluate(version, values);
          }
        break;
      case ENDIF:
        stack.pop_back();
        break;
      case DEPENDS:
        if (stack.back())
          {
          feed_queue->push(entry.second);
          depends.push_back(entry.second);
          }
        break;
      case CONFLICTS:
        if (stack.back())
          {
          conflicts.push_back(entry.second);
          }
        break;
      }
    }
  }

} // namespace Karrot
