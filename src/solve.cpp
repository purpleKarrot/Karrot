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
#include <karrot/url.hpp>
#include <karrot/quark.hpp>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <boost/foreach.hpp>

namespace karrot
{

static const int ASTERISK = string_to_quark("*");

static inline int hash_artefact(int domain, int project)
  {
  return domain << 3 ^ project ^ project << 11;
  }

static inline int hash_artefact(const Identification& ident)
  {
  return hash_artefact(ident.domain, ident.project);
  }

static void query(const Hash& hash, const std::vector<Deliverable>& entries,
    const Spec& dep, vec<Lit>& res)
  {
  int id;
  std::size_t h = hash_artefact(dep.domain, dep.project) & hash.mask;
  std::size_t hh = hash.begin();
  while ((id = hash.table[h]) != 0)
    {
    const Identification& e = entries[id - 1].id;
    if (dep.domain == e.domain && dep.project == e.project
        && (dep.component == e.component || e.component == ASTERISK)
        && evaluate(dep.query, e.version, e.variant))
      {
      res.push(Lit(id - 1));
      }
    h = hash.next(h, hh);
    }
  }

static void print(const std::vector<Deliverable>& entries, const vec<Lit>& lits)
  {
  for (int i = 0; i < lits.size(); ++i)
    {
    int index = var(lits[i]);
    // TODO:
    //std::cout << index << ": " << entries[index].id << std::endl;
    }
  }

static void dependency_clauses(const Hash& hash, const std::vector<Deliverable>& entries, Solver& solver)
  {
  for (std::size_t i = 0; i < entries.size(); ++i)
    {
    BOOST_FOREACH(const Spec& dependency, entries[i].depends)
      {
      vec<Lit> clause;
      clause.push(~Lit(i));
      query(hash, entries, dependency, clause);
      if (clause.size() == 1)
        {
        // TODO:
        //std::cout << "unsatisfied dependency: \n" << dependency << std::endl;
        std::cout << "required by: " << entries[i].folder << std::endl;
        solver.addUnit(clause[0]);
        }
      else
        {
        solver.addClause(clause);
        }
      }
    }
  }

static void explicit_conflict_clauses(const Hash& hash, const std::vector<Deliverable>& entries, Solver& solver)
  {
  for (std::size_t i = 0; i < entries.size(); ++i)
    {
    Lit lit = ~Lit(i);
    BOOST_FOREACH(const Spec& dependency, entries[i].conflicts)
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

// Deliverables of the same project either complement or conflict each other.
// Example: the component runtime(exe, dll) and develop(lib, hpp) complement
// each other when both have the same version and variant. If version and
// variant differ, the version does not match, or one deliverable provides
// all components, there is a conflict.
static void implicit_conflict_clauses(const std::vector<Deliverable>& entries, Solver& solver)
  {
  for (std::size_t i = 0; i < entries.size(); ++i)
    {
    for (std::size_t k = i + 1; k < entries.size(); ++k)
      {
      const Identification& id1 = entries[i].id;
      const Identification& id2 = entries[k].id;
      if (id1.project != id2.project || id1.domain != id2.domain)
        {
        break;
        }
      if (id1.version != id2.version ||
          id1.variant != id2.variant ||
          id1.component == id2.component ||
          id1.component == ASTERISK ||
          id2.component == ASTERISK)
        {
        solver.addBinary(~Lit(i), ~Lit(k));
        }
      }
    }
  }

std::vector<int> solve(
    const std::vector<Deliverable>& database,
    const std::set<Spec>& projects)
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

  vec<Lit> request;
  BOOST_FOREACH(const Spec& spec, projects)
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
