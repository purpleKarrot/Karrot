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
#include "minisat/Solver.h"
#include "url.hpp"
#include "quark.hpp"
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace karrot
{

static const int ASTERISK = string_to_quark("*");

static inline int hash_artefact(int domain, int project)
  {
  return domain << 3 ^ project ^ project << 11;
  }

static inline int hash_artefact(const Implementation2& ident)
  {
  return hash_artefact(ident.domain, ident.project);
  }

static void query(const Hash& hash, const std::vector<Implementation2>& entries,
    const Spec& dep, vec<Lit>& res)
  {
  int id;
  std::size_t h = hash_artefact(dep.domain, dep.project) & hash.mask;
  std::size_t hh = hash.begin();
  while ((id = hash.table[h]) != 0)
    {
    const Implementation2& e = entries[id - 1];
    if (dep.domain == e.domain && dep.project == e.project
        && (quark_to_string(dep.component) == e.base.component || e.base.component == "*")
        && evaluate(dep.query, to_quark(e.base.version), e.base.variant))
      {
      res.push(Lit(id - 1));
      }
    h = hash.next(h, hh);
    }
  }

static void print(const std::vector<Implementation2>& entries, const vec<Lit>& lits)
  {
  for (int i = 0; i < lits.size(); ++i)
    {
    int index = var(lits[i]);
    // TODO:
    //std::cout << index << ": " << entries[index].id << std::endl;
    }
  }

static void dependency_clauses(const Hash& hash, const std::vector<Implementation2>& entries, Solver& solver)
  {
  for (std::size_t i = 0; i < entries.size(); ++i)
    {
    for (const Spec& dependency : entries[i].depends)
      {
      vec<Lit> clause;
      clause.push(~Lit(i));
      query(hash, entries, dependency, clause);
      if (clause.size() == 1)
        {
        // TODO:
        //std::cout << "unsatisfied dependency: \n" << dependency << std::endl;
        //std::cout << "required by: " << entries[i].name << std::endl;
        solver.addUnit(clause[0]);
        }
      else
        {
        solver.addClause(clause);
        }
      }
    }
  }

static void explicit_conflict_clauses(const Hash& hash, const std::vector<Implementation2>& entries, Solver& solver)
  {
  for (std::size_t i = 0; i < entries.size(); ++i)
    {
    Lit lit = ~Lit(i);
    for (const Spec& dependency : entries[i].conflicts)
      {
      vec<Lit> conflicts;
      query(hash, entries, dependency, conflicts);
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
static void implicit_conflict_clauses(const std::vector<Implementation2>& entries, Solver& solver)
  {
  for (std::size_t i = 0; i < entries.size(); ++i)
    {
    for (std::size_t k = i + 1; k < entries.size(); ++k)
      {
      const Implementation2& id1 = entries[i];
      const Implementation2& id2 = entries[k];
      if (id1.project != id2.project || id1.domain != id2.domain)
        {
        break;
        }
      if (id1.base.version != id2.base.version ||
          id1.base.variant != id2.base.variant ||
          id1.base.component == id2.base.component ||
          id1.base.component == "*" ||
          id2.base.component == "*")
        {
        solver.addBinary(~Lit(i), ~Lit(k));
        }
      }
    }
  }

std::vector<int> solve(
    const std::vector<Implementation2>& database,
    const std::set<Spec>& projects)
  {
  Hash hash;
  if (hash.rehash_needed(database.size()))
    {
    for (std::size_t i = 0; i < database.size(); ++i)
      {
      std::size_t h = hash_artefact(database[i]) & hash.mask;
      std::size_t hh = hash.begin();
      while (hash.table[h] != 0)
        {
        h = hash.next(h, hh);
        }
      hash.table[h] = i + 1;
      }
    }

  vec<Lit> request;
  for (const Spec& spec : projects)
    {
    vec<Lit> choices;
    query(hash, database, spec, choices);
    if (choices.size() == 0)
      {
      // TODO: add spec
      throw std::runtime_error(std::string("impossible request"));
      }
    if (choices.size() == 1)
      {
      request.push(choices[0]);
      }
    else
      {
      std::cout << "multiple choices:" << std::endl;
      print(database, choices);
      int selection = 0;
      std::cout << "Select: " << std::flush;
      std::cin >> selection;
      request.push(Lit(selection));
      }
    }
  std::cout << "Request:" << std::endl;
  print(database, request);

  Solver solver;
  for (std::size_t i = 0; i < database.size(); ++i)
    {
    solver.newVar();
    }

  dependency_clauses(hash, database, solver);
  explicit_conflict_clauses(hash, database, solver);
  implicit_conflict_clauses(database, solver);

  if (!solver.solve(request))
    {
    throw std::runtime_error("UNSAT!");
    }

  std::vector<int> result;
  for (int i = 0; i < solver.nVars(); ++i)
    {
    if (solver.model[i] == l_True)
      {
      result.push_back(i);
      }
    }
  return result;
  }

} // namespace karrot
