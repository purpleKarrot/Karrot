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

if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
  set(CMAKE_CXX_LINK_FLAGS "-std=c++11 -stdlib=libc++")
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

find_package(CURL REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost "1.46" REQUIRED COMPONENTS
  filesystem
  system
  )

add_definitions(-DBOOST_ALL_NO_LIB)

include_directories(
  ${CURL_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  include
  )

add_subdirectory(src)
add_subdirectory(test)
