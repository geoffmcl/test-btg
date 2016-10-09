# test-btg project - using simgear show contents of STG/BTG files
# CMakeLists.txt, generated gencmake.pl, on 2015/03/28 17:59:52
cmake_minimum_required( VERSION 2.8.8 )

include (CheckIncludeFile)

# CMakeScripts or use the ones that come by default with CMake.
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

project( test_btg )

# The version number.
set( test_btg_MAJOR 3 )
set( test_btg_MINOR 0 )
set( test_btg_POINT 0 )

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
endif()

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT" )
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MSVC_LD_FLAGS}" )

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
list(APPEND add_LIBS ${SIMGEAR_LIBRARIES})
message(STATUS "Found SG libs ${SIMGEAR_LIBRARIES} inc ${SIMGEAR_INCLUDE_DIR}")
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

# eof
