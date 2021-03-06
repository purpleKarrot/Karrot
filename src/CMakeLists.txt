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

if(NOT WIN32)
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-fvisibility=hidden VISIBILITY_HIDDEN)
  check_cxx_compiler_flag(-fvisibility-inlines-hidden VISIBILITY_INLINES_HIDDEN)
  if(VISIBILITY_HIDDEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
  endif()
  if(VISIBILITY_INLINES_HIDDEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
  endif()
endif()

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

re2c_target(
  ${CMAKE_CURRENT_SOURCE_DIR}/xml_re2c.in.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/xml_re2c.hpp -s
  )

add_library(karrot
  minisat/Global.h
  minisat/Solver.cpp
  minisat/Solver.h
  minisat/SolverTypes.h
  minisat/VarOrder.h
  database.hpp
  dependencies.cpp
  dependencies.hpp
  dictionary.cpp
  dictionary.hpp
  driver.hpp
  engine.cpp
  error.cpp
  error.hpp
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
  quark.cpp
  quark.hpp
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
  karrot.rc
  )

source_group("Minisat Files" REGULAR_EXPRESSION "/minisat/")

set_property(TARGET karrot APPEND PROPERTY
  COMPILE_DEFINITIONS "KARROT_BUILD=1"
  )

if(NOT BUILD_SHARED_LIBS)
  set_property(TARGET karrot APPEND PROPERTY
    COMPILE_DEFINITIONS "KARROT_STATIC=1"
    )
endif()

if(WIN32)
  if(BUILD_SHARED_LIBS)
    set_target_properties(karrot PROPERTIES
      IMPORT_PREFIX ""
      PREFIX ""
      )
  else()
    set_target_properties(karrot PROPERTIES
      PREFIX "lib"
      )
  endif()
endif()

if(WIN32)
  target_link_libraries(karrot LINK_PRIVATE
    shlwapi
    )
else()
  target_link_libraries(karrot LINK_PRIVATE
    ${Boost_LIBRARIES}
    ${CURL_LIBRARIES}
    )
endif()

install(TARGETS karrot EXPORT karrot_export
  ARCHIVE
    DESTINATION lib
    COMPONENT   dev
    #CONFIGURATIONS "Release"
  LIBRARY
    DESTINATION lib
    COMPONENT   bin
    #CONFIGURATIONS "Release"
  RUNTIME
    DESTINATION bin
    COMPONENT   bin
    #CONFIGURATIONS "Release"
  )

install(EXPORT karrot_export
  DESTINATION cmake
  FILE KarrotTargets.cmake
  )

set(cfg_file "${CMAKE_CURRENT_BINARY_DIR}/KarrotConfig.cmake")
set(ver_file "${CMAKE_CURRENT_BINARY_DIR}/KarrotConfigVersion.cmake")

file(WRITE "${cfg_file}" "
GET_FILENAME_COMPONENT(_DIR \"\${CMAKE_CURRENT_LIST_FILE}\" PATH)
INCLUDE(\"\${_DIR}/KarrotTargets.cmake\")
SET(Karrot_INCLUDE_DIRS \"\${_DIR}/include\")
SET(Karrot_LIBRARIES karrot)
")

install(FILES "${cfg_file}"
  COMPONENT dev
  DESTINATION cmake
  )

if(NOT CMAKE_VERSION VERSION_LESS "2.8.8")
  include(CMakePackageConfigHelpers)
  write_basic_package_version_file("${ver_file}"
    VERSION "0.1.0"
    COMPATIBILITY SameMajorVersion
    )
  install(FILES "${ver_file}"
    COMPONENT dev
    DESTINATION cmake
    )
endif()

install(FILES "${Karrot_SOURCE_DIR}/include/karrot.h"
  COMPONENT dev
  DESTINATION include
  )
