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

find_package(RE2C REQUIRED)
find_package(LibYAML REQUIRED)

if(NOT WIN32)
  find_package(CURL REQUIRED)
  include_directories(${CURL_INCLUDE_DIRS})
endif()

configure_file(version.h.in version.h)

re2c_target(
  ${CMAKE_CURRENT_SOURCE_DIR}/xml_re2c.in.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/xml_re2c.hpp -s
  )

add_executable(karrot
  cache.cpp
  cache.hpp
  dependencies.cpp
  dependencies.hpp
  driver.cpp
  driver.hpp
  engine.hpp
  feed_parser.cpp
  feed_parser.hpp
  feed_queue.hpp
  graph.cpp
  graph.hpp
  package_handler.hpp
  package.hpp
  url.cpp
  url.hpp
  url_curl.cpp
  url_win32.cpp
  variants.cpp
  variants.hpp
  xml_re2c.hpp
  xml_re2c.in.hpp
  xml_reader.cpp
  xml_reader.hpp
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

if(WIN32)
  target_link_libraries(karrot winhttp)
else()
  target_link_libraries(karrot
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CURL_LIBRARIES}
    )
endif()
