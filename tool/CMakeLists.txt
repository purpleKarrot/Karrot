#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

set(sources)
set(libraries)

find_package(LibGit2)
if(LIBGIT2_FOUND)
  list(APPEND sources
    git.cpp
    git.hpp
    )
  list(APPEND libraries
    ${LibGit2_LIBRARY}
    )
  include_directories(${LibGit2_INCLUDE_DIR})
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
