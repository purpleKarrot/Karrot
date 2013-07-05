/****************************************************************************************[Solver.h]
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

#ifndef Solver_h
#define Solver_h

#include <vector>
#include <karrot.h>
#include "solver_types.hpp"

//=================================================================================================
// Solver -- the main class:


class Solver
  {
  private:
    // Temporaries (to reduce allocation overhead). Each variable is prefixed by the method in which is used:
    //
    std::vector<char>   analyze_seen;
    std::vector<Lit>    analyze_stack;
    std::vector<Lit>    analyze_toclear;
    Clause              propagate_tmpbin;
    Clause              analyze_tmpbin;
    Clause              solve_tmpunit;

  public:
    Solver(std::vector<Var>&& preferences);
    ~Solver();

    // Helpers: (semi-internal)
    //
    lbool   value(Var x) const { return toLbool(assigns[x]); }
    lbool   value(Lit p) const { return sign(p) ? ~toLbool(assigns[var(p)]) : toLbool(assigns[var(p)]); }

    int     nAssigns() { return trail.size(); }
    int     nClauses() { return clauses.size() + n_bin_clauses; }   // (minor difference from MiniSat without the GClause trick: learnt binary clauses will be counted as original clauses)
    int     nLearnts() { return learnts.size(); }

    // Problem specification:
    //
    int     nVars     ()                    { return assigns.size(); }
    void    addUnit   (Lit p)               { if (ok) ok = enqueue(p); }
    void    addBinary (Lit p, Lit q)        { add_clause({p, q}); }
    void    addTernary(Lit p, Lit q, Lit r) { add_clause({p, q, r}); }
    void    add_clause(std::vector<Lit> const& ps, bool learnt = false);

    // Solving:
    //
    bool    okay() { return ok; }       // FALSE means solver is in an conflicting state (must never be used again!)
    void    simplifyDB();
    bool    solve(const std::vector<Lit>& assumps, KPrintFun log);
    bool    solve(KPrintFun log) { std::vector<Lit> tmp; return solve(tmp, log); }

    std::vector<lbool>  model;          // If problem is satisfiable, this vector contains the model (if any).
    std::vector<Lit>    conflict;       // If problem is unsatisfiable (possibly under assumptions), this vector represent the conflict clause expressed in the assumptions.

  private:
    // Main internal methods:
    //
    bool        assume           (Lit p);
    void        cancelUntil      (int level);
    void        record           (const std::vector<Lit>& clause);

    void        analyze          (Clause* confl, std::vector<Lit>& out_learnt, int& out_btlevel); // (bt = backtrack)
    bool        analyze_removable(Lit p, std::size_t min_level);                                 // (helper method for 'analyze()')
    void        analyzeFinal     (Clause* confl,  bool skip_first = false);
    bool        enqueue          (Lit fact, GClause from = GClause_NULL);
    Clause*     propagate        ();
    lbool       search           (int nof_conflicts, int nof_learnts);

    // Operations on clauses:
    //
    void     remove          (Clause* c);
    bool     locked          (const Clause* c) const { GClause r = reason[var((*c)[0])]; return !r.isLit() && r.clause() == c; }
    bool     simplify        (Clause* c) const;

  private:
    bool                ok = true;        // If FALSE, the constraints are already unsatisfiable. No part of the solver state may be used!
    std::vector<Clause*>clauses;          // List of problem clauses.
    std::vector<Clause*>learnts;          // List of learnt clauses.
    int                 n_bin_clauses = 0;// Keep track of number of binary clauses "inlined" into the watcher lists (we do this primarily to get identical behavior to the version without the binary clauses trick).

    std::vector<Var>    order;            // Keeps track of the decision variable order.

    std::vector<std::vector<GClause>>
                        watches;          // 'watches[lit]' is a list of constraints watching 'lit' (will go there if literal becomes true).
    std::vector<char>   assigns;          // The current assignments (lbool:s stored as char:s).
    std::vector<Lit>    trail;            // Assignment stack; stores all assigments made in the order they were made.
    std::vector<int>    trail_lim;        // Separator indices for different decision levels in 'trail'.
    std::vector<GClause>reason;           // 'reason[var]' is the clause that implied the variables current value, or 'NULL' if none.
    std::vector<int>    level;            // 'level[var]' is the decision level at which assignment was made.
    int                 root_level;       // Level of first proper decision.
    int                 qhead = 0;        // Head of queue (as index into the trail -- no more explicit propagation queue in MiniSat).
    int                 simpDB_assigns=0; // Number of top-level assignments since last execution of 'simplifyDB()'.
    std::int64_t        simpDB_props = 0; // Remaining number of propagations that must be made before next execution of 'simplifyDB()'.
  };


//=================================================================================================
#endif
