# btgchk - potential scenery output **CHECKER**, even **CORRECTOR**, if need be...
# CMakeLists.txt, generated gencmake.pl, on 2019/02/15 02:29:29
cmake_minimum_required( VERSION 2.8.8 )

# CMakeScripts or use the ones that come by default with CMake.
# set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules ${CMAKE_MODULE_PATH})

project( btgchk )

# The version number.
set( btgchk_MAJOR 3 )
set( btgchk_MINOR 0 )
set( btgchk_POINT 0 )

# Allow developer to select is Dynamic or static library built
set( LIB_TYPE STATIC )  # set default static
option( BUILD_SHARED_LIB "Set ON to build shared Library (DLL)" OFF )


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
else()
    # items for unix
    list(APPEND EXTRA_LIBS m)
endif()

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT" )
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MSVC_LD_FLAGS}" )

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

#   LibArchive_FOUND        - true if libarchive was found
#   LibArchive_INCLUDE_DIRS - include search path
#   LibArchive_LIBRARIES    - libraries to link
#   LibArchive_VERSION      - libarchive 3-component version number
find_package( LibArchive )
if (LibArchive_FOUND)
    message(STATUS "Found 'LibArchive' vers ${LibArchive_VERSION}")
    message(STATUS "inc = ${LibArchive_INCLUDE_DIRS}, libs = ${LibArchive_LIBRARIES}")
    include_directories( ${LibArchive_INCLUDE_DIRS} )
    list(APPEND EXTRA_LIBS ${LibArchive_LIBRARIES})
else ()
    add_definitions( -DNO_LIBARCH )
    message(WARNING "*** Can NOT locate 'LibArchive'! Use alternates... like zlib..." )
endif ()

# Total libs 0, exes 1
#################################################
##### LIBRARIES 1 #####
#################################################
set(name btgutils)
set(dir .)
set(${name}_SRCS
    ${dir}/airport.c
    ${dir}/bsphere.c
    ${dir}/btgio.c
    ${dir}/check.c
    ${dir}/color.c
    ${dir}/coord.c
    ${dir}/edge.c
    ${dir}/element.c
    ${dir}/file.c
    ${dir}/geometry.c
    ${dir}/header.c
    ${dir}/normal.c
    ${dir}/object.c
    ${dir}/point.c
    ${dir}/property.c
    ${dir}/raw.c
    ${dir}/seam.c
    ${dir}/texcoo.c
    ${dir}/triangle.c
    ${dir}/vertex.c
    )
set(${name}_HDRS
    ${dir}/airport.h
    ${dir}/bsphere.h
    ${dir}/btgio.h
    ${dir}/check.h
    ${dir}/color.h
    ${dir}/coord.h
    ${dir}/edge.h
    ${dir}/element.h
    ${dir}/file.h
    ${dir}/geometry.h
    ${dir}/header.h
    ${dir}/normal.h
    ${dir}/object.h
    ${dir}/point.h
    ${dir}/property.h
    ${dir}/raw.h
    ${dir}/seam.h
    ${dir}/struct.h
    ${dir}/texcoo.h
    ${dir}/triangle.h
    ${dir}/vertex.h
    )
if (MSVC)
list( APPEND ${name}_SRCS ${dir}/win32/getopt.c )
endif ()
add_library( ${name} ${${name}_SRCS} ${${name}_HDRS} )
list(APPEND add_LIBS ${name})
# deal with install, if any...
#install( TARGETS ${name}
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib )
#install(FILES ${${name}_HDRS} DESTINATION include)

#################################################
##### EXECUTABLE 2 #####
#################################################

# btgchk EXECUTABLE from [/home/geoff/fg/next/btgchk],
# have 21 C/C++ sources, 20 headers
set(name btg2dxf)
set(dir .)
set(${name}_SRCS
    ${dir}/${name}.c
    )
add_executable( ${name} ${${name}_SRCS} )
if (add_LIBS OR EXTRA_LIBS)
    target_link_libraries( ${name} ${add_LIBS} ${EXTRA_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install, if any...
#install( TARGETS ${name} DESTINATION bin )

set(name btgchk)
set(dir .)
set(${name}_SRCS
    ${dir}/btgchk.c
    )
add_executable( ${name} ${${name}_SRCS} )
if (add_LIBS OR EXTRA_LIBS)
    target_link_libraries( ${name} ${add_LIBS} ${EXTRA_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install, if any...
#install( TARGETS ${name} DESTINATION bin )

# eof
