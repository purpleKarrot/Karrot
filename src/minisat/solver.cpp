/****************************************************************************************[Solver.C]
MiniSat -- Copyright (c) 2003-2005, Niklas Een, Niklas Sorensson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#include "solver.hpp"
#include <algorithm>
#include <cmath>


Solver::Solver(std::vector<Var>&& preferences)
    : order{std::move(preferences)}
  {
  assert(std::all_of(std::begin(order), std::end(order), [&](Var v)
    {
    return v < order.size();
    }));

  std::vector<Lit> dummy(2,lit_Undef);
  propagate_tmpbin = new Clause(dummy);
  analyze_tmpbin   = new Clause(dummy);
  dummy.pop_back();
  solve_tmpunit    = new Clause(dummy);

  std::size_t size = order.size();
  watches.resize(size * 2); // for positive and negative literals
  reason.resize(size, GClause_NULL);
  assigns.resize(size, toInt(l_Undef));
  level.resize(size, -1);
  analyze_seen.resize(size, 0);
  }

Solver::~Solver()
  {
  for (Clause* clause : learnts)
    {
    remove(clause, true);
    }
  for (Clause* clause : clauses)
    {
    if (clause != NULL)
      {
      remove(clause, true);
      }
    }
  remove(solve_tmpunit, true);
  remove(analyze_tmpbin, true);
  remove(propagate_tmpbin, true);
  }

//=================================================================================================
// Helper functions:


// Pre-condition: 'elem' must exists in 'ws' OR 'ws' must be empty.
bool removeWatch(std::vector<GClause>& ws, GClause elem)
  {
  if (ws.empty())
    {
    return false; // (skip lists that are already cleared)
    }
  auto found = std::find(std::begin(ws), std::end(ws), elem);
  assert(found != std::end(ws));
  ws.erase(found);
  return true;
  }

// Just like 'assert()' but expression will be evaluated in the release version as well.
inline void check(bool expr)
  {
  assert(expr);
  }


//=================================================================================================
// Operations on clauses:


/*_________________________________________________________________________________________________
|
|  newClause : (ps : const std::vector<Lit>&) (learnt : bool)  ->  [void]
|  
|  Description:
|    Allocate and add a new clause to the SAT solvers clause database. If a conflict is detected,
|    the 'ok' flag is cleared and the solver is in an unusable state (must be disposed).
|  
|  Input:
|    ps     - The new clause as a vector of literals.
|    learnt - Is the clause a learnt clause? For learnt clauses, 'ps[0]' is assumed to be the
|             asserting literal. An appropriate 'enqueue()' operation will be performed on this
|             literal. One of the watches will always be on this literal, the other will be set to
|             the literal with the highest decision level.
|  
|  Effect:
|    Activity heuristics are updated.
|________________________________________________________________________________________________@*/
void Solver::add_clause(std::vector<Lit> const& ps_, bool learnt)
  {
  if (!ok)
    {
    return;
    }

  std::vector<Lit> qs;
  if (!learnt)
    {
    assert(decisionLevel() == 0);
    qs = ps_; // Make a copy of the input vector.

    // Remove duplicates:
    std::sort(qs.begin(), qs.end());
    qs.erase(std::unique(qs.begin(), qs.end()), qs.end());

    // Check if clause is satisfied:
    for (int i = 0; i < qs.size() - 1; ++i)
      {
      if (qs[i] == ~qs[i + 1])
        {
        return;
        }
      }
    for (Lit const& lit : qs)
      {
      if (value(lit) == l_True)
        {
        return;
        }
      }

    // Remove false literals:
    auto is_false = [&](Lit const& lit) -> bool
      {
      return (value(lit) == l_False);
      };
    qs.erase(std::remove_if(qs.begin(), qs.end(), is_false), qs.end());
    }

  const std::vector<Lit>& ps = learnt ? ps_ : qs; // 'ps' is now the (possibly) reduced vector of literals.

  if (ps.size() == 0)
    {
    ok = false;
    }
  else if (ps.size() == 1)
    {
    // NOTE: If enqueue takes place at root level, the assignment will be lost in incremental use (it doesn't seem to hurt much though).
    if (!enqueue(ps[0]))
      {
      ok = false;
      }
    }
  else if (ps.size() == 2)
    {
    // Create special binary clause watch:
    watches[index(~ps[0])].push_back(GClause::create(ps[1]));
    watches[index(~ps[1])].push_back(GClause::create(ps[0]));

    if (learnt)
      {
      check(enqueue(ps[0], GClause::create(~ps[1])));
      }
    n_bin_clauses++;
    }
  else
    {
    // Allocate clause:
    Clause* c = new Clause(ps);

    if (learnt)
      {
      // Put the second watch on the literal with highest decision level:
      int max_i = 1;
      int max = level[var(ps[1])];
      for (int i = 2; i < ps.size(); i++)
        {
        if (level[var(ps[i])] > max)
          {
          max = level[var(ps[i])];
          max_i = i;
          }
        }
      (*c)[1] = ps[max_i];
      (*c)[max_i] = ps[1];

      // Bump, enqueue, store clause:
      check(enqueue((*c)[0], GClause::create(c)));
      learnts.push_back(c);
      }
    else
      {
      // Store clause:
      clauses.push_back(c);
      }
    // Watch clause:
    watches[index(~(*c)[0])].push_back(GClause::create(c));
    watches[index(~(*c)[1])].push_back(GClause::create(c));
    }
  }


// Disposes a clauses and removes it from watcher lists. NOTE! Low-level; does NOT change the 'clauses' and 'learnts' vector.
//
void Solver::remove(Clause* c, bool just_dealloc)
  {
  if (!just_dealloc)
    {
    if (c->size() == 2)
      {
      removeWatch(watches[index(~(*c)[0])], GClause::create((*c)[1]));
      removeWatch(watches[index(~(*c)[1])], GClause::create((*c)[0]));
      }
    else
      {
      removeWatch(watches[index(~(*c)[0])], GClause::create(c));
      removeWatch(watches[index(~(*c)[1])], GClause::create(c));
      }
    }

  delete c;
  }


// Can assume everything has been propagated! (esp. the first two literals are != l_False, unless
// the clause is binary and satisfied, in which case the first literal is true)
// Returns True if clause is satisfied (will be removed), False otherwise.
//
bool Solver::simplify(Clause* c) const
  {
  assert(decisionLevel() == 0);
  for (int i = 0; i < c->size(); i++)
    {
    if (value((*c)[i]) == l_True)
      {
      return true;
      }
    }
  return false;
  }


//=================================================================================================
// Minor methods:


// Returns FALSE if immediate conflict.
bool Solver::assume(Lit p)
  {
  trail_lim.push_back(trail.size());
  return enqueue(p);
  }


// Revert to the state at given level.
void Solver::cancelUntil(int level)
  {
  if (decisionLevel() > level)
    {
    for (int c = trail.size() - 1; c >= trail_lim[level]; c--)
      {
      Var x = var(trail[c]);
      assigns[x] = toInt(l_Undef);
      reason[x] = GClause_NULL;
      order.push_back(x);
      }
    trail.resize(trail_lim[level]);
    trail_lim.resize(level);
    qhead = trail.size();
    }
  }


//=================================================================================================
// Major methods:


/*_________________________________________________________________________________________________
|
|  analyze : (confl : Clause*) (out_learnt : std::vector<Lit>&) (out_btlevel : int&)  ->  [void]
|  
|  Description:
|    Analyze conflict and produce a reason clause.
|  
|    Pre-conditions:
|      * 'out_learnt' is assumed to be cleared.
|      * Current decision level must be greater than root level.
|  
|    Post-conditions:
|      * 'out_learnt[0]' is the asserting literal at level 'out_btlevel'.
|  
|  Effect:
|    Will undo part of the trail, upto but not beyond the assumption of the current decision level.
|________________________________________________________________________________________________@*/
void Solver::analyze(Clause* _confl, std::vector<Lit>& out_learnt, int& out_btlevel)
  {
  GClause confl = GClause::create(_confl);
  std::vector<char>& seen = analyze_seen;
  int pathC = 0;
  Lit p = lit_Undef;

  // Generate conflict clause:
  //
  out_learnt.emplace_back(); // (leave room for the asserting literal)
  out_btlevel = 0;
  int index = trail.size() - 1;
  do
    {
    assert(confl != GClause_NULL); // (otherwise should be UIP)

    Clause& c = confl.isLit() ? ((*analyze_tmpbin)[1] = confl.lit(), *analyze_tmpbin) : *confl.clause();

    for (int j = (p == lit_Undef) ? 0 : 1; j < c.size(); j++)
      {
      Lit q = c[j];
      if (!seen[var(q)] && level[var(q)] > 0)
        {
        seen[var(q)] = 1;
        if (level[var(q)] == decisionLevel())
          {
          pathC++;
          }
        else
          {
          out_learnt.push_back(q);
          out_btlevel = std::max(out_btlevel, level[var(q)]);
          }
        }
      }

    // Select next clause to look at:
    while (!seen[var(trail[index--])])
      {
      }
    p = trail[index + 1];
    confl = reason[var(p)];
    seen[var(p)] = 0;
    pathC--;

    }
  while (pathC > 0);
  out_learnt[0] = ~p;

  int i, j;
  // Simplify conflict clause
  uint min_level = 0;
  for (i = 1; i < out_learnt.size(); i++)
    {
    min_level |= 1 << (level[var(out_learnt[i])] & 31); // (maintain an abstraction of levels involved in conflict)
    }

  analyze_toclear = out_learnt;

  auto pred = [&](Lit const& lit) -> bool
    {
    return reason[var(lit)] != GClause_NULL && analyze_removable(lit, min_level);
    };
  out_learnt.erase(std::remove_if(out_learnt.begin() + 1, out_learnt.end(), pred), out_learnt.end());

  for (int j = 0; j < analyze_toclear.size(); j++)
    {
    seen[var(analyze_toclear[j])] = 0; // ('seen[]' is now cleared)
    }
  }


// Check if 'p' can be removed. 'min_level' is used to abort early if visiting literals at a level that cannot be removed.
//
bool Solver::analyze_removable(Lit p, std::size_t min_level)
  {
  assert(reason[var(p)] != GClause_NULL);
  analyze_stack.clear();
  analyze_stack.push_back(p);
  int top = analyze_toclear.size();
  while (analyze_stack.size() > 0)
    {
    assert(reason[var(analyze_stack.back())] != GClause_NULL);
    GClause r = reason[var(analyze_stack.back())];
    analyze_stack.pop_back();
    Clause& c = r.isLit() ? ((*analyze_tmpbin)[1] = r.lit(), *analyze_tmpbin) : *r.clause();
    for (int i = 1; i < c.size(); i++)
      {
      Lit p = c[i];
      if (!analyze_seen[var(p)] && level[var(p)] != 0)
        {
        if (reason[var(p)] != GClause_NULL && ((1 << (level[var(p)] & 31)) & min_level) != 0)
          {
          analyze_seen[var(p)] = 1;
          analyze_stack.push_back(p);
          analyze_toclear.push_back(p);
          }
        else
          {
          for (int j = top; j < analyze_toclear.size(); j++)
            {
            analyze_seen[var(analyze_toclear[j])] = 0;
            }
          analyze_toclear.resize(top);
          return false;
          }
        }
      }
    }

  return true;
  }


/*_________________________________________________________________________________________________
|
|  analyzeFinal : (confl : Clause*) (skip_first : bool)  ->  [void]
|  
|  Description:
|    Specialized analysis procedure to express the final conflict in terms of assumptions.
|    'root_level' is allowed to point beyond end of trace (useful if called after conflict while
|    making assumptions). If 'skip_first' is TRUE, the first literal of 'confl' is  ignored (needed
|    if conflict arose before search even started).
|________________________________________________________________________________________________@*/
void Solver::analyzeFinal(Clause* confl, bool skip_first)
  {
  // -- NOTE! This code is relatively untested. Please report bugs!
  conflict.clear();
  if (root_level == 0)
    {
    return;
    }

  std::vector<char>& seen = analyze_seen;
  for (int i = skip_first ? 1 : 0; i < confl->size(); i++)
    {
    Var x = var((*confl)[i]);
    if (level[x] > 0)
      {
      seen[x] = 1;
      }
    }

  int start = (root_level >= trail_lim.size()) ? trail.size() - 1 : trail_lim[root_level];
  for (int i = start; i >= trail_lim[0]; i--)
    {
    Var x = var(trail[i]);
    if (seen[x])
      {
      GClause r = reason[x];
      if (r == GClause_NULL)
        {
        assert(level[x] > 0);
        conflict.push_back(~trail[i]);
        }
      else
        {
        if (r.isLit())
          {
          Lit p = r.lit();
          if (level[var(p)] > 0)
            {
            seen[var(p)] = 1;
            }
          }
        else
          {
          Clause& c = *r.clause();
          for (int j = 1; j < c.size(); j++)
            {
            if (level[var(c[j])] > 0)
              {
              seen[var(c[j])] = 1;
              }
            }
          }
        }
      seen[x] = 0;
      }
    }
  }


/*_________________________________________________________________________________________________
|
|  enqueue : (p : Lit) (from : Clause*)  ->  [bool]
|  
|  Description:
|    Puts a new fact on the propagation queue as well as immediately updating the variable's value.
|    Should a conflict arise, FALSE is returned.
|  
|  Input:
|    p    - The fact to enqueue
|    from - [Optional] Fact propagated from this (currently) unit clause. Stored in 'reason[]'.
|           Default value is NULL (no reason).
|  
|  Output:
|    TRUE if fact was enqueued without conflict, FALSE otherwise.
|________________________________________________________________________________________________@*/
bool Solver::enqueue(Lit p, GClause from)
  {
  if (value(p) != l_Undef)
    {
    return value(p) != l_False;
    }

  assigns[var(p)] = toInt(lbool(!sign(p)));
  level[var(p)] = decisionLevel();
  reason[var(p)] = from;
  trail.push_back(p);
  return true;
  }


/*_________________________________________________________________________________________________
|
|  propagate : [void]  ->  [Clause*]
|  
|  Description:
|    Propagates all enqueued facts. If a conflict arises, the conflicting clause is returned,
|    otherwise NULL.
|  
|    Post-conditions:
|      * the propagation queue is empty, even if there was a conflict.
|________________________________________________________________________________________________@*/
Clause* Solver::propagate()
  {
  Clause* confl = NULL;
  while (qhead < trail.size())
    {
    simpDB_props--;

    Lit p = trail[qhead++]; // 'p' is enqueued fact to propagate.
    std::vector<GClause>& ws = watches[index(p)];
    std::vector<GClause>::iterator i, j, end;

    for (i = j = ws.begin(), end = ws.end(); i != end;)
      {
      if (i->isLit())
        {
        if (!enqueue(i->lit(), GClause::create(p)))
          {
          if (decisionLevel() == 0)
            {
            ok = false;
            }
          confl = propagate_tmpbin;
          (*confl)[1] = ~p;
          (*confl)[0] = i->lit();

          qhead = trail.size();
          // Copy the remaining watches:
          while (i < end)
            {
            *j++ = *i++;
            }
          }
        else
          {
          *j++ = *i++;
          }
        }
      else
        {
        Clause& c = *i->clause();
        i++;
        assert(c.size() > 2);
        // Make sure the false literal is data[1]:
        Lit false_lit = ~p;
        if (c[0] == false_lit)
          {
          c[0] = c[1], c[1] = false_lit;
          }

        assert(c[1] == false_lit);

        // If 0th watch is true, then clause is already satisfied.
        Lit first = c[0];
        lbool val = value(first);
        if (val == l_True)
          {
          *j++ = GClause::create(&c);
          }
        else
          {
          // Look for new watch:
          for (int k = 2; k < c.size(); k++)
            {
            if (value(c[k]) != l_False)
              {
              c[1] = c[k];
              c[k] = false_lit;
              watches[index(~c[1])].push_back(GClause::create(&c));
              goto FoundWatch;
              }
            }

          // Did not find watch -- clause is unit under assignment:
          *j++ = GClause::create(&c);
          if (!enqueue(first, GClause::create(&c)))
            {
            if (decisionLevel() == 0)
              {
              ok = false;
              }
            confl = &c;
            qhead = trail.size();
            // Copy the remaining watches:
            while (i < end)
              {
              *j++ = *i++;
              }
            }
          FoundWatch: ;
          }
        }
      }
    auto n = i - j;
    ws.erase(ws.end() - n, ws.end());
    }

  return confl;
  }


/*_________________________________________________________________________________________________
|
|  simplifyDB : [void]  ->  [bool]
|  
|  Description:
|    Simplify the clause database according to the current top-level assigment. Currently, the only
|    thing done here is the removal of satisfied clauses, but more things can be put here.
|________________________________________________________________________________________________@*/
void Solver::simplifyDB()
  {
  if (!ok)
    {
    return; // GUARD (public method)
    }
  assert(decisionLevel() == 0);

  if (propagate() != NULL)
    {
    ok = false;
    return;
    }

  if (nAssigns() == simpDB_assigns || simpDB_props > 0) // (nothing has changed or preformed a simplification too recently)
    {
    return;
    }

  // Clear watcher lists:
  for (int i = simpDB_assigns; i < nAssigns(); i++)
    {
    Lit p = trail[i];
    std::vector<GClause>& ws = watches[index(~p)];
    for (int j = 0; j < ws.size(); j++)
      {
      if (ws[j].isLit())
        {
        if (removeWatch(watches[index(~ws[j].lit())], GClause::create(p))) // (remove binary GClause from "other" watcher list)
          {
          n_bin_clauses--;
          }
        }
      }
    watches[index(p)].clear();
    watches[index(~p)].clear();
    }

  // Remove satisfied clauses:
  for (int type = 0; type < 2; type++)
    {
    std::vector<Clause*>& cs = type ? learnts : clauses;
    int j = 0;
    for (int i = 0; i < cs.size(); i++)
      {
      if (!locked(cs[i]) && simplify(cs[i])) // (the test for 'locked()' is currently superfluous, but without it the reason-graph is not correctly maintained for decision level 0)
        {
        remove(cs[i]);
        }
      else
        {
        cs[j++] = cs[i];
        }
      }
    cs.resize(j);
    }

  simpDB_assigns = nAssigns();
  simpDB_props = clauses.size() + learnts.size();
  }


/*_________________________________________________________________________________________________
|
|  search : (nof_conflicts : int) (nof_learnts : int) (params : const SearchParams&)  ->  [lbool]
|  
|  Description:
|    Search for a model the specified number of conflicts, keeping the number of learnt clauses
|    below the provided limit. NOTE! Use negative value for 'nof_conflicts' or 'nof_learnts' to
|    indicate infinity.
|  
|  Output:
|    'l_True' if a partial assigment that is consistent with respect to the clauseset is found. If
|    all variables are decision variables, this means that the clause set is satisfiable. 'l_False'
|    if the clause set is unsatisfiable. 'l_Undef' if the bound on number of conflicts is reached.
|________________________________________________________________________________________________@*/
lbool Solver::search(int nof_conflicts, int nof_learnts)
  {
  if (!ok)
    {
    return l_False; // GUARD (public method)
    }
  assert(root_level == decisionLevel());

  int conflictC = 0;
  model.clear();

  for (;;)
    {
    Clause* confl = propagate();
    if (confl != NULL)
      {
      // CONFLICT

      conflictC++;
      std::vector<Lit> learnt_clause;
      int backtrack_level;
      if (decisionLevel() == root_level)
        {
        // Contradiction found:
        analyzeFinal(confl);
        return l_False;
        }
      analyze(confl, learnt_clause, backtrack_level);
      cancelUntil(std::max(backtrack_level, root_level));
      add_clause(learnt_clause, true);

      // (this is ugly (but needed for 'analyzeFinal()') -- in future versions, we will backtrack past the 'root_level' and redo the assumptions)
      if (learnt_clause.size() == 1)
        {
        level[var(learnt_clause[0])] = 0;
        }
      }
    else
      {
      // NO CONFLICT

      if (nof_conflicts >= 0 && conflictC >= nof_conflicts)
        {
        // Reached bound on number of conflicts:
        cancelUntil(root_level);
        return l_Undef;
        }

      if (decisionLevel() == 0)
        {
        // Simplify the set of problem clauses:
        simplifyDB();
        assert(ok);
        }

      // New variable decision:
      auto select = [&]() -> Var
        {
        while (!order.empty())
          {
          Var next = order.back();
          order.pop_back();
          if (toLbool(assigns[next]) == l_Undef)
            {
            return next;
            }
          }
        return var_Undef;
        };
      Var next = select();

      if (next == var_Undef)
        {
        // Model found:
        model.resize(nVars());
        for (int i = 0; i < nVars(); i++)
          {
          model[i] = value(i);
          }
        cancelUntil(root_level);
        return l_True;
        }

      check(assume(~Lit(next)));
      }
    }
  }


/*_________________________________________________________________________________________________
|
|  solve : (assumps : const std::vector<Lit>&)  ->  [bool]
|  
|  Description:
|    Top-level solve. If using assumptions (non-empty 'assumps' vector), you must call
|    'simplifyDB()' first to see that no top-level conflict is present (which would put the solver
|    in an undefined state).
|________________________________________________________________________________________________@*/
bool Solver::solve(const std::vector<Lit>& assumps, KPrintFun log)
  {
  simplifyDB();
  if (!ok)
    {
    return false;
    }

  double nof_conflicts = 100;
  double nof_learnts = nClauses() / 3;
  lbool status = l_Undef;

  // Perform assumptions:
  root_level = assumps.size();
  for (int i = 0; i < assumps.size(); i++)
    {
    Lit p = assumps[i];
    assert(var(p) < nVars());
    if (!assume(p))
      {
      GClause r = reason[var(p)];
      if (r != GClause_NULL)
        {
        Clause* confl;
        if (r.isLit())
          {
          confl = propagate_tmpbin;
          (*confl)[1] = ~p;
          (*confl)[0] = r.lit();
          }
        else
          {
          confl = r.clause();
          }
        analyzeFinal(confl, true);
        conflict.push_back(~p);
        }
      else
        {
        conflict.clear();
        conflict.push_back(~p);
        }
      cancelUntil(0);
      return false;
      }
    Clause* confl = propagate();
    if (confl != NULL)
      {
      analyzeFinal(confl);
      assert(conflict.size() > 0);
      cancelUntil(0);
      return false;
      }
    }
  assert(root_level == decisionLevel());

  // Search:
  while (status == l_Undef)
    {
    status = search((int) nof_conflicts, (int) nof_learnts);
    nof_conflicts *= 1.5;
    nof_learnts *= 1.1;
    }

  cancelUntil(0);
  return status == l_True;
  }
