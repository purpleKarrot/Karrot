#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)
project(Karrot CXX)

list(APPEND CMAKE_MODULE_PATH "${Karrot_SOURCE_DIR}/CMake")

enable_testing()

if(MSVC)
  add_definitions(
    -DBOOST_ALL_NO_LIB # no autolinking please
    -D_CRT_SECURE_NO_WARNINGS
    -D_SCL_SECURE_NO_WARNINGS
    )
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libstdc++")
  set(CMAKE_CXX_LINK_FLAGS "-std=c++11 -stdlib=libstdc++")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
  set(CMAKE_CXX_FLAGS "-std=c++11")
endif()

option(BUILD_COVERAGE "build with gcov support" OFF)
if(BUILD_COVERAGE AND ${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
  set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage"
    )
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG
    "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage"
    )
endif()

add_subdirectory(doc)
add_subdirectory(libs/karrot)
add_subdirectory(libs/xml)
add_subdirectory(tools)

configure_file(coveralls.in coveralls @ONLY)
