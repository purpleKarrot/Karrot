/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_VARORDER_H
#define KARROT_VARORDER_H

#include "solver_types.hpp"
#include <vector>

class VarOrder
  {
  public:
    VarOrder(std::vector<char> const& assigns, std::vector<Var>&& preferences)
        : assigns(assigns), preferences(std::move(preferences))
      {
      }
  public:
    void undo(Var x)
      {
      preferences.push_back(x);
      }
    Var select()
      {
      while (!preferences.empty())
        {
        Var next = preferences.back();
        preferences.pop_back();
        if (toLbool(assigns[next]) == l_Undef)
          {
          return next;
          }
        }
      return var_Undef;
      }
  private:
    std::vector<char> const& assigns;
    std::vector<Var> preferences;
};

#endif /* KARROT_VARORDER_H */
