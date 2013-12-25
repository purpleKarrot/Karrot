#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

find_package(Boost "1.46" REQUIRED program_options)

add_executable(karrot_tool
  main.cpp
  karrot.rc
  )

target_link_libraries(karrot_tool
  ${Boost_LIBRARIES}
  karrot
  drivers
  )

set_property(TARGET karrot_tool PROPERTY
  OUTPUT_NAME karrot
  )
