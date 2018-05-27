# FindSHP2.cmake - localized MSVC version - 20140417
# Find the native SHP includes and library

# This module defines
#  SHP2_INCLUDE_DIR, where to find shapefil.h, etc.
#  SHP2_LIBRARIES, libraries to link against to use SHP.
#  SHP2_FOUND, If false, do not try to use SHP.

find_path(SHP2_INCLUDE_DIR shapefil.h
    HINTS ${SHP_ROOT} $ENV{SHP_ROOT}
    PATH_SUFFIXES include)

set(SHP2_NAMES ${SHP2_NAMES} shp libshp)
if (MSVC)
    find_library(SHP2_LIB_DBG NAMES shpd )
    find_library(SHP2_LIB_REL NAMES shp )
    if (SHP2_LIB_DBG AND SHP2_LIB_REL)
        set(SHP2_LIBRARY
            debug ${SHP2_LIB_DBG}
            optimized ${SHP2_LIB_REL}
            )
    elseif (SHP2_LIB_REL)
        set(SHP2_LIBRARY ${SHP2_LIB_REL})
    endif ()
else ()
    find_library(SHP2_LIBRARY NAMES ${SHP2_NAMES} )
endif ()

# handle the QUIETLY and REQUIRED arguments and set SHP2_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SHP2
                                  REQUIRED_VARS SHP2_LIBRARY SHP2_INCLUDE_DIR)
if(SHP2_FOUND)
  set( SHP2_LIBRARIES ${SHP2_LIBRARY} )
endif()

# eof
