#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

find_package(RE2C REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

re2c_target(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/xml_re2c.in.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/xml_re2c.hpp -s
  )

add_library(xml STATIC
  include/xml_reader.hpp
  src/xml_reader.cpp
  src/xml_re2c.in.hpp
  xml_re2c.hpp
  )

set_property(TARGET xml PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(xml PUBLIC include)
