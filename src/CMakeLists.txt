#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

find_package(RE2C REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Boost "1.55" REQUIRED filesystem system)
include_directories(${Boost_INCLUDE_DIRS})

re2c_target(
  ${CMAKE_CURRENT_SOURCE_DIR}/query_re2c.in.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/query_re2c.hpp -s
  )

add_library(karrotlib STATIC
  ../include/karrot/dictionary.hpp
  ../include/karrot/implementation.hpp
  ../include/karrot/query.hpp
  ../include/karrot/solve.hpp
  ../include/karrot/spec.hpp
  ../include/karrot/string_pool.hpp
  minisat/solver.cpp
  minisat/solver.hpp
  minisat/solver_types.hpp
  hash.hpp
  implementation.cpp
  query_re2c.hpp
  query_re2c.in.hpp
  query.cpp
  solve.cpp
  string_pool.cpp
  vercmp.cpp
  vercmp.hpp
  )

source_group("Minisat Files" REGULAR_EXPRESSION "/minisat/")
