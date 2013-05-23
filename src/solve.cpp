/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "solve.hpp"
#include "hash.hpp"
#include "query.hpp"
#include "vercmp.hpp"
#include "minisat/Solver.h"
#include "url.hpp"
#include "log.hpp"
#include <algorithm>
#include <stdexcept>

namespace Karrot
{

static const String SOURCE{"SOURCE"};

static inline std::size_t hash_artefact(const std::string& id)
  {
  std::hash<std::string> hash_fn;
  return hash_fn(id);
  }

static void query(
    const Hash& hash,
    const Database& database,
    const Spec& spec,
    vec<Lit>& res)
  {
  int id;
  std::size_t h = hash_artefact(spec.id) & hash.mask;
  std::size_t hh = hash.begin();
  while ((id = hash.table[h]) != 0)
    {
    if (satisfies(database[id - 1], spec))
      {
      res.push(Lit(id - 1));
      }
    h = hash.next(h, hh);
    }
  }

// 1. prefer binary packages
// 2. prefer fewer dependencies
// 3. prefer older releases
static std::vector<Var> make_preferences(const Database& database)
  {
  Var i = 0;
  std::vector<Var> preferences(database.size());
  std::generate(std::begin(preferences), std::end(preferences),
    [&i]() -> Var
    {
    return i++;
    });
  std::sort(std::begin(preferences), std::end(preferences),
    [&database](Var var1, Var var2) -> bool
    {
    const KImplementation& impl1 = database[var1];
    const KImplementation& impl2 = database[var2];
    if (impl1.component != SOURCE && impl2.component == SOURCE)
      {
      return true;
      }
    if (impl1.component == SOURCE && impl2.component != SOURCE)
      {
      return false;
      }
    if (impl1.depends.size() < impl2.depends.size())
      {
      return true;
      }
    if (impl1.depends.size() > impl2.depends.size())
      {
      return false;
      }
    return vercmp(impl1.version, impl2.version) < 0;
    });
  return std::move(preferences);
  }

static void dependency_clauses(
    const Hash& hash,
    const Database& database,
    Solver& solver)
  {
  for (std::size_t i = 0; i < database.size(); ++i)
    {
    for (const Spec& spec : database[i].depends)
      {
      vec<Lit> clause;
      clause.push(~Lit(i));
      query(hash, database, spec, clause);
      if (clause.size() == 1)
        {
        solver.addUnit(clause[0]);
        }
      else
        {
        solver.addClause(clause);
        }
      }
    }
  }

static void explicit_conflict_clauses(
    const Hash& hash,
    const Database& database,
    Solver& solver)
  {
  for (std::size_t i = 0; i < database.size(); ++i)
    {
    Lit lit = ~Lit(i);
    for (const Spec& spec : database[i].conflicts)
      {
      vec<Lit> conflicts;
      query(hash, database, spec, conflicts);
      for (int k = 0; k < conflicts.size(); ++k)
        {
        solver.addBinary(lit, ~conflicts[k]);
        }
      }
    }
  }

// Implementations of the same project either complement or conflict each other.
// Example: the component runtime(exe, dll) and develop(lib, hpp) complement
// each other when both have the same version and variant. If version and
// variant differ, the version does not match, or one implementation provides
// all components, there is a conflict.
static void implicit_conflict_clauses(const Database& database, Solver& solver)
  {
  for (std::size_t i = 0; i < database.size(); ++i)
    {
    for (std::size_t k = i + 1; k < database.size(); ++k)
      {
      const KImplementation& impl1 = database[i];
      const KImplementation& impl2 = database[k];
      if (impl1.id != impl2.id)
        {
        break;
        }
      if (impl1.version != impl2.version ||
          impl1.variant != impl2.variant ||
          impl1.component == impl2.component ||
          impl1.component == "*" || impl2.component == "*" ||
          impl1.component == SOURCE || impl2.component == SOURCE)
        {
        solver.addBinary(~Lit(i), ~Lit(k));
        }
      }
    }
  }

// If a project is built from source, all dependent projects should be built
// from source too.
static void source_conflict_clauses(const Database& database, Solver& solver)
  {
  for (std::size_t i = 0; i < database.size(); ++i)
    {
    if (database[i].component != SOURCE)
      {
      continue;
      }
    for (std::size_t k = 0; k < database.size(); ++k)
      {
      if (database[k].component == SOURCE)
        {
        continue;
        }
      for (const Spec& spec : database[k].depends)
        {
        if (satisfies(database[i], spec))
          {
          solver.addBinary(~Lit(i), ~Lit(k));
          }
        }
      }
    }
  }

bool solve(
    const Database& database,
    const Requests& requests,
    bool ignore_source_conflicts,
    KPrintFun log,
    std::vector<int>& model)
  {
  Hash hash;
  if (hash.rehash_needed(database.size()))
    {
    for (std::size_t i = 0; i < database.size(); ++i)
      {
      std::size_t h = hash_artefact(database[i].id) & hash.mask;
      std::size_t hh = hash.begin();
      while (hash.table[h] != 0)
        {
        h = hash.next(h, hh);
        }
      hash.table[h] = i + 1;
      }
    }

  Solver solver(make_preferences(database));
  for (std::size_t i = 0; i < database.size(); ++i)
    {
    solver.newVar();
    }

  vec<Lit> request;
  for (const Spec& spec : requests)
    {
    vec<Lit> choices;
    query(hash, database, spec, choices);
    if (choices.size() == 0)
      {
      Log(log, "No implementation for '%1%':") % spec;
      for (auto& entry : database)
        {
        Log(log, "  - %1%") % entry;
        }
      return false;
      }
    if (choices.size() == 1)
      {
      Log(log, "Exactly one implementation for '%1%':") % spec;
      Log(log, "  + %1%") % database[var(choices[0])];
      request.push(choices[0]);
      }
    else
      {
      Log(log, "Multiple implementations for '%1%':") % spec;
      for (int i = 0; i < choices.size(); ++i)
        {
        Log(log, "  + %1%") % database[var(choices[i])];
        }
      solver.addClause(choices);
      }
    }
  if (request.size() == 0)
    {
    log("Warning: request is ambiguous.");
    }

  dependency_clauses(hash, database, solver);
  explicit_conflict_clauses(hash, database, solver);
  implicit_conflict_clauses(database, solver);

  if (!ignore_source_conflicts)
    {
    source_conflict_clauses(database, solver);
    }

  if (!solver.solve(request, log))
    {
    log("no solution exists, because of conflicts");
    vec<Lit> const& conflict = solver.conflict;
    for (int i = 0; i < conflict.size(); ++i)
      {
      Lit const& lit = conflict[i];
      Log(log, "  %1% %2%") % (sign(lit) ? "-" : "+") % database[var(lit)];
      }
    return false;
    }
  for (int i = 0; i < solver.nVars(); ++i)
    {
    if (solver.model[i] == l_True)
      {
      model.push_back(i);
      }
    }
  return true;
  }

} // namespace Karrot
