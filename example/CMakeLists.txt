#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

option(KARROT_BUILD_EXAMPLE "Build the Karrot example tool" ON)
if(NOT KARROT_BUILD_EXAMPLE)
  return()
endif()

#set(Boost_USE_STATIC_LIBS ON)
find_package(Boost "1.46" REQUIRED COMPONENTS
  filesystem
  program_options
  system
  )
include_directories(${Boost_INCLUDE_DIRS})

add_executable(karrot_example
  karrot.cpp
  karrot.hpp
  )
target_link_libraries(karrot_example
  karrot
  ${Boost_LIBRARIES}
  )
set_target_properties(karrot_example PROPERTIES
  OUTPUT_NAME karrot
  )
