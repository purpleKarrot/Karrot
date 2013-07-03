/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "../src/minisat/solver.hpp"
#include <iostream>

int sat(int argc, char* argv[])
  {
  int vars = 50;
  std::vector<Var> prefs(vars);
  for (int i = 0; i < vars; ++i)
    {
    prefs[i] = i;
    }

  Solver solver(std::move(prefs));

  solver.addTernary( Lit{15},  Lit{16},  Lit{29});
  solver.addTernary(~Lit{16},  Lit{21},  Lit{29});
  solver.addTernary(~Lit{16}, ~Lit{21},  Lit{29});
  solver.addTernary( Lit{15}, ~Lit{29},  Lit{46});
  solver.addTernary( Lit{15}, ~Lit{29}, ~Lit{46});
  solver.addTernary(~Lit{15}, ~Lit{20},  Lit{30});
  solver.addTernary(~Lit{15}, ~Lit{20}, ~Lit{30});
  solver.addTernary(~Lit{15},  Lit{20}, ~Lit{27});
  solver.addTernary(~Lit{12},  Lit{20},  Lit{27});
  solver.addTernary( Lit{12}, ~Lit{15},  Lit{17});
  solver.addTernary( Lit{12}, ~Lit{17}, ~Lit{37});
  solver.addTernary( Lit{12}, ~Lit{17}, ~Lit{30});
  solver.addTernary( Lit{30},  Lit{37},  Lit{43});
  solver.addTernary(~Lit{ 7},  Lit{30}, ~Lit{43});
  solver.addTernary( Lit{ 7}, ~Lit{11}, ~Lit{43});
  solver.addTernary( Lit{ 7},  Lit{11}, ~Lit{26});
  solver.addTernary( Lit{11},  Lit{26},  Lit{39});
  solver.addTernary(~Lit{ 3},  Lit{26}, ~Lit{39});
  solver.addTernary( Lit{11},  Lit{22}, ~Lit{39});
  solver.addTernary(~Lit{ 2},  Lit{ 3}, ~Lit{22});
  solver.addTernary( Lit{ 2}, ~Lit{22}, ~Lit{48});
  solver.addTernary( Lit{ 2}, ~Lit{12}, ~Lit{48});
  solver.addTernary(~Lit{22}, ~Lit{25},  Lit{48});
  solver.addTernary( Lit{11}, ~Lit{33},  Lit{48});
  solver.addTernary(~Lit{11},  Lit{25}, ~Lit{33});
  solver.addTernary( Lit{18},  Lit{33},  Lit{35});
  solver.addTernary(~Lit{18},  Lit{25},  Lit{35});
  solver.addTernary(~Lit{29},  Lit{33}, ~Lit{35});
  solver.addTernary( Lit{23},  Lit{33}, ~Lit{35});
  solver.addTernary(~Lit{23}, ~Lit{35},  Lit{42});
  solver.addTernary( Lit{ 5},  Lit{41}, ~Lit{42});
  solver.addTernary(~Lit{23},  Lit{41}, ~Lit{42});
  solver.addTernary(~Lit{ 4}, ~Lit{23}, ~Lit{41});
  solver.addTernary( Lit{ 4},  Lit{19}, ~Lit{41});
  solver.addTernary( Lit{ 4}, ~Lit{ 6}, ~Lit{19});
  solver.addTernary( Lit{ 3},  Lit{ 6},  Lit{ 9});
  solver.addTernary(~Lit{ 3},  Lit{ 9}, ~Lit{19});
  solver.addTernary( Lit{ 6}, ~Lit{ 9}, ~Lit{40});
  solver.addTernary(~Lit{ 9},  Lit{40},  Lit{45});
  solver.addTernary(~Lit{32},  Lit{40}, ~Lit{45});
  solver.addTernary( Lit{32}, ~Lit{36}, ~Lit{45});
  solver.addTernary( Lit{31},  Lit{32},  Lit{36});
  solver.addTernary( Lit{ 5}, ~Lit{31},  Lit{36});
  solver.addTernary(~Lit{ 5},  Lit{24}, ~Lit{31});
  solver.addTernary(~Lit{ 5}, ~Lit{24}, ~Lit{47});
  solver.addTernary(~Lit{ 8},  Lit{27},  Lit{47});
  solver.addTernary(~Lit{ 8}, ~Lit{24}, ~Lit{27});
  solver.addTernary( Lit{18}, ~Lit{24},  Lit{47});
  solver.addTernary( Lit{ 1},  Lit{ 8}, ~Lit{18});
  solver.addTernary(~Lit{ 1}, ~Lit{18},  Lit{34});
  solver.addTernary(~Lit{ 1},  Lit{21}, ~Lit{34});
  solver.addTernary(~Lit{21}, ~Lit{34},  Lit{49});
  solver.addTernary(~Lit{16}, ~Lit{34}, ~Lit{49});
  solver.addTernary(~Lit{28}, ~Lit{34}, ~Lit{49});
  solver.addTernary(~Lit{ 0},  Lit{28}, ~Lit{49});
  solver.addTernary( Lit{ 0},  Lit{10},  Lit{28});
  solver.addTernary(~Lit{10},  Lit{16}, ~Lit{44});
  solver.addTernary(~Lit{10},  Lit{38},  Lit{44});
  solver.addTernary(~Lit{25},  Lit{38},  Lit{44});
  solver.addTernary(~Lit{ 2}, ~Lit{25},  Lit{44});
  solver.addTernary(~Lit{10},  Lit{14}, ~Lit{38});
  solver.addTernary( Lit{13}, ~Lit{14}, ~Lit{38});
  solver.addTernary( Lit{13}, ~Lit{14}, ~Lit{44});
  solver.addTernary( Lit{13}, ~Lit{14}, ~Lit{26});
  solver.addTernary(~Lit{13}, ~Lit{14},  Lit{46});
  solver.addTernary( Lit{16},  Lit{16},  Lit{39});
  solver.addTernary( Lit{ 0}, ~Lit{28}, ~Lit{30});
  solver.addTernary(~Lit{ 6},  Lit{31},  Lit{37});
  solver.addTernary(~Lit{13}, ~Lit{32}, ~Lit{46});
  solver.addTernary(~Lit{ 0},  Lit{ 1}, ~Lit{ 7});
  solver.addTernary( Lit{34},  Lit{42},  Lit{43});
  solver.addTernary( Lit{20},  Lit{20},  Lit{23});
  solver.addTernary( Lit{19},  Lit{28}, ~Lit{47});
  solver.addTernary( Lit{22},  Lit{34}, ~Lit{36});
  solver.addTernary( Lit{ 1},  Lit{17}, ~Lit{32});
  solver.addTernary( Lit{14},  Lit{24}, ~Lit{44});
  solver.addTernary( Lit{ 8},  Lit{13}, ~Lit{37});
  solver.addTernary(~Lit{ 4},  Lit{10},  Lit{49});
  solver.addTernary(~Lit{ 2}, ~Lit{12},  Lit{45});
  solver.addTernary(~Lit{12}, ~Lit{40},  Lit{42});

  auto print = [](char const *string)
    {
    std::cout << string << std::endl;
    };

  return solver.solve(print) ? 0 : -1;
  }
