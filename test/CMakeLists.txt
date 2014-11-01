#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

find_package(Boost "1.46" REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(test_list
  query
  sat
  spec
  string_pool
  unsat
  url
  vercmp
  )

create_test_sourcelist(test_sources
  main.cpp
  ${test_list}
  )

add_executable(karrot_test_driver ${test_sources})
target_link_libraries(karrot_test_driver karrotlib)

find_program(VALGRIND_EXECUTABLE valgrind)

foreach(test IN LISTS test_list)
  add_test(NAME karrot.${test}
    COMMAND $<TARGET_FILE:karrot_test_driver> ${test}
    )
  if(VALGRIND_EXECUTABLE)
    add_test(NAME karrot.${test}.memcheck
      COMMAND ${VALGRIND_EXECUTABLE}
        --tool=memcheck --leak-check=full --show-reachable=yes --track-origins=yes
        $<TARGET_FILE:karrot_test_driver> ${test}
      )
    set_tests_properties(karrot.${test}.memcheck PROPERTIES
      PASS_REGULAR_EXPRESSION " 0 errors from 0 contexts"
      DEPENDS karrot.${test}
      )
  endif()
endforeach()

configure_file(coveralls.in ${CMAKE_BINARY_DIR}/coveralls @ONLY)
