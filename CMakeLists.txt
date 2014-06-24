#=============================================================================
# Copyright (C) 2013-2014 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(Karrot VERSION 1.0.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${Karrot_SOURCE_DIR}/CMake")

enable_testing()
include_directories(include)

if(MSVC)
  add_definitions(
    -DBOOST_ALL_NO_LIB # no autolinking please
    -D_CRT_SECURE_NO_WARNINGS
    -D_SCL_SECURE_NO_WARNINGS
    )
endif()

if(NOT MSVC)
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
add_subdirectory(src)
add_subdirectory(test)
