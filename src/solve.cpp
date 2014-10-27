/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot/solve.hpp>
#include "hash.hpp"
#include <karrot/query.hpp>
#include "vercmp.hpp"
#include "minisat/solver.hpp"
#include <boost/format.hpp>
#include <algorithm>
#include <stdexcept>

namespace Karrot
{

static const String SOURCE{"SOURCE"};

static std::ostream& operator<<(std::ostream &os, Implementation const& impl)
  {
  os << impl.id;
  if (!impl.component.get().empty())
    {
    os << '#' << impl.component;
    }
  if (!impl.version.get().empty())
    {
    os << ' ' << impl.version;
    }
  return os;
  }

static inline std::size_t hash_artefact(const std::string& id)
  {
  std::hash<std::string> hash_fn;
  return hash_fn(id);
  }

static void query(
    const Hash& hash,
    const Database& database,
    const Spec& spec,
    std::vector<Lit>& res)
  {
  int id;
  std::size_t h = hash_artefact(spec.id) & hash.mask;
  std::size_t hh = hash.begin();
  while ((id = hash.table[h]) != 0)
    {
    if (satisfies(database[id - 1], spec))
      {
      res.push_back(Lit(id - 1));
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
    const Implementation& impl1 = database[var1];
    const Implementation& impl2 = database[var2];
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
      std::vector<Lit> clause{~Lit(i)};
      query(hash, database, spec, clause);
      if (clause.size() == 1)
        {
        std::clog << boost::format("Warning: No implementation satisfies '%1%'\n") % spec;
        std::clog << boost::format("Warning: blacklisting '%1%'\n") % database[i];
        solver.addUnit(clause[0]);
        }
      else
        {
        solver.add_clause(clause);
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
      std::vector<Lit> conflicts;
      query(hash, database, spec, conflicts);
      for (Lit const& conflict : conflicts)
        {
        solver.addBinary(lit, ~conflict);
        }
      }
    }
  }

// Implementations of the same project either complement or conflict each other.
// Example: the component runtime(exe, dll) and develop(lib, hpp) complement
// each other when both have the same version. If the version differs, or one
// implementation provides all components, there is a conflict.
static bool implicitly_conflicts(
    Implementation const& impl1,
    Implementation const& impl2)
{
  if (impl1.id == impl2.id)
    {
    if (impl1.version != impl2.version ||
        impl1.component == "*" || impl2.component == "*" ||
        impl1.component == SOURCE || impl2.component == SOURCE)
      {
      return true;
      }
    }
  return false;
}

static void implicit_conflict_clauses(const Database& database, Solver& solver)
  {
  for (std::size_t i = 0; i < database.size(); ++i)
    {
    for (std::size_t k = i + 1; k < database.size(); ++k)
      {
      if (implicitly_conflicts(database[i],  database[k]))
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

  std::vector<Lit> request;
  for (const Spec& spec : requests)
    {
    std::vector<Lit> choices;
    query(hash, database, spec, choices);
    if (choices.size() == 0)
      {
      std::clog << boost::format("No implementation for '%1%':\n") % spec;
      for (auto& entry : database)
        {
        std::clog << boost::format("  - %1%\n") % entry;
        }
      return false;
      }
    if (choices.size() == 1)
      {
      std::clog << boost::format("Exactly one implementation for '%1%':\n") % spec;
      std::clog << boost::format("  + %1%\n") % database[var(choices[0])];
      request.push_back(choices[0]);
      }
    else
      {
      std::clog << boost::format("Multiple implementations for '%1%':\n") % spec;
      for (int i = 0; i < choices.size(); ++i)
        {
        std::clog << boost::format("  + %1%\n") % database[var(choices[i])];
        }
      solver.add_clause(choices);
      }
    }
  if (request.size() == 0)
    {
    std::clog << "Warning: request is ambiguous.\n";
    }

  dependency_clauses(hash, database, solver);
  explicit_conflict_clauses(hash, database, solver);
  implicit_conflict_clauses(database, solver);
  source_conflict_clauses(database, solver);

  if (!solver.solve(request))
    {
    std::clog << "no solution exists, because of conflicts.\n";
    for (Lit const& lit : solver.conflict)
      {
      std::clog << boost::format("  %1% %2%\n") % (sign(lit) ? "-" : "+") % database[var(lit)];
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
