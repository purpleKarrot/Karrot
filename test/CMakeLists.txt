#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

find_package(Boost "1.46" REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# module tests
set(test_list
  version
  )

# unit tests
if(NOT BUILD_SHARED_LIBS OR (NOT BUILD_VISIBILITY_HIDDEN AND NOT WIN32))
  list(APPEND test_list
    query
    sat
    spec
    unsat
    url
    variants
    vercmp
    )
endif()

create_test_sourcelist(test_sources
  main.cpp
  ${test_list}
  )

add_definitions(-DKARROT_STATIC)
add_executable(karrot_test_driver ${test_sources})
target_link_libraries(karrot_test_driver karrot)

find_program(VALGRIND_EXECUTABLE valgrind)
if(VALGRIND_EXECUTABLE)
  set(launcher ${VALGRIND_EXECUTABLE} --tool=memcheck --leak-check=yes --quiet)
else()
  set(launcher)
endif()

foreach(test IN LISTS test_list)
  add_test(NAME karrot.${test}
    COMMAND ${launcher} $<TARGET_FILE:karrot_test_driver> ${test}
    )
endforeach()

configure_file(coveralls.in ${CMAKE_BINARY_DIR}/coveralls @ONLY)
