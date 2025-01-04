#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "orocos-kdl" for configuration "RelWithDebInfo"
set_property(TARGET orocos-kdl APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(orocos-kdl PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/orocos-kdl.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/orocos-kdl.dll"
  )

list(APPEND _cmake_import_check_targets orocos-kdl )
list(APPEND _cmake_import_check_files_for_orocos-kdl "${_IMPORT_PREFIX}/lib/orocos-kdl.lib" "${_IMPORT_PREFIX}/bin/orocos-kdl.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
