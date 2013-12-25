#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

find_package(RE2C REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_DEBUG_POSTFIX "_d")

if(WIN32)
  find_package(Boost "1.46" REQUIRED)
else()
  find_package(Boost "1.46" REQUIRED filesystem system)
endif()
include_directories(${Boost_INCLUDE_DIRS})

if(NOT WIN32)
  find_package(CURL REQUIRED)
  include_directories(${CURL_INCLUDE_DIRS})
endif()

re2c_target(
  ${CMAKE_CURRENT_SOURCE_DIR}/query_re2c.in.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/query_re2c.hpp -s
  )

add_library(karrot STATIC
  ../include/karrot/dictionary.hpp
  ../include/karrot/driver.hpp
  ../include/karrot/string.hpp
  ../include/karrot/implementation.hpp
  minisat/solver.cpp
  minisat/solver.hpp
  minisat/solver_types.hpp
  dependencies.cpp
  dependencies.hpp
  engine.cpp
  feed_parser.cpp
  feed_parser.hpp
  feed_queue.hpp
  hash.hpp
  implementation.cpp
  package.hpp
  query_re2c.hpp
  query_re2c.in.hpp
  query.cpp
  query.hpp
  solve.cpp
  solve.hpp
  spec.hpp
  types.hpp
  url.cpp
  url.hpp
  url_curl.cpp
  url_win32.cpp
  variants.cpp
  variants.hpp
  vercmp.cpp
  vercmp.hpp
  )

source_group("Minisat Files" REGULAR_EXPRESSION "/minisat/")

target_include_directories(karrot PUBLIC ../include)

target_link_libraries(karrot PRIVATE
  xml
  )

if(WIN32)
  target_link_libraries(karrot PRIVATE
    shlwapi
    )
else()
  target_link_libraries(karrot PRIVATE
    ${Boost_LIBRARIES}
    ${CURL_LIBRARIES}
    )
endif()
