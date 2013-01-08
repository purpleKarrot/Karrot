/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_DEPENDENCIES_HPP
#define KARROT_DEPENDENCIES_HPP

#include <vector>
#include <karrot/spec.hpp>
#include <karrot/quark.hpp>
#include "query.hpp"
#include <utility>
#include <iostream>

namespace karrot
{

static const int ASTERISK = string_to_quark("*");

class Dependencies
  {
  public:
    Dependencies(int name = 0) :
        name(name)
      {
      }
    void start_if(int test)
      {
      deps.emplace_back(IF, Spec(0, 0, 0, parse_query(test)));
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
    void start_elseif(int test)
      {
      Entry& entry = deps.back();
      if (entry.first != ENDIF)
        {
        std::cout << "ERROR: <elseif> has no matching <if>!" << std::endl;
        }
      entry = Entry(ELSEIF, Spec(0, 0, 0, parse_query(test)));
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
    void replay(int component, int version, int values,
        std::vector<Spec>& depends,
        std::vector<Spec>& conflicts) const
      {
      if (component != name && component != ASTERISK)
        {
        return;
        }
      std::vector<bool> stack; //{true};
      stack.push_back(true);
      for (const Entry& entry : deps)
        {
        switch (entry.first)
          {
          case DEPENDS:
            if (stack.back())
              {
              depends.push_back(entry.second);
              }
            break;
          case CONFLICTS:
            if (stack.back())
              {
              conflicts.push_back(entry.second);
              }
            break;
          case IF:
            stack.push_back(stack.back() && evaluate(entry.second.query, version, values));
            break;
          case ELSEIF:
            stack.back() = !stack.back() && evaluate(entry.second.query, version, values);
            break;
          case ENDIF:
            stack.pop_back();
            break;
          }
        }
      }
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
    typedef std::pair<Code, Spec> Entry;
    std::vector<std::pair<Code, Spec>> deps;
  };

} // namespace

#endif /* KARROT_DEPENDENCIES_HPP */
