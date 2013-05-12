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

if(CMAKE_CROSSCOMPILING AND CMAKE_HOST_UNIX)
  set(Boost_INCLUDE_DIR "${CMAKE_BINARY_DIR}/boost_root")
  if(NOT EXISTS "${Boost_INCLUDE_DIR}/boost")
    file(MAKE_DIRECTORY "${Boost_INCLUDE_DIR}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
      "/usr/include/boost"
      "${Boost_INCLUDE_DIR}/boost"
      )
  endif()
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

add_subdirectory(doc)
add_subdirectory(example)
add_subdirectory(src)
add_subdirectory(test)
