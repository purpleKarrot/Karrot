#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

option(KARROT_USE_ARCHIVE    "Build with Archive support"    OFF)
option(KARROT_USE_GIT        "Build with Git support"        OFF)
option(KARROT_USE_PACKAGEKIT "Build with PackageKit support" OFF)
option(KARROT_USE_SUBVERSION "Build with Subversion support" OFF)

set(sources)
set(libraries)

if(KARROT_USE_ARCHIVE)
  find_package(CURL REQUIRED)
  find_package(LibArchive REQUIRED)
  include_directories(
    ${CURL_INCLUDE_DIRS}
    ${LibArchive_INCLUDE_DIRS}
    )
  list(APPEND sources
    archive.cpp
    archive.hpp
    md5.c
    md5.h
    )
  list(APPEND libraries
    ${CURL_LIBRARIES}
    ${LibArchive_LIBRARIES}
    )
  enable_language(C)
  add_definitions(-DUSE_ARCHIVE)
endif()

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
  add_definitions(-DUSE_GIT)
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
  add_definitions(-DUSE_PACKAGEKIT)
endif()

if(KARROT_USE_SUBVERSION)
  find_package(APR       REQUIRED)
  find_package(SVN "1.7" REQUIRED COMPONENTS client delta diff fs subr wc)
  include_directories(
    ${APR_INCLUDE_DIRS}
    ${SVN_INCLUDE_DIRS}
    )
  list(APPEND sources
    subversion.cpp
    subversion.hpp
    )
  list(APPEND libraries
    ${APR_LIBRARIES}
    ${SVN_LIBRARIES}
    )
  add_definitions(-DUSE_SUBVERSION)
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
