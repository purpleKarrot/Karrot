# This module sets the following variables:
#
#  VALA_FOUND       Whether the vala compiler has been found
#  VALA_EXECUTABLE  Full path to the valac executable
#  VALA_VERSION     Version number of valac

find_program(VALA_EXECUTABLE valac)
mark_as_advanced(VALA_EXECUTABLE)

if(VALA_EXECUTABLE)
  execute_process(COMMAND ${VALA_EXECUTABLE} "--version" 
    OUTPUT_VARIABLE VALA_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  string(REPLACE "Vala " "" VALA_VERSION "${VALA_VERSION}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Vala
  REQUIRED_VARS VALA_EXECUTABLE
  VERSION_VAR VALA_VERSION
  )
