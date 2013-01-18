#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

find_package(RE2C REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DKARROT_SOURCE)

re2c_target(
  ${CMAKE_CURRENT_SOURCE_DIR}/query_re2c.in.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/query_re2c.hpp -s
  )

re2c_target(
  ${CMAKE_CURRENT_SOURCE_DIR}/xml_re2c.in.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/xml_re2c.hpp -s
  )

add_library(karrot STATIC
  minisat/Global.h
  minisat/Heap.h
  minisat/Solver.cpp
  minisat/Solver.h
  minisat/SolverTypes.h
  minisat/Sort.h
  minisat/VarOrder.h
  dependencies.hpp
  driver.cpp
  engine.cpp
  feed_cache.cpp
  feed_cache.hpp
  feed_parser.cpp
  feed_parser.hpp
  feed_queue.hpp
  hash.hpp
  package_handler.hpp
  quark_internal.hpp
  quark.cpp
  query_re2c.hpp
  query_re2c.in.hpp
  query.cpp
  query.hpp
  solve.cpp
  solve.hpp
  spec.cpp
  url.cpp
  variants.cpp
  variants.hpp
  xml_re2c.hpp
  xml_re2c.in.hpp
  xml_reader.cpp
  xml_reader.hpp
  )

source_group("Minisat Files" REGULAR_EXPRESSION "/minisat/")

target_link_libraries(karrot
  ${CURL_LIBRARIES}
  ${Boost_LIBRARIES}
  )