/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "../src/minisat/Solver.h"
#include <iostream>

int unsat(int argc, char* argv[])
  {
  int vars = 100;
  std::vector<Var> prefs(vars);
  for (int i = 0; i < vars; ++i)
    {
    prefs[i] = i + 1;
    }

  Solver solver(std::move(prefs));
  for (int i = 0; i < vars; ++i)
    {
    solver.newVar();
    }

  solver.addTernary( Lit{ 16},  Lit{ 30},  Lit{ 95});
  solver.addTernary(~Lit{ 16},  Lit{ 30},  Lit{ 95});
  solver.addTernary(~Lit{ 30},  Lit{ 35},  Lit{ 78});
  solver.addTernary(~Lit{ 30}, ~Lit{ 78},  Lit{ 85});
  solver.addTernary(~Lit{ 78}, ~Lit{ 85},  Lit{ 95});
  solver.addTernary( Lit{  8},  Lit{ 55},  Lit{100});
  solver.addTernary( Lit{  8},  Lit{ 55}, ~Lit{ 95});
  solver.addTernary( Lit{  9},  Lit{ 52},  Lit{100});
  solver.addTernary( Lit{  9},  Lit{ 73}, ~Lit{100});
  solver.addTernary(~Lit{  8}, ~Lit{  9},  Lit{ 52});
  solver.addTernary( Lit{ 38},  Lit{ 66},  Lit{ 83});
  solver.addTernary(~Lit{ 38},  Lit{ 83},  Lit{ 87});
  solver.addTernary(~Lit{ 52},  Lit{ 83}, ~Lit{ 87});
  solver.addTernary( Lit{ 66},  Lit{ 74}, ~Lit{ 83});
  solver.addTernary(~Lit{ 52}, ~Lit{ 66},  Lit{ 89});
  solver.addTernary(~Lit{ 52},  Lit{ 73}, ~Lit{ 89});
  solver.addTernary(~Lit{ 52},  Lit{ 73}, ~Lit{ 74});
  solver.addTernary(~Lit{  8}, ~Lit{ 73}, ~Lit{ 95});
  solver.addTernary( Lit{ 40}, ~Lit{ 55},  Lit{ 90});
  solver.addTernary(~Lit{ 40}, ~Lit{ 55},  Lit{ 90});
  solver.addTernary( Lit{ 25},  Lit{ 35},  Lit{ 82});
  solver.addTernary(~Lit{ 25},  Lit{ 82}, ~Lit{ 90});
  solver.addTernary(~Lit{ 55}, ~Lit{ 82}, ~Lit{ 90});
  solver.addTernary( Lit{ 11},  Lit{ 75},  Lit{ 84});
  solver.addTernary( Lit{ 11}, ~Lit{ 75},  Lit{ 96});
  solver.addTernary( Lit{ 23}, ~Lit{ 75}, ~Lit{ 96});
  solver.addTernary(~Lit{ 11},  Lit{ 23}, ~Lit{ 35});
  solver.addTernary(~Lit{ 23},  Lit{ 29},  Lit{ 65});
  solver.addTernary( Lit{ 29}, ~Lit{ 35}, ~Lit{ 65});
  solver.addTernary(~Lit{ 23}, ~Lit{ 29},  Lit{ 84});
  solver.addTernary(~Lit{ 35},  Lit{ 54},  Lit{ 70});
  solver.addTernary(~Lit{ 54},  Lit{ 70},  Lit{ 77});
  solver.addTernary( Lit{ 19}, ~Lit{ 77}, ~Lit{ 84});
  solver.addTernary(~Lit{ 19}, ~Lit{ 54},  Lit{ 70});
  solver.addTernary( Lit{ 22},  Lit{ 68},  Lit{ 81});
  solver.addTernary(~Lit{ 22},  Lit{ 48},  Lit{ 81});
  solver.addTernary(~Lit{ 22}, ~Lit{ 48},  Lit{ 93});
  solver.addTernary( Lit{  3}, ~Lit{ 48}, ~Lit{ 93});
  solver.addTernary( Lit{  7},  Lit{ 18}, ~Lit{ 81});
  solver.addTernary(~Lit{  7},  Lit{ 56}, ~Lit{ 81});
  solver.addTernary( Lit{  3},  Lit{ 18}, ~Lit{ 56});
  solver.addTernary(~Lit{ 18},  Lit{ 47},  Lit{ 68});
  solver.addTernary(~Lit{ 18}, ~Lit{ 47}, ~Lit{ 81});
  solver.addTernary(~Lit{  3},  Lit{ 68},  Lit{ 77});
  solver.addTernary(~Lit{  3}, ~Lit{ 77}, ~Lit{ 84});
  solver.addTernary( Lit{ 19}, ~Lit{ 68}, ~Lit{ 70});
  solver.addTernary(~Lit{ 19}, ~Lit{ 68},  Lit{ 74});
  solver.addTernary(~Lit{ 68}, ~Lit{ 70}, ~Lit{ 74});
  solver.addTernary( Lit{ 54},  Lit{ 61}, ~Lit{ 62});
  solver.addTernary( Lit{ 50},  Lit{ 53}, ~Lit{ 62});
  solver.addTernary(~Lit{ 50},  Lit{ 61}, ~Lit{ 62});
  solver.addTernary(~Lit{ 27},  Lit{ 56},  Lit{ 93});
  solver.addTernary( Lit{  4},  Lit{ 14},  Lit{ 76});
  solver.addTernary( Lit{  4}, ~Lit{ 76},  Lit{ 96});
  solver.addTernary(~Lit{  4},  Lit{ 14},  Lit{ 80});
  solver.addTernary(~Lit{ 14}, ~Lit{ 68},  Lit{ 80});
  solver.addTernary(~Lit{ 10}, ~Lit{ 39}, ~Lit{ 89});
  solver.addTernary( Lit{  1},  Lit{ 49}, ~Lit{ 81});
  solver.addTernary( Lit{  1},  Lit{ 26}, ~Lit{ 49});
  solver.addTernary( Lit{ 17}, ~Lit{ 26}, ~Lit{ 49});
  solver.addTernary(~Lit{  1},  Lit{ 17}, ~Lit{ 40});
  solver.addTernary( Lit{ 16},  Lit{ 51}, ~Lit{ 89});
  solver.addTernary(~Lit{  9},  Lit{ 57},  Lit{ 60});
  solver.addTernary( Lit{ 12},  Lit{ 45}, ~Lit{ 51});
  solver.addTernary( Lit{  2},  Lit{ 12},  Lit{ 69});
  solver.addTernary( Lit{  2}, ~Lit{ 12},  Lit{ 40});
  solver.addTernary(~Lit{ 12}, ~Lit{ 51},  Lit{ 69});
  solver.addTernary(~Lit{ 33},  Lit{ 60}, ~Lit{ 98});
  solver.addTernary( Lit{  5}, ~Lit{ 32}, ~Lit{ 66});
  solver.addTernary( Lit{  2}, ~Lit{ 47}, ~Lit{100});
  solver.addTernary(~Lit{ 42},  Lit{ 64},  Lit{ 83});
  solver.addTernary( Lit{ 20}, ~Lit{ 42}, ~Lit{ 64});
  solver.addTernary( Lit{ 20}, ~Lit{ 48},  Lit{ 98});
  solver.addTernary(~Lit{ 20},  Lit{ 50},  Lit{ 98});
  solver.addTernary(~Lit{ 32}, ~Lit{ 50},  Lit{ 98});
  solver.addTernary(~Lit{ 24},  Lit{ 37}, ~Lit{ 73});
  solver.addTernary(~Lit{ 24}, ~Lit{ 37}, ~Lit{100});
  solver.addTernary(~Lit{ 57},  Lit{ 71},  Lit{ 81});
  solver.addTernary(~Lit{ 37},  Lit{ 40}, ~Lit{ 91});
  solver.addTernary( Lit{ 31},  Lit{ 42},  Lit{ 81});
  solver.addTernary(~Lit{ 31},  Lit{ 42},  Lit{ 72});
  solver.addTernary(~Lit{ 31},  Lit{ 42}, ~Lit{ 72});
  solver.addTernary( Lit{  7}, ~Lit{ 19},  Lit{ 25});
  solver.addTernary(~Lit{  1}, ~Lit{ 25}, ~Lit{ 94});
  solver.addTernary(~Lit{ 15}, ~Lit{ 44},  Lit{ 79});
  solver.addTernary(~Lit{  6},  Lit{ 31},  Lit{ 46});
  solver.addTernary(~Lit{ 39},  Lit{ 41},  Lit{ 88});
  solver.addTernary( Lit{ 28}, ~Lit{ 39},  Lit{ 43});
  solver.addTernary( Lit{ 28}, ~Lit{ 43}, ~Lit{ 88});
  solver.addTernary(~Lit{  4}, ~Lit{ 28}, ~Lit{ 88});
  solver.addTernary(~Lit{ 30}, ~Lit{ 39}, ~Lit{ 41});
  solver.addTernary(~Lit{ 29},  Lit{ 33},  Lit{ 88});
  solver.addTernary(~Lit{ 16},  Lit{ 21},  Lit{ 94});
  solver.addTernary(~Lit{ 10},  Lit{ 26},  Lit{ 62});
  solver.addTernary(~Lit{ 11}, ~Lit{ 64},  Lit{ 86});
  solver.addTernary(~Lit{  6}, ~Lit{ 41},  Lit{ 76});
  solver.addTernary( Lit{ 38}, ~Lit{ 46},  Lit{ 93});
  solver.addTernary( Lit{ 26}, ~Lit{ 37},  Lit{ 94});
  solver.addTernary(~Lit{ 26},  Lit{ 53}, ~Lit{ 79});
  solver.addTernary( Lit{ 78},  Lit{ 87}, ~Lit{ 94});
  solver.addTernary( Lit{ 65},  Lit{ 76}, ~Lit{ 87});
  solver.addTernary( Lit{ 23},  Lit{ 51}, ~Lit{ 62});
  solver.addTernary(~Lit{ 11}, ~Lit{ 36},  Lit{ 57});
  solver.addTernary( Lit{ 41},  Lit{ 59}, ~Lit{ 65});
  solver.addTernary(~Lit{ 56},  Lit{ 72}, ~Lit{ 91});
  solver.addTernary( Lit{ 13}, ~Lit{ 20}, ~Lit{ 46});
  solver.addTernary(~Lit{ 13},  Lit{ 15},  Lit{ 79});
  solver.addTernary(~Lit{ 17},  Lit{ 47}, ~Lit{ 60});
  solver.addTernary(~Lit{ 13}, ~Lit{ 44},  Lit{ 99});
  solver.addTernary(~Lit{  7}, ~Lit{ 38},  Lit{ 67});
  solver.addTernary( Lit{ 37}, ~Lit{ 49},  Lit{ 62});
  solver.addTernary(~Lit{ 14}, ~Lit{ 17}, ~Lit{ 79});
  solver.addTernary(~Lit{ 13}, ~Lit{ 15}, ~Lit{ 22});
  solver.addTernary( Lit{ 32}, ~Lit{ 33}, ~Lit{ 34});
  solver.addTernary( Lit{ 24},  Lit{ 45},  Lit{ 48});
  solver.addTernary( Lit{ 21},  Lit{ 24}, ~Lit{ 48});
  solver.addTernary(~Lit{ 36},  Lit{ 64}, ~Lit{ 85});
  solver.addTernary( Lit{ 10}, ~Lit{ 61},  Lit{ 67});
  solver.addTernary(~Lit{  5},  Lit{ 44},  Lit{ 59});
  solver.addTernary(~Lit{ 80}, ~Lit{ 85}, ~Lit{ 99});
  solver.addTernary( Lit{  6},  Lit{ 37}, ~Lit{ 97});
  solver.addTernary(~Lit{ 21}, ~Lit{ 34},  Lit{ 64});
  solver.addTernary(~Lit{  5},  Lit{ 44},  Lit{ 46});
  solver.addTernary( Lit{ 58}, ~Lit{ 76},  Lit{ 97});
  solver.addTernary(~Lit{ 21}, ~Lit{ 36},  Lit{ 75});
  solver.addTernary(~Lit{ 15},  Lit{ 58}, ~Lit{ 59});
  solver.addTernary(~Lit{ 58}, ~Lit{ 76}, ~Lit{ 99});
  solver.addTernary(~Lit{  2},  Lit{ 15},  Lit{ 33});
  solver.addTernary(~Lit{ 26},  Lit{ 34}, ~Lit{ 57});
  solver.addTernary(~Lit{ 18}, ~Lit{ 82}, ~Lit{ 92});
  solver.addTernary( Lit{ 27}, ~Lit{ 80}, ~Lit{ 97});
  solver.addTernary( Lit{  6},  Lit{ 32},  Lit{ 63});
  solver.addTernary(~Lit{ 34}, ~Lit{ 86},  Lit{ 92});
  solver.addTernary( Lit{ 13}, ~Lit{ 61},  Lit{ 97});
  solver.addTernary(~Lit{ 28},  Lit{ 43}, ~Lit{ 98});
  solver.addTernary( Lit{  5},  Lit{ 39}, ~Lit{ 86});
  solver.addTernary( Lit{ 39}, ~Lit{ 45},  Lit{ 92});
  solver.addTernary( Lit{ 27}, ~Lit{ 43},  Lit{ 97});
  solver.addTernary( Lit{ 13}, ~Lit{ 58}, ~Lit{ 86});
  solver.addTernary(~Lit{ 28}, ~Lit{ 67}, ~Lit{ 93});
  solver.addTernary(~Lit{ 69},  Lit{ 85},  Lit{ 99});
  solver.addTernary( Lit{ 42},  Lit{ 71}, ~Lit{ 72});
  solver.addTernary( Lit{ 10}, ~Lit{ 27}, ~Lit{ 63});
  solver.addTernary(~Lit{ 59},  Lit{ 63}, ~Lit{ 83});
  solver.addTernary( Lit{ 36},  Lit{ 86}, ~Lit{ 96});
  solver.addTernary(~Lit{  2},  Lit{ 36},  Lit{ 75});
  solver.addTernary(~Lit{ 59}, ~Lit{ 71},  Lit{ 89});
  solver.addTernary( Lit{ 36}, ~Lit{ 67},  Lit{ 91});
  solver.addTernary( Lit{ 36}, ~Lit{ 60},  Lit{ 63});
  solver.addTernary(~Lit{ 63},  Lit{ 91}, ~Lit{ 93});
  solver.addTernary( Lit{ 25},  Lit{ 87},  Lit{ 92});
  solver.addTernary(~Lit{ 21},  Lit{ 49}, ~Lit{ 71});
  solver.addTernary(~Lit{  2},  Lit{ 10},  Lit{ 22});
  solver.addTernary( Lit{  6}, ~Lit{ 18},  Lit{ 41});
  solver.addTernary( Lit{  6},  Lit{ 71}, ~Lit{ 92});
  solver.addTernary(~Lit{ 53}, ~Lit{ 69}, ~Lit{ 71});
  solver.addTernary(~Lit{  2}, ~Lit{ 53}, ~Lit{ 58});
  solver.addTernary( Lit{ 43}, ~Lit{ 45}, ~Lit{ 96});
  solver.addTernary( Lit{ 34}, ~Lit{ 45}, ~Lit{ 69});
  solver.addTernary( Lit{ 63}, ~Lit{ 86}, ~Lit{ 98});

  auto print = [](char const *string)
    {
    std::cout << string << std::endl;
    };

  return solver.solve(print) ? -1 : 0;
  }
