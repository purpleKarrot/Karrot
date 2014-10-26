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

find_path(LIBYAML_INCLUDE_DIR NAMES yaml.h)
find_library(LIBYAML_LIBRARY NAMES yaml libyaml)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibYAML DEFAULT_MSG
  LIBYAML_LIBRARY
  LIBYAML_INCLUDE_DIR
  )

if(LIBYAML_FOUND)
  set(LIBYAML_INCLUDE_DIRS ${LIBYAML_INCLUDE_DIR})
  set(LIBYAML_LIBRARIES ${LIBYAML_LIBRARY})
  if(NOT TARGET yaml::libyaml)
    add_library(yaml::libyaml UNKNOWN IMPORTED)
    set_target_properties(yaml::libyaml PROPERTIES
      IMPORTED_LOCATION "${LIBYAML_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${LIBYAML_INCLUDE_DIR}"
      )
  endif()
endif()

mark_as_advanced(LIBYAML_INCLUDE_DIR LIBYAML_LIBRARY)
