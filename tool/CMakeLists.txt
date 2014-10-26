# Copyright (c) 2014, Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Boost "1.55" REQUIRED filesystem system program_options)
include_directories(${Boost_INCLUDE_DIRS})

find_package(LibYAML REQUIRED)

configure_file(version.h.in version.h)

add_executable(karrot
  cache.cpp
  cache.hpp
  graph.cpp
  graph.hpp
  yaml_writer.cpp
  yaml_writer.hpp
  karrot.rc
  main.cpp
  )

target_link_libraries(karrot
  karrotlib
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  yaml::libyaml
  )
