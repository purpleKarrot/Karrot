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
include_directories(include)

option(BUILD_SHARED_LIBS "build shared libraries" ON)
if(BUILD_SHARED_LIBS)
  add_definitions(-DKARROT_DYN_LINK)
endif()

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
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-std=c++11 HAVE_STD11)
  if(HAVE_STD11)
    set(CMAKE_CXX_FLAGS -std=c++11)
  else()
    check_cxx_compiler_flag(-std=c++0x HAVE_STD0X)
    if(HAVE_STD0X)
      set(CMAKE_CXX_FLAGS -std=c++0x)
    endif()
  endif()
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

option(BUILD_VISIBILITY_HIDDEN "build with gcov support" ON)
if(BUILD_VISIBILITY_HIDDEN AND NOT WIN32)
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-fvisibility=hidden VISIBILITY_HIDDEN)
  if(VISIBILITY_HIDDEN)
    set(CMAKE_CXX_FLAGS_RELEASE
      "${CMAKE_CXX_FLAGS_RELEASE} -fvisibility=hidden"
      )
  endif()
  check_cxx_compiler_flag(-fvisibility-inlines-hidden VISIBILITY_INLINES_HIDDEN)
  if(VISIBILITY_INLINES_HIDDEN)
    set(CMAKE_CXX_FLAGS_RELEASE
      "${CMAKE_CXX_FLAGS_RELEASE} -fvisibility-inlines-hidden"
      )
  endif()
endif()

add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(test)
