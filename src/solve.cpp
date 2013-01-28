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
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace Karrot
{

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
    const DatabaseEntry& entry1 = database[var1];
    const DatabaseEntry& entry2 = database[var2];
    const Implementation& impl1 = entry1.impl;
    const Implementation& impl2 = entry2.impl;
    if (impl1.component != "SOURCE" && impl2.component == "SOURCE")
      {
      return true;
      }
    if (impl1.component == "SOURCE" && impl2.component != "SOURCE")
      {
      return false;
      }
    if (entry1.depends.size() < entry2.depends.size())
      {
      return true;
      }
    if (entry1.depends.size() > entry2.depends.size())
      {
      return false;
      }
    return vercmp(impl1.version.c_str(), impl2.version.c_str()) < 0;
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
      const DatabaseEntry& entry1 = database[i];
      const DatabaseEntry& entry2 = database[k];
      if (entry1.id != entry2.id)
        {
        break;
        }
      const Implementation& impl1 = entry1.impl;
      const Implementation& impl2 = entry2.impl;
      if (impl1.version != impl2.version ||
          impl1.variant != impl2.variant ||
          impl1.component == impl2.component ||
          impl1.component == "*" || impl2.component == "*" ||
          impl1.component == "SOURCE" || impl2.component == "SOURCE")
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
    if (database[i].impl.component != "SOURCE")
      {
      continue;
      }
    for (std::size_t k = 0; k < database.size(); ++k)
      {
      if (database[k].impl.component == "SOURCE")
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

bool solve(const Database& database, const Requests& requests, std::vector<int>& model)
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
      std::clog << "no implementation satisfies the request" << std::endl;
      return false;
      }
    if (choices.size() == 1)
      {
      request.push(choices[0]);
      }
    else
      {
      solver.addClause(choices);
      }
    }
  if (request.size() == 0)
    {
    std::clog << "request is not specific enough" << std::endl;
    return false;
    }

  dependency_clauses(hash, database, solver);
  explicit_conflict_clauses(hash, database, solver);
  implicit_conflict_clauses(database, solver);
  source_conflict_clauses(database, solver);

  if (!solver.solve(request))
    {
    std::clog << "no solution exists, because of conflicts" << std::endl;
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
