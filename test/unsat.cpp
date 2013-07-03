/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "../src/minisat/solver.hpp"
#include <iostream>

int unsat(int argc, char* argv[])
  {
  int vars = 100;
  std::vector<Var> prefs(vars);
  for (int i = 0; i < vars; ++i)
    {
    prefs[i] = i;
    }

  Solver solver(std::move(prefs));

  solver.addTernary( Lit{15},  Lit{29},  Lit{94});
  solver.addTernary(~Lit{15},  Lit{29},  Lit{94});
  solver.addTernary(~Lit{29},  Lit{34},  Lit{77});
  solver.addTernary(~Lit{29}, ~Lit{77},  Lit{84});
  solver.addTernary(~Lit{77}, ~Lit{84},  Lit{94});
  solver.addTernary( Lit{ 7},  Lit{54},  Lit{99});
  solver.addTernary( Lit{ 7},  Lit{54}, ~Lit{94});
  solver.addTernary( Lit{ 8},  Lit{51},  Lit{99});
  solver.addTernary( Lit{ 8},  Lit{72}, ~Lit{99});
  solver.addTernary(~Lit{ 7}, ~Lit{ 8},  Lit{51});
  solver.addTernary( Lit{37},  Lit{65},  Lit{82});
  solver.addTernary(~Lit{37},  Lit{82},  Lit{86});
  solver.addTernary(~Lit{51},  Lit{82}, ~Lit{86});
  solver.addTernary( Lit{65},  Lit{73}, ~Lit{82});
  solver.addTernary(~Lit{51}, ~Lit{65},  Lit{88});
  solver.addTernary(~Lit{51},  Lit{72}, ~Lit{88});
  solver.addTernary(~Lit{51},  Lit{72}, ~Lit{73});
  solver.addTernary(~Lit{ 7}, ~Lit{72}, ~Lit{94});
  solver.addTernary( Lit{39}, ~Lit{54},  Lit{89});
  solver.addTernary(~Lit{39}, ~Lit{54},  Lit{89});
  solver.addTernary( Lit{24},  Lit{34},  Lit{81});
  solver.addTernary(~Lit{24},  Lit{81}, ~Lit{89});
  solver.addTernary(~Lit{54}, ~Lit{81}, ~Lit{89});
  solver.addTernary( Lit{10},  Lit{74},  Lit{83});
  solver.addTernary( Lit{10}, ~Lit{74},  Lit{95});
  solver.addTernary( Lit{22}, ~Lit{74}, ~Lit{95});
  solver.addTernary(~Lit{10},  Lit{22}, ~Lit{34});
  solver.addTernary(~Lit{22},  Lit{28},  Lit{64});
  solver.addTernary( Lit{28}, ~Lit{34}, ~Lit{64});
  solver.addTernary(~Lit{22}, ~Lit{28},  Lit{83});
  solver.addTernary(~Lit{34},  Lit{53},  Lit{69});
  solver.addTernary(~Lit{53},  Lit{69},  Lit{76});
  solver.addTernary( Lit{18}, ~Lit{76}, ~Lit{83});
  solver.addTernary(~Lit{18}, ~Lit{53},  Lit{69});
  solver.addTernary( Lit{21},  Lit{67},  Lit{80});
  solver.addTernary(~Lit{21},  Lit{47},  Lit{80});
  solver.addTernary(~Lit{21}, ~Lit{47},  Lit{92});
  solver.addTernary( Lit{ 2}, ~Lit{47}, ~Lit{92});
  solver.addTernary( Lit{ 6},  Lit{17}, ~Lit{80});
  solver.addTernary(~Lit{ 6},  Lit{55}, ~Lit{80});
  solver.addTernary( Lit{ 2},  Lit{17}, ~Lit{55});
  solver.addTernary(~Lit{17},  Lit{46},  Lit{67});
  solver.addTernary(~Lit{17}, ~Lit{46}, ~Lit{80});
  solver.addTernary(~Lit{ 2},  Lit{67},  Lit{76});
  solver.addTernary(~Lit{ 2}, ~Lit{76}, ~Lit{83});
  solver.addTernary( Lit{18}, ~Lit{67}, ~Lit{69});
  solver.addTernary(~Lit{18}, ~Lit{67},  Lit{73});
  solver.addTernary(~Lit{67}, ~Lit{69}, ~Lit{73});
  solver.addTernary( Lit{53},  Lit{60}, ~Lit{61});
  solver.addTernary( Lit{49},  Lit{52}, ~Lit{61});
  solver.addTernary(~Lit{49},  Lit{60}, ~Lit{61});
  solver.addTernary(~Lit{26},  Lit{55},  Lit{92});
  solver.addTernary( Lit{ 3},  Lit{13},  Lit{75});
  solver.addTernary( Lit{ 3}, ~Lit{75},  Lit{95});
  solver.addTernary(~Lit{ 3},  Lit{13},  Lit{79});
  solver.addTernary(~Lit{13}, ~Lit{67},  Lit{79});
  solver.addTernary(~Lit{ 9}, ~Lit{38}, ~Lit{88});
  solver.addTernary( Lit{ 0},  Lit{48}, ~Lit{80});
  solver.addTernary( Lit{ 0},  Lit{25}, ~Lit{48});
  solver.addTernary( Lit{16}, ~Lit{25}, ~Lit{48});
  solver.addTernary(~Lit{ 0},  Lit{16}, ~Lit{39});
  solver.addTernary( Lit{15},  Lit{50}, ~Lit{88});
  solver.addTernary(~Lit{ 8},  Lit{56},  Lit{59});
  solver.addTernary( Lit{11},  Lit{44}, ~Lit{50});
  solver.addTernary( Lit{ 1},  Lit{11},  Lit{68});
  solver.addTernary( Lit{ 1}, ~Lit{11},  Lit{39});
  solver.addTernary(~Lit{11}, ~Lit{50},  Lit{68});
  solver.addTernary(~Lit{32},  Lit{59}, ~Lit{97});
  solver.addTernary( Lit{ 4}, ~Lit{31}, ~Lit{65});
  solver.addTernary( Lit{ 1}, ~Lit{46}, ~Lit{99});
  solver.addTernary(~Lit{41},  Lit{63},  Lit{82});
  solver.addTernary( Lit{19}, ~Lit{41}, ~Lit{63});
  solver.addTernary( Lit{19}, ~Lit{47},  Lit{97});
  solver.addTernary(~Lit{19},  Lit{49},  Lit{97});
  solver.addTernary(~Lit{31}, ~Lit{49},  Lit{97});
  solver.addTernary(~Lit{23},  Lit{36}, ~Lit{72});
  solver.addTernary(~Lit{23}, ~Lit{36}, ~Lit{99});
  solver.addTernary(~Lit{56},  Lit{70},  Lit{80});
  solver.addTernary(~Lit{36},  Lit{39}, ~Lit{90});
  solver.addTernary( Lit{30},  Lit{41},  Lit{80});
  solver.addTernary(~Lit{30},  Lit{41},  Lit{71});
  solver.addTernary(~Lit{30},  Lit{41}, ~Lit{71});
  solver.addTernary( Lit{ 6}, ~Lit{18},  Lit{24});
  solver.addTernary(~Lit{ 0}, ~Lit{24}, ~Lit{93});
  solver.addTernary(~Lit{14}, ~Lit{43},  Lit{78});
  solver.addTernary(~Lit{ 5},  Lit{30},  Lit{45});
  solver.addTernary(~Lit{38},  Lit{40},  Lit{87});
  solver.addTernary( Lit{27}, ~Lit{38},  Lit{42});
  solver.addTernary( Lit{27}, ~Lit{42}, ~Lit{87});
  solver.addTernary(~Lit{ 3}, ~Lit{27}, ~Lit{87});
  solver.addTernary(~Lit{29}, ~Lit{38}, ~Lit{40});
  solver.addTernary(~Lit{28},  Lit{32},  Lit{87});
  solver.addTernary(~Lit{15},  Lit{20},  Lit{93});
  solver.addTernary(~Lit{9},  Lit{25},  Lit{61});
  solver.addTernary(~Lit{10}, ~Lit{63},  Lit{85});
  solver.addTernary(~Lit{ 5}, ~Lit{40},  Lit{75});
  solver.addTernary( Lit{37}, ~Lit{45},  Lit{92});
  solver.addTernary( Lit{25}, ~Lit{36},  Lit{93});
  solver.addTernary(~Lit{25},  Lit{52}, ~Lit{78});
  solver.addTernary( Lit{77},  Lit{86}, ~Lit{93});
  solver.addTernary( Lit{64},  Lit{75}, ~Lit{86});
  solver.addTernary( Lit{22},  Lit{50}, ~Lit{61});
  solver.addTernary(~Lit{10}, ~Lit{35},  Lit{56});
  solver.addTernary( Lit{40},  Lit{58}, ~Lit{64});
  solver.addTernary(~Lit{55},  Lit{71}, ~Lit{90});
  solver.addTernary( Lit{12}, ~Lit{19}, ~Lit{45});
  solver.addTernary(~Lit{12},  Lit{14},  Lit{78});
  solver.addTernary(~Lit{16},  Lit{46}, ~Lit{59});
  solver.addTernary(~Lit{12}, ~Lit{43},  Lit{98});
  solver.addTernary(~Lit{ 6}, ~Lit{37},  Lit{66});
  solver.addTernary( Lit{36}, ~Lit{48},  Lit{61});
  solver.addTernary(~Lit{13}, ~Lit{16}, ~Lit{78});
  solver.addTernary(~Lit{12}, ~Lit{14}, ~Lit{21});
  solver.addTernary( Lit{31}, ~Lit{32}, ~Lit{33});
  solver.addTernary( Lit{23},  Lit{44},  Lit{47});
  solver.addTernary( Lit{20},  Lit{23}, ~Lit{47});
  solver.addTernary(~Lit{35},  Lit{63}, ~Lit{84});
  solver.addTernary( Lit{ 9}, ~Lit{60},  Lit{66});
  solver.addTernary(~Lit{ 4},  Lit{43},  Lit{58});
  solver.addTernary(~Lit{79}, ~Lit{84}, ~Lit{98});
  solver.addTernary( Lit{ 5},  Lit{36}, ~Lit{96});
  solver.addTernary(~Lit{20}, ~Lit{33},  Lit{63});
  solver.addTernary(~Lit{ 4},  Lit{43},  Lit{45});
  solver.addTernary( Lit{57}, ~Lit{75},  Lit{96});
  solver.addTernary(~Lit{20}, ~Lit{35},  Lit{74});
  solver.addTernary(~Lit{14},  Lit{57}, ~Lit{58});
  solver.addTernary(~Lit{57}, ~Lit{75}, ~Lit{98});
  solver.addTernary(~Lit{ 1},  Lit{14},  Lit{32});
  solver.addTernary(~Lit{25},  Lit{33}, ~Lit{56});
  solver.addTernary(~Lit{17}, ~Lit{81}, ~Lit{91});
  solver.addTernary( Lit{26}, ~Lit{79}, ~Lit{96});
  solver.addTernary( Lit{ 5},  Lit{31},  Lit{62});
  solver.addTernary(~Lit{33}, ~Lit{85},  Lit{91});
  solver.addTernary( Lit{12}, ~Lit{60},  Lit{96});
  solver.addTernary(~Lit{27},  Lit{42}, ~Lit{97});
  solver.addTernary( Lit{ 4},  Lit{38}, ~Lit{85});
  solver.addTernary( Lit{38}, ~Lit{44},  Lit{91});
  solver.addTernary( Lit{26}, ~Lit{42},  Lit{96});
  solver.addTernary( Lit{12}, ~Lit{57}, ~Lit{85});
  solver.addTernary(~Lit{27}, ~Lit{66}, ~Lit{92});
  solver.addTernary(~Lit{68},  Lit{84},  Lit{98});
  solver.addTernary( Lit{41},  Lit{70}, ~Lit{71});
  solver.addTernary( Lit{ 9}, ~Lit{26}, ~Lit{62});
  solver.addTernary(~Lit{58},  Lit{62}, ~Lit{82});
  solver.addTernary( Lit{35},  Lit{85}, ~Lit{95});
  solver.addTernary(~Lit{ 1},  Lit{35},  Lit{74});
  solver.addTernary(~Lit{58}, ~Lit{70},  Lit{88});
  solver.addTernary( Lit{35}, ~Lit{66},  Lit{90});
  solver.addTernary( Lit{35}, ~Lit{59},  Lit{62});
  solver.addTernary(~Lit{62},  Lit{90}, ~Lit{92});
  solver.addTernary( Lit{24},  Lit{86},  Lit{91});
  solver.addTernary(~Lit{20},  Lit{48}, ~Lit{70});
  solver.addTernary(~Lit{ 1},  Lit{ 9},  Lit{21});
  solver.addTernary( Lit{ 5}, ~Lit{17},  Lit{40});
  solver.addTernary( Lit{ 5},  Lit{70}, ~Lit{91});
  solver.addTernary(~Lit{52}, ~Lit{68}, ~Lit{70});
  solver.addTernary(~Lit{ 1}, ~Lit{52}, ~Lit{57});
  solver.addTernary( Lit{42}, ~Lit{44}, ~Lit{95});
  solver.addTernary( Lit{33}, ~Lit{44}, ~Lit{68});
  solver.addTernary( Lit{62}, ~Lit{85}, ~Lit{97});

  auto print = [](char const *string)
    {
    std::cout << string << std::endl;
    };

  return solver.solve(print) ? -1 : 0;
  }
