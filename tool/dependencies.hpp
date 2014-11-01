/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_DEPENDENCIES_HPP
#define KARROT_DEPENDENCIES_HPP

#include <karrot/spec.hpp>
#include <iostream>

namespace Karrot
{

class FeedQueue;
struct Implementation;

class Dependencies
  {
  public:
    Dependencies(FeedQueue& feed_queue, int name = 0)
      : name(name)
      , feed_queue(&feed_queue)
      {
      }
    void start_if(const std::string& test, StringPool& pool)
      {
      deps.emplace_back(IF, Spec(std::string(), std::string(), test, pool));
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
    void start_elseif(const std::string& test, StringPool& pool)
      {
      Entry& entry = deps.back();
      if (entry.first != ENDIF)
        {
        std::cout << "ERROR: <elseif> has no matching <if>!" << std::endl;
        }
      entry = Entry(ELSEIF, Spec(std::string(), std::string(), test, pool));
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
    void replay(Implementation& impl, StringPool& pool) const;
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
    int name;
    using Entry = std::pair<Code, Spec>;
    std::vector<Entry> deps;
    FeedQueue* feed_queue;
  };

} // namespace Karrot

#endif /* KARROT_DEPENDENCIES_HPP */
