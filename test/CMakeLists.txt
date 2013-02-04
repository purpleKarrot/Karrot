#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

set(test_list
  quark
  url
  )

create_test_sourcelist(test_sources
  main.cpp
  ${test_list}
  )

add_executable(karrot_test_driver ${test_sources})

foreach(test IN LISTS test_list)
  add_test(NAME karrot.${test} COMMAND $<TARGET_FILE:karrot_test_driver> ${test})
endforeach()
