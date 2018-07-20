#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "realsense2::realsense-file" for configuration "Debug"
set_property(TARGET realsense2::realsense-file APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(realsense2::realsense-file PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C;CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/realsense-file.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS realsense2::realsense-file )
list(APPEND _IMPORT_CHECK_FILES_FOR_realsense2::realsense-file "${_IMPORT_PREFIX}/lib/realsense-file.lib" )

# Import target "realsense2::realsense2" for configuration "Debug"
set_property(TARGET realsense2::realsense2 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(realsense2::realsense2 PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/realsense2.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/realsense2.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS realsense2::realsense2 )
list(APPEND _IMPORT_CHECK_FILES_FOR_realsense2::realsense2 "${_IMPORT_PREFIX}/lib/realsense2.lib" "${_IMPORT_PREFIX}/bin/realsense2.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
