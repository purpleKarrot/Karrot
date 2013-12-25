/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_DEPENDENCIES_HPP
#define KARROT_DEPENDENCIES_HPP

#include "spec.hpp"

namespace Karrot
{
class FeedPreQueue;
class Implementation;

class Dependencies
  {
  public:
    Dependencies(FeedPreQueue& feed_queue, const std::string& name = std::string())
      : name(name)
      , feed_queue(&feed_queue)
      {
      }
    void start_if(const std::string& test)
      {
      deps.emplace_back(IF, Spec(std::string(), std::string(), test));
      }
    void start_else()
      {
      Entry& entry = deps.back();
      if (entry.first != ENDIF)
        {
        std::cout << "ERROR: <else> has no matching <if>!" << std::endl;
        }
      entry.first = ELSE;
      }
    void start_elseif(const std::string& test)
      {
      Entry& entry = deps.back();
      if (entry.first != ENDIF)
        {
        std::cout << "ERROR: <elseif> has no matching <if>!" << std::endl;
        }
      entry = Entry(ELSEIF, Spec(std::string(), std::string(), test));
      }
    void end_if()
      {
      deps.emplace_back(ENDIF, Spec());
      }
    void depends(const Spec& spec)
      {
      deps.emplace_back(DEPENDS, spec);
      }
    void conflicts(const Spec& spec)
      {
      deps.emplace_back(CONFLICTS, spec);
      }
    void replay(Implementation& impl) const;
  private:
    enum Code
      {
      IF,
      ELSE,
      ELSEIF,
      ENDIF,
      DEPENDS,
      CONFLICTS,
      };
    std::string name;
    using Entry = std::pair<Code, Spec>;
    std::vector<Entry> deps;
    FeedPreQueue* feed_queue;
  };

} // namespace Karrot

#endif /* KARROT_DEPENDENCIES_HPP */
