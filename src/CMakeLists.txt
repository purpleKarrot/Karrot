#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

find_package(RE2C REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Boost "1.46" REQUIRED filesystem system program_options)
include_directories(${Boost_INCLUDE_DIRS})

if(NOT WIN32)
  find_package(CURL REQUIRED)
  include_directories(${CURL_INCLUDE_DIRS})
endif()

re2c_target(
  ${CMAKE_CURRENT_SOURCE_DIR}/query_re2c.in.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/query_re2c.hpp -s
  )

re2c_target(
  ${CMAKE_CURRENT_SOURCE_DIR}/xml_re2c.in.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/xml_re2c.hpp -s
  )

add_library(karrotlib STATIC
  minisat/solver.cpp
  minisat/solver.hpp
  minisat/solver_types.hpp
  dependencies.cpp
  dependencies.hpp
  dictionary.hpp
  driver.cpp
  driver.hpp
  engine.cpp
  engine.hpp
  feed_parser.cpp
  feed_parser.hpp
  feed_queue.hpp
  graph.cpp
  graph.hpp
  hash.hpp
  implementation.cpp
  implementation.hpp
  package_handler.hpp
  package.hpp
  query_re2c.hpp
  query_re2c.in.hpp
  query.cpp
  query.hpp
  solve.cpp
  solve.hpp
  spec.hpp
  url.cpp
  url.hpp
  url_curl.cpp
  url_win32.cpp
  variants.cpp
  variants.hpp
  vercmp.cpp
  vercmp.hpp
  version.cpp
  xml_re2c.hpp
  xml_re2c.in.hpp
  xml_reader.cpp
  xml_reader.hpp
  )

source_group("Minisat Files" REGULAR_EXPRESSION "/minisat/")

if(WIN32)
  target_link_libraries(karrotlib PRIVATE winhttp)
else()
  target_link_libraries(karrotlib PRIVATE
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CURL_LIBRARIES}
    )
endif()

add_executable(karrot
  karrot.rc
  main.cpp
  )

target_link_libraries(karrot
  karrotlib
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  )
