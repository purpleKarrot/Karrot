#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

option(KARROT_USE_GIT        "Build with Git support"        OFF)
option(KARROT_USE_PACKAGEKIT "Build with PackageKit support" OFF)

set(sources)
set(libraries)

if(KARROT_USE_GIT)
  find_package(LibGit2 REQUIRED)
  list(APPEND sources
    git.cpp
    git.hpp
    )
  list(APPEND libraries
    ${LibGit2_LIBRARY}
    )
  include_directories(${LibGit2_INCLUDE_DIR})
endif()

if(KARROT_USE_PACKAGEKIT)
  find_package(Vala 0.16 REQUIRED)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GLIB REQUIRED gio-2.0)
  set(vala_output
    packagekit_g.c
    packagekit_g.h 
    )
  add_custom_command(OUTPUT ${vala_output}
    COMMAND
      ${VALA_EXECUTABLE}
    ARGS
      --pkg=gio-2.0
      -H packagekit_g.h
      -C "${CMAKE_CURRENT_SOURCE_DIR}/packagekit_g.vala"
      -b ${CMAKE_CURRENT_SOURCE_DIR}
      -d ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS
      "${CMAKE_CURRENT_SOURCE_DIR}/packagekit_g.vala"
    )
  include_directories(
    ${GLIB_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    )
  list(APPEND libraries
    ${GLIB_LIBRARIES}
    )
  list(APPEND sources
    ${vala_output}
    packagekit.cpp
    packagekit.hpp
    )
  enable_language(C)
endif()

add_executable(karrot_tool
  main.cpp
  ${sources}
  )
target_link_libraries(karrot_tool
  karrot
  ${libraries}
  ${Boost_LIBRARIES}
  )
set_target_properties(karrot_tool PROPERTIES
  OUTPUT_NAME karrot
  )
