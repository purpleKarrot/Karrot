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

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost "1.46" REQUIRED COMPONENTS
  filesystem
  system
  )

add_definitions(-DBOOST_ALL_NO_LIB)

include_directories(
  ${Boost_INCLUDE_DIRS}
  include
  )

add_subdirectory(src)
add_subdirectory(test)
