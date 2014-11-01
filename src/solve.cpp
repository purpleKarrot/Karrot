/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot.h>
#include "vercmp.hpp"
#include "minisat/solver.hpp"
#include <boost/format.hpp>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include "print.hpp"

namespace Karrot
{

// 1. prefer binary packages
// 2. prefer fewer dependencies
// 3. prefer older releases
static std::vector<Var> make_preferences(const Database& database, StringPool const& pool)
  {
  Var i = 0;
  std::vector<Var> preferences(database.size());
  std::generate(std::begin(preferences), std::end(preferences),
    [&i]() -> Var
    {
    return i++;
    });
  std::sort(std::begin(preferences), std::end(preferences),
    [&database, &pool](Var var1, Var var2) -> bool
    {
    const Implementation& impl1 = database[var1];
    const Implementation& impl2 = database[var2];
    if (impl1.component != STR_SOURCE && impl2.component == STR_SOURCE)
      {
      return true;
      }
    if (impl1.component == STR_SOURCE && impl2.component != STR_SOURCE)
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
    return vercmp(impl1.version, impl2.version, pool) < 0;
    });
  return preferences;
  }

static void dependency_clauses(
    const Database& database,
    Solver& solver, StringPool const& pool)
  {
  for (std::size_t i = 0; i < database.size(); ++i)
    {
    for (const Spec& spec : database[i].depends)
      {
      std::vector<Lit> clause{~Lit(i)};
      for (std::size_t k = 0; k < database.size(); ++k)
        {
        if (spec.satisfies(database[k], pool))
          {
          clause.push_back(Lit(k));
          }
        }
      if (clause.size() == 1)
        {
        std::clog << boost::format("Warning: No implementation satisfies '%1%'\n") % SpecPrinter{spec, pool};
        std::clog << boost::format("Warning: blacklisting '%1%'\n") % ImplPrinter{database[i], pool};
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
    const Database& database,
    Solver& solver, StringPool const& pool)
  {
  for (std::size_t i = 0; i < database.size(); ++i)
    {
    for (const Spec& spec : database[i].conflicts)
      {
      for (std::size_t k = 0; k < database.size(); ++k)
        {
        if (spec.satisfies(database[k], pool))
          {
          solver.addBinary(~Lit(i), ~Lit(k));
          }
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
        impl1.component == STR_ANY || impl2.component == STR_ANY ||
        impl1.component == STR_SOURCE || impl2.component == STR_SOURCE)
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
static void source_conflict_clauses(const Database& database, Solver& solver, StringPool const& pool)
  {
  for (std::size_t i = 0; i < database.size(); ++i)
    {
    if (database[i].component != STR_SOURCE)
      {
      continue;
      }
    for (std::size_t k = 0; k < database.size(); ++k)
      {
      if (database[k].component == STR_SOURCE)
        {
        continue;
        }
      for (const Spec& spec : database[k].depends)
        {
        if (spec.satisfies(database[i], pool))
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
    std::vector<int>& model, StringPool const& pool)
  {
  Solver solver(make_preferences(database, pool));

  std::vector<Lit> request;
  for (const Spec& spec : requests)
    {
    std::vector<Lit> choices;
    for (std::size_t k = 0; k < database.size(); ++k)
      {
      if (spec.satisfies(database[k], pool))
        {
        choices.push_back(Lit(k));
        }
      }
    if (choices.size() == 0)
      {
      std::clog << boost::format("No implementation for '%1%':\n") % SpecPrinter{spec, pool};
      for (auto& entry : database)
        {
        std::clog << boost::format("  - %1%\n") % ImplPrinter{entry, pool};
        }
      return false;
      }
    if (choices.size() == 1)
      {
      std::clog << boost::format("Exactly one implementation for '%1%':\n") % SpecPrinter{spec, pool};
      std::clog << boost::format("  + %1%\n") % ImplPrinter{database[var(choices[0])], pool};
      request.push_back(choices[0]);
      }
    else
      {
      std::clog << boost::format("Multiple implementations for '%1%':\n") % SpecPrinter{spec, pool};
      for (int i = 0; i < choices.size(); ++i)
        {
        std::clog << boost::format("  + %1%\n") % ImplPrinter{database[var(choices[i])], pool};
        }
      solver.add_clause(choices);
      }
    }
  if (request.size() == 0)
    {
    std::clog << "Warning: request is ambiguous.\n";
    }

  dependency_clauses(database, solver, pool);
  explicit_conflict_clauses(database, solver, pool);
  implicit_conflict_clauses(database, solver);
  source_conflict_clauses(database, solver, pool);

  if (!solver.solve(request))
    {
    std::clog << "no solution exists, because of conflicts.\n";
    for (Lit const& lit : solver.conflict)
      {
      std::clog << boost::format("  %1% %2%\n") % (sign(lit) ? "-" : "+") % ImplPrinter{database[var(lit)], pool};
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
