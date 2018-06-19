# test-btg project - using simgear show contents of STG/BTG files
# CMakeLists.txt, generated gencmake.pl, on 2015/03/28 17:59:52
cmake_minimum_required( VERSION 2.8.8 )

include (CheckIncludeFile)

# CMakeScripts or use the ones that come by default with CMake.
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

project( test_btg )

# The version number.
file(READ version.txt versionFile)
if (NOT versionFile)
    message(FATAL_ERROR "*** Error: Unable to determine project version. version.txt file is missing!")
endif()
string(STRIP "${versionFile}" VERSION_TEXT)
string(REGEX REPLACE "(.*)[\r\n|\n](.*)" "\\1" PV_VERSION ${VERSION_TEXT})
string(REGEX REPLACE "(.*)[\r\n|\n](.*)" "\\2" PV_DATE ${VERSION_TEXT})
# establish version number
if (PV_VERSION)
    string(REPLACE "." ";" VERSION_LIST ${PV_VERSION})
    list(GET VERSION_LIST 0 PV_MAJOR_VERSION)
    list(GET VERSION_LIST 1 PV_MINOR_VERSION)
    list(GET VERSION_LIST 2 PV_POINT_VERSION)
else ()
    message(FATAL_ERROR "*** FAILED to get a VERSION from version.txt!")
endif ()    
# establish version date
if (PV_DATE)
    string(REPLACE "." ";" DATE_LIST ${PV_DATE})
    list(GET DATE_LIST 0 PV_YEAR)
    list(GET DATE_LIST 1 PV_MONTH)
    list(GET DATE_LIST 2 PV_DAY)
else ()
    message(FATAL_ERROR "*** FAILED to get a DATE from version.txt!")
endif ()
add_definitions( -DTEST_BTG_VERSION="${PV_VERSION}" )
add_definitions( -DTEST_BTG_DATE="${PV_DATE}" )
message(STATUS "*** Set TEST_BTG_VERSION=${PV_VERSION} DATE=${PV_DATE}")

option(USE_EXTERN_ZLIB "Set ON to use find and use extrnal zlib library"  OFF)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(IS_64_BIT 1)
    message(STATUS "*** Compiling in 64-bit environmnet")
    if (NOT NV_SYSTEM_PROCESSOR)
        set(NV_SYSTEM_PROCESSOR "AMD64")
    endif ()
    add_definitions( -DWIN64 )
else ()
    set(IS_64_BIT 0)
    message(STATUS "*** Compiling in 32-bit environmnet")
endif ()

# Allow developer to select is Dynamic or static library built
set( LIB_TYPE STATIC )  # set default static
option( BUILD_SHARED_LIB "Set ON to build/use SHARED Libraries" OFF )

if(CMAKE_COMPILER_IS_GNUCXX)
    set( WARNING_FLAGS -Wall )
endif(CMAKE_COMPILER_IS_GNUCXX)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") 
   set( WARNING_FLAGS "-Wall -Wno-overloaded-virtual" )
endif() 

if(WIN32 AND MSVC)
    # turn off various warnings - none needed in this compile
    set(WARNING_FLAGS "${WARNING_FLAGS} /wd4996")
    # foreach(warning 4244 4251 4267 4275 4290 4786 4305)
    #     set(WARNING_FLAGS "${WARNING_FLAGS} /wd${warning}")
    # endforeach(warning)
    set( MSVC_FLAGS "-DNOMINMAX -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -D__CRT_NONSTDC_NO_WARNINGS" )
    # if (${MSVC_VERSION} EQUAL 1600)
    #    set( MSVC_LD_FLAGS "/FORCE:MULTIPLE" )
    # endif (${MSVC_VERSION} EQUAL 1600)
    #set( NOMINMAX 1 )
    # to distinguish between debug and release libs
    set( CMAKE_DEBUG_POSTFIX "d" )
    list(APPEND add_LIBS ws2_32 Winmm)
else()
    # items for unix
    set(WARNING_FLAGS "${WARNING_FLAGS} -Wno-unused-variable")
    set(CXX_FLAGS "${CXX_FLAGS} -std=c++11")
endif()

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT" )
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MSVC_LD_FLAGS}" )

check_include_file( sys/time.h HAVE_SYS_TIME_H )
if (HAVE_SYS_TIME_H)
    add_definitions( -DHAVE_SYS_TIME_H )
    message(STATUS "Found sys/time.h and set HAVE_SYS_TIME_H")
else ()
    message(STATUS "Not found sys/time.h...")
endif ()
# Setup MSVC 3rd party directories
# is this required??? include( ConfigureMsvc3rdParty )

#   ZLIB_INCLUDE_DIRS   - where to find zlib.h, etc.
#   ZLIB_LIBRARIES      - List of libraries when using zlib.
#   ZLIB_FOUND          - True if zlib found.
#   ZLIB_VERSION_STRING - The version of zlib found (x.y.z)
if (USE_EXTERN_ZLIB)
    find_package(ZLIB REQUIRED)
else ()
    set(ZLIB_INCLUDE_DIRS 3rdParty/zlib_1_2_8)
    configure_file(${ZLIB_INCLUDE_DIRS}/zconf.h.in ${CMAKE_BINARY_DIR}/zconf.h @ONLY)
    ### add_subdirectory( ${ZLIB_INCLUDE_DIRS} ) # build internal ZLIB static
    set(name zlibstatic)
    set(dir ${ZLIB_INCLUDE_DIRS})
    set (${name}_HDRS
        ${dir}/zlib.h
        ${dir}/crc32.h
        ${dir}/deflate.h
        ${dir}/gzguts.h
        ${dir}/inffast.h
        ${dir}/inffixed.h
        ${dir}/inflate.h
        ${dir}/inftrees.h
        ${dir}/trees.h
        ${dir}/zutil.h
        ${CMAKE_BINARY_DIR}/zconf.h
        )
    set (${name}_SRCS
        ${dir}/adler32.c
        ${dir}/compress.c
        ${dir}/crc32.c
        ${dir}/deflate.c
        ${dir}/gzclose.c
        ${dir}/gzlib.c
        ${dir}/gzread.c
        ${dir}/gzwrite.c
        ${dir}/inflate.c
        ${dir}/infback.c
        ${dir}/inftrees.c
        ${dir}/inffast.c
        ${dir}/trees.c
        ${dir}/uncompr.c
        ${dir}/zutil.c
        # ${dir}/zlib.rc
        )
    if (WIN32 AND NOT MINGW)
        list(APPEND ${name}_SRCS ${dir}/win32/zlib.rc)
    endif ()
    add_library( ${name} ${${name}_SRCS} ${${name}_HDRS} )
    list(APPEND add_LIBS ${name})
    include_directories( ${dir} ${CMAKE_BINARY_DIR} )
    set(ZLIB_FOUND 1)
    set(ZLIB_LIBRARIES ${name})
    message(STATUS "*** Using internal zlib source in ${ZLIB_INCLUDE_DIRS}")
endif ()

# SIMGEAR_FOUND, if false, do not try to link to SimGear
# SIMGEAR_CORE_LIBRARIES, a list of the core static libraries
# SIMGEAR_LIBRARIES, a list of all the static libraries (core + scene)
# SIMGEAR_INCLUDE_DIR, where to find the headers
message(STATUS "*** Doing dearch for SimGear (and zlib)")
find_package(SimGear "2.6" REQUIRED)
list(APPEND add_LIBS ${SIMGEAR_CORE_LIBRARIES}) # no scenery ${SIMGEAR_LIBRARIES})
message(STATUS "Found SG libs ${SIMGEAR_CORE_LIBRARIES} inc ${SIMGEAR_INCLUDE_DIR}")
include_directories( ${SIMGEAR_INCLUDE_DIR} )
if (USE_EXTERN_ZLIB)
    list(APPEND add_LIBS ${ZLIB_LIBRARIES})
    message(STATUS "Found ZLIB libs ${ZLIB_LIBRARIES} inc ${ZLIB_INCLUDE_DIRS}")
    include_directories( ${ZLIB_INCLUDE_DIRS} )
endif ()

#   Boost_FOUND            - True if headers and requested libraries were found
#   Boost_INCLUDE_DIRS     - Boost include directories
find_package(Boost   REQUIRED)
include_directories( ${Boost_INCLUDE_DIRS} )
if (UNIX)
    list(APPEND add_LIBS ${CMAKE_THREAD_LIBS_INIT})
endif ()

#  SHP2_INCLUDE_DIR, where to find shapefil.h, etc.
#  SHP2_LIBRARIES, libraries to link against to use SHP.
#  SHP2_FOUND, If false, do not try to use SHP.
find_package( SHP2 )
if (SHP2_FOUND)
    message(STATUS "Found SHP shapefil lib: ${SHP2_LIBRARIES}, inc ${SHP2_INCLUDE_DIR}" )
    list(APPEND add_LIBS ${SHP2_LIBRARIES})
    include_directories( ${SHP2_INCLUDE_DIR} )
    add_definitions( -DHAVE_SHAPEFILE_H )
else ()
    message(STATUS "Note: SHP shpfile NOT FOUND" )
endif ()    

set (CGAL_MINIMUM 4.0)
set (CGAL_OLD 0)
find_package(CGAL COMPONENTS Core REQUIRED)
if (CGAL_FOUND)
  if(${CGAL_MAJOR_VERSION}.${CGAL_MINOR_VERSION} VERSION_LESS ${CGAL_MINIMUM})
        message(FATAL_ERROR "CGAL >= ${CGAL_MINIMUM} is required. Found CGAL ${CGAL_MAJOR_VERSION}.${CGAL_MINOR_VERSION}")
  else(${CGAL_MAJOR_VERSION}.${CGAL_MINOR_VERSION} VERSION_LESS ${CGAL_MINIMUM})
        message(STATUS "Found CGAL ${CGAL_MAJOR_VERSION}.${CGAL_MINOR_VERSION}")
	if(${CGAL_MAJOR_VERSION}.${CGAL_MINOR_VERSION} VERSION_LESS 4.5)
		set (CGAL_OLD 1)
	endif()
  endif(${CGAL_MAJOR_VERSION}.${CGAL_MINOR_VERSION} VERSION_LESS ${CGAL_MINIMUM})
endif (CGAL_FOUND)

if (CGAL_USE_FILE)
include ( ${CGAL_USE_FILE} )
endif ()

#     GDAL_FOUND - True if libgdal is found
#     GDAL_LIBRARY - A variable pointing to the GDAL library
#     GDAL_INCLUDE_DIR - Where to find the headers
# Name: ${_gdal_lib} HINTS ENV GDAL_DIR ENV GDAL_ROOT ${_gdal_libpath}
find_package(GDAL 2.0.0)
if (GDAL_FOUND)
    message(STATUS "Found GDAL - inc ${GDAL_INCLUDE_DIR}, lib ${GDAL_LIBRARY}")
    if (CGAL_USE_FILE)
        message(STATUS "Have CGAL_USE_FILE=${CGAL_USE_FILE}" )
    endif ()
    list(APPEND add_LIBS ${GDAL_LIBRARY})
    include_directories( ${GDAL_INCLUDE_DIR} )
    add_definitions( -DHAVE_GDAL_H )
    include_directories( src/Lib )  # access to libs
    add_subdirectory(src/Lib/poly2tri)
    add_subdirectory(src/Lib/Output)
    add_subdirectory(src/Lib/TriangleJRS)
    add_subdirectory(src/Lib/Polygon)
    add_subdirectory(src/Lib/Geometry)
    add_subdirectory(src/Lib/vpf)   # VMAP0 lib and tools to explore
    add_subdirectory(src/Prep/TGVPF) # enable extraction tool
else ()
    message(WARNING "NOT Found GDAL - HINTS ENV $ENV{GDAL_DIR}, ENV $ENV{GDAL_ROOT}")
endif ()
    
#####################################################################
## configuration
check_include_file(dirent.h HAVE_DIRENT_H)

# configuration file, if needed
# configure_file( ${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_BINARY_DIR}/config.h )
# add_definitions( -DHAVE_CONFIG_H )
# include_directories( ${CMAKE_BINARY_DIR} )

if(BUILD_SHARED_LIB)
   set(LIB_TYPE SHARED)
   message(STATUS "*** Building DLL library ${LIB_TYPE}")
else()
   message(STATUS "*** Option BUILD_SHARED_LIB is OFF ${LIB_TYPE}")
endif()

#################################################
##### LIBRARY #####
#################################################
set(name utils)
set(dir src/utils)
include_directories( ${dir} )
set(${name}_SRCS
    ${dir}/sprtf.cxx
    ${dir}/utils.cxx
    ${dir}/sprtfstr.cxx
    )
set(${name}_HDRS
    ${dir}/sprtf.hxx
    ${dir}/utils.hxx
    ${dir}/sprtfstr.hxx
    )
if (HAVE_DIRENT_H)
    message(STATUS "Found system dirent.h - excluding local source")
else ()
    message(STATUS "dirent.h NOT found - using local source")
    include_directories( src/dirent )
    list(APPEND ${name}_SRCS src/dirent/dirent.c)
    list(APPEND ${name}_HDRS src/dirent/dirent.h)
endif ()    
add_library( ${name} ${${name}_SRCS} ${${name}_HDRS} )
list(APPEND add_LIBS ${name})

#################################################
##### EXECUTABLE #####
#################################################

# test-btg EXECUTABLE from [C:\GTools\ConApps\tests\test-btg\src],
# have 1 C/C++ sources, 1 headers
set(name test-btg)
set(dir src)
set(${name}_SRCS
    ${dir}/test-btg.cxx
    ${dir}/load-btg.cxx
    ${dir}/load-stg.cxx
    ${dir}/palette.cxx
    )
set(${name}_HDRS
    ${dir}/test-btg.hxx
    ${dir}/load-btg.hxx
    ${dir}/load-stg.hxx
    ${dir}/palette.hxx
    )
add_executable( ${name} ${${name}_SRCS} ${${name}_HDRS} )
if (add_LIBS)
    message(STATUS "*** target_link_libraries( ${name} ${add_LIBS} )")
    target_link_libraries( ${name} ${add_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install, if any...
install( TARGETS ${name} DESTINATION bin )

# compile the PUBLIC DOMAIN vpf extractor
add_subdirectory(src/vpf)

# eof
