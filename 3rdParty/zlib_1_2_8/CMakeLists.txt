cmake_minimum_required (VERSION 2.8)
PROJECT (ZLIB C)

#-----------------------------------------------------------------------------
# Basic ZLIB stuff here
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Define some CMake variables for use later in the project
#-----------------------------------------------------------------------------
SET (HDF_RESOURCES_DIR           ${ZLIB_SOURCE_DIR}/config/cmake)
SET (ZLIB_SRC_DIR                ${ZLIB_SOURCE_DIR})
SET (ZLIB_EXAMPLES_SOURCE_DIR    ${ZLIB_SOURCE_DIR}/examples)

#-----------------------------------------------------------------------------
# Set the core names of all the libraries
#-----------------------------------------------------------------------------
SET (ZLIB_LIB_CORENAME           "zlib")

#-----------------------------------------------------------------------------
# Set the true names of all the libraries if customized by external project
#-----------------------------------------------------------------------------
SET (ZLIB_LIB_NAME              "${ZLIB_EXTERNAL_LIB_PREFIX}${ZLIB_LIB_CORENAME}")

#-----------------------------------------------------------------------------
# Set the target names of all the libraries
#-----------------------------------------------------------------------------
SET (ZLIB_LIB_TARGET             ${ZLIB_LIB_CORENAME})

IF (NOT ZLIB_INSTALL_BIN_DIR)
  SET (ZLIB_INSTALL_BIN_DIR bin)
ENDIF (NOT ZLIB_INSTALL_BIN_DIR)
IF (NOT ZLIB_INSTALL_LIB_DIR)
  SET (ZLIB_INSTALL_LIB_DIR lib)
ENDIF (NOT ZLIB_INSTALL_LIB_DIR)
IF (NOT ZLIB_INSTALL_INCLUDE_DIR)
  SET (ZLIB_INSTALL_INCLUDE_DIR include)
ENDIF (NOT ZLIB_INSTALL_INCLUDE_DIR)
IF (NOT ZLIB_INSTALL_DATA_DIR)
  IF (NOT WIN32)
    SET (ZLIB_INSTALL_DATA_DIR share)
    SET (ZLIB_INSTALL_CMAKE_DIR share/cmake)
  ELSE (NOT WIN32)
    SET (ZLIB_INSTALL_DATA_DIR ".")
    SET (ZLIB_INSTALL_CMAKE_DIR cmake)
  ENDIF (NOT WIN32)
ENDIF (NOT ZLIB_INSTALL_DATA_DIR)

#-----------------------------------------------------------------------------
# parse the full version number from zlib.h and include in ZLIB_VERS_INFO
#-----------------------------------------------------------------------------
FILE (READ ${ZLIB_SRC_DIR}/zlib.h _zlib_h_contents)
STRING (REGEX REPLACE ".*#define[ \t]+ZLIB_VER_MAJOR[ \t]+([0-9]*).*$"
    "\\1" ZLIB_VERS_MAJOR ${_zlib_h_contents})
STRING (REGEX REPLACE ".*#define[ \t]+ZLIB_VER_MINOR[ \t]+([0-9]*).*$"
    "\\1" ZLIB_VERS_MINOR ${_zlib_h_contents})
STRING (REGEX REPLACE ".*#define[ \t]+ZLIB_VER_REVISION[ \t]+([0-9]*).*$"
    "\\1" ZLIB_VERS_RELEASE ${_zlib_h_contents})
STRING (REGEX REPLACE ".*#define[ \t]+ZLIB_VER_SUBREVISION[ \t]+([0-9]*).*$"
    "\\1" ZLIB_VERS_SUBRELEASE ${_zlib_h_contents})
#MESSAGE (STATUS "VERSION: ${ZLIB_VERS_MAJOR}.${ZLIB_VERS_MINOR}.${ZLIB_VERS_RELEASE}-${ZLIB_VERS_SUBRELEASE}")
STRING (REGEX REPLACE ".*#define[ \t]+ZLIB_VERSION[ \t]+\"([0-9A-Za-z.]+)\".*"
    "\\1" ZLIB_FULL_VERSION ${_zlib_h_contents})
#MESSAGE (STATUS "VERSION: ${ZLIB_FULL_VERSION}")

#-----------------------------------------------------------------------------
SET (ZLIB_PACKAGE "zlib")
SET (ZLIB_PACKAGE_NAME "ZLIB")
SET (ZLIB_PACKAGE_VERSION "${ZLIB_VERS_MAJOR}.${ZLIB_VERS_MINOR}")
SET (ZLIB_PACKAGE_VERSION_MAJOR "${ZLIB_VERS_MAJOR}.${ZLIB_VERS_MINOR}")
SET (ZLIB_PACKAGE_VERSION_MINOR "${ZLIB_VERS_RELEASE}")
SET (ZLIB_PACKAGE_STRING "${ZLIB_PACKAGE_NAME} ${ZLIB_PACKAGE_VERSION}-${ZLIB_VERS_SUBRELEASE}")
SET (ZLIB_PACKAGE_TARNAME "zlib")
SET (ZLIB_PACKAGE_URL "http://www.hdfgroup.org")
SET (ZLIB_PACKAGE_BUGREPORT "help@hdfgroup.org")

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
INCLUDE (${HDF_RESOURCES_DIR}/HDFMacros.cmake)
INCLUDE (${HDF_RESOURCES_DIR}/ZLIBMacros.cmake)

#-----------------------------------------------------------------------------
# Setup output Directories
#-----------------------------------------------------------------------------
IF (NOT ZLIB_EXTERNALLY_CONFIGURED)
  SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables."
  )
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Libraries"
  )
  SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all static libraries."
  )
ELSE (NOT ZLIB_EXTERNALLY_CONFIGURED)
  # if we are externally configured, but the project uses old cmake scripts
  # this may not be set
  IF (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
  ENDIF (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
ENDIF (NOT ZLIB_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Targets built within this project are exported at Install time for use
# by other projects using FindZLIB. 
#-----------------------------------------------------------------------------
IF (NOT ZLIB_EXPORTED_TARGETS)
  SET (ZLIB_EXPORTED_TARGETS "zlib-targets")
ENDIF (NOT ZLIB_EXPORTED_TARGETS)

#-----------------------------------------------------------------------------
# To include a library in the list exported by the project AT BUILD TIME,
# add it to this variable. This is NOT used by Make Install, but for projects
# which include ZLIB as a sub-project within their build tree
#-----------------------------------------------------------------------------
SET_GLOBAL_VARIABLE(ZLIB_LIBRARIES_TO_EXPORT "")

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
INCLUDE (${HDF_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# Option to Build Shared/Static libs, default is static
#-----------------------------------------------------------------------------
OPTION (BUILD_SHARED_LIBS "Build Shared Libraries" OFF)
SET (LIB_TYPE STATIC)
IF (BUILD_SHARED_LIBS)
  SET (LIB_TYPE SHARED)
ELSE (BUILD_SHARED_LIBS)
  IF (NOT WIN32)
    # should this be a user setting : Everyone uses it anyway ?
    ADD_DEFINITIONS (-DPIC)
  ENDIF (NOT WIN32)
ENDIF (BUILD_SHARED_LIBS)

#-----------------------------------------------------------------------------
# Option to Build Assembly code
#-----------------------------------------------------------------------------
OPTION (ASM686 "Enable building i686 assembly implementation")
OPTION (AMD64 "Enable building amd64 assembly implementation")

#-----------------------------------------------------------------------------
# When building utility executables that generate other (source) files :
# we make use of the following variables defined in the root CMakeLists.
# Certain systems may add /Debug or /Release to output paths
# and we need to call the executable from inside the CMake configuration
#-----------------------------------------------------------------------------
SET (EXE_EXT "")
IF (WIN32)
  SET (EXE_EXT ".exe")
  ADD_DEFINITIONS (-DBIND_TO_CURRENT_VCLIBS_VERSION=1)
  ADD_DEFINITIONS (-D_CRT_SECURE_NO_WARNINGS)
  ADD_DEFINITIONS (-D_CRT_NONSTDC_NO_DEPRECATE)
ENDIF (WIN32)

IF (MSVC)
  SET (CMAKE_MFC_FLAG 0)
ENDIF (MSVC)

SET (MAKE_SYSTEM)
IF (CMAKE_BUILD_TOOL MATCHES "make")
  SET (MAKE_SYSTEM 1)
ENDIF (CMAKE_BUILD_TOOL MATCHES "make")

SET (CFG_INIT "/${CMAKE_CFG_INTDIR}")
IF (MAKE_SYSTEM)
  SET (CFG_INIT "")
ENDIF (MAKE_SYSTEM)

#-----------------------------------------------------------------------------
# Compiler specific flags : Shouldn't there be compiler tests for these
#-----------------------------------------------------------------------------
IF (CMAKE_COMPILER_IS_GNUCC)
  SET (CMAKE_C_FLAGS "${CMAKE_ANSI_CFLAGS} ${CMAKE_C_FLAGS} -std=c99 -fomit-frame-pointer -finline-functions -fno-common")
ENDIF (CMAKE_COMPILER_IS_GNUCC)
IF (CMAKE_COMPILER_IS_GNUCXX)
  SET (CMAKE_CXX_FLAGS "${CMAKE_ANSI_CFLAGS} ${CMAKE_CXX_FLAGS} -fomit-frame-pointer -finline-functions -fno-common")
ENDIF (CMAKE_COMPILER_IS_GNUCXX)

#-----------------------------------------------------------------------------
# This is in here to help some of the GCC based IDES like Eclipse
# and code blocks parse the compiler errors and warnings better.
#-----------------------------------------------------------------------------
IF (CMAKE_COMPILER_IS_GNUCC)
  SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDIF (CMAKE_COMPILER_IS_GNUCC)
IF (CMAKE_COMPILER_IS_GNUCXX)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF (CMAKE_COMPILER_IS_GNUCXX)

#-----------------------------------------------------------------------------
# Include user macros
#-----------------------------------------------------------------------------
INCLUDE (UserMacros.cmake)

#-----------------------------------------------------------------------------
# All libs/tests/examples need the main include directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${ZLIB_BINARY_DIR} ${ZLIB_SOURCE_DIR})

IF (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    # If we're doing an out of source build and the user has a zconf.h
    # in their source tree...
    IF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/zconf.h)
        MESSAGE (FATAL_ERROR
            "You must remove ${CMAKE_CURRENT_SOURCE_DIR}/zconf.h "
            "from the source tree.  This file is included with zlib "
            "but CMake generates this file for you automatically "
            "in the build directory.")
  ENDIF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/zconf.h)
ENDIF (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)

#============================================================================
# zlib
#============================================================================

INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR})

#-----------------------------------------------------------------------------
# Define zlib Library
#-----------------------------------------------------------------------------
SET (ZLIB_PUBLIC_HEADERS
    zlib.h
)
SET (ZLIB_HDRS
    crc32.h
    deflate.h
    gzguts.h
    inffast.h
    inffixed.h
    inflate.h
    inftrees.h
    trees.h
    zutil.h
)
SET (ZLIB_SRCS
    adler32.c
    compress.c
    crc32.c
    deflate.c
    gzclose.c
    gzlib.c
    gzread.c
    gzwrite.c
    inflate.c
    infback.c
    inftrees.c
    inffast.c
    trees.c
    uncompr.c
    zutil.c
#    zlib.rc
)
IF (WIN32 AND NOT MINGW)
  SET (ZLIB_SRCS ${ZLIB_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/win32/zlib.rc)
ENDIF (WIN32 AND NOT MINGW)

IF (MINGW)
  # This gets us DLL resource information when compiling on MinGW.
  ADD_CUSTOM_COMMAND (
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/zlibrc.obj
      COMMAND windres.exe
          -D GCC_WINDRES
          -I ${CMAKE_CURRENT_SOURCE_DIR}
          -I ${CMAKE_CURRENT_BINARY_DIR}
          -o ${CMAKE_CURRENT_BINARY_DIR}/zlibrc.obj
          -i ${CMAKE_CURRENT_SOURCE_DIR}/win32/zlib.rc
  )
  SET (ZLIB_SRCS ${ZLIB_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/zlibrc.obj)
ENDIF (MINGW)

IF (CMAKE_COMPILER_IS_GNUCC)
    IF (ASM686)
        SET (ZLIB_ASMS contrib/asm686/match.S)
    ELSEIF (AMD64)
        SET (ZLIB_ASMS contrib/amd64/amd64-match.S)
    ENDIF  ()

    IF (ZLIB_ASMS)
        ADD_DEFINITIONS (-DASMV)
        SET_SOURCE_FILES_PROPERTIES (${ZLIB_ASMS} PROPERTIES LANGUAGE C COMPILE_FLAGS -DNO_UNDERLINE)
    ENDIF (ZLIB_ASMS)
ENDIF (CMAKE_COMPILER_IS_GNUCC)

IF (MSVC)
    IF (ASM686)
        ENABLE_LANGUAGE (ASM_MASM)
        SET (ZLIB_ASMS
            contrib/masmx86/inffas32.asm
            contrib/masmx86/match686.asm
        )
    ELSEIF (AMD64)
        ENABLE_LANGUAGE (ASM_MASM)
        SET (ZLIB_ASMS
            contrib/masmx64/gvmat64.asm
            contrib/masmx64/inffasx64.asm
        )
    ENDIF ()

    IF (ZLIB_ASMS)
        ADD_DEFINITIONS (-DASMV -DASMINF)
    ENDIF (ZLIB_ASMS)
ENDIF (MSVC)

ADD_LIBRARY (${ZLIB_LIB_TARGET} ${LIB_TYPE} ${ZLIB_SRCS} ${ZLIB_HDRS} ${ZLIB_PUBLIC_HEADERS})

SET (SHARED_LINK_FLAGS " ")
IF (BUILD_SHARED_LIBS)
  IF (WIN32)
    ADD_DEFINITIONS (-DZLIB_DLL)
    IF (MSVC)
      SET (SHARED_LINK_FLAGS "/DLL /DEF:${CMAKE_CURRENT_SOURCE_DIR}/win32/zlib.def")
    ENDIF (MSVC)
  ENDIF (WIN32)
  SET_TARGET_PROPERTIES(${ZLIB_LIB_TARGET} PROPERTIES DEFINE_SYMBOL ${ZLIB_LIB_CORENAME}_EXPORTS)
ELSE (BUILD_SHARED_LIBS)
    IF (MSVC AND CMAKE_CL_64)
      SET_TARGET_PROPERTIES (${ZLIB_LIB_TARGET} PROPERTIES STATIC_LIBRARY_FLAGS "/machine:x64")
    ENDIF (MSVC AND CMAKE_CL_64)
ENDIF (BUILD_SHARED_LIBS)
TARGET_C_PROPERTIES (${ZLIB_LIB_TARGET} " " "${SHARED_LINK_FLAGS}")
TARGET_LINK_LIBRARIES (${ZLIB_LIB_TARGET} ${LINK_LIBS})
ZLIB_SET_LIB_OPTIONS (${ZLIB_LIB_TARGET} ${ZLIB_LIB_NAME} ${LIB_TYPE})

SET_GLOBAL_VARIABLE (ZLIB_LIBRARIES_TO_EXPORT ${ZLIB_LIB_TARGET})

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
OPTION (BUILD_TESTING "Build ZLIB Unit Testing" OFF)
IF (BUILD_TESTING)
  SET (DART_TESTING_TIMEOUT 1200 CACHE INTEGER
       "Timeout in seconds for each test (default 1200=20minutes)")
  ENABLE_TESTING ()
  INCLUDE (CTest)
  ADD_SUBDIRECTORY (${ZLIB_SOURCE_DIR}/test ${PROJECT_BINARY_DIR}/test)

#-----------------------------------------------------------------------------
# Option to build examples
#-----------------------------------------------------------------------------
  OPTION (BUILD_EXAMPLES  "Build ZLIB Library Examples" OFF)
  IF (BUILD_EXAMPLES)
    ADD_SUBDIRECTORY (${ZLIB_SOURCE_DIR}/examples ${PROJECT_BINARY_DIR}/examples)
  ENDIF (BUILD_EXAMPLES)
ENDIF (BUILD_TESTING)

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install 
#-----------------------------------------------------------------------------
INSTALL (
    FILES
        ${ZLIB_PUBLIC_HEADERS}
    DESTINATION
        ${ZLIB_INSTALL_INCLUDE_DIR}
    COMPONENT
        headers
)

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
IF (ZLIB_EXPORTED_TARGETS)
  INSTALL (
      TARGETS
          ${ZLIB_LIB_TARGET}
      EXPORT
          ${ZLIB_EXPORTED_TARGETS}
      LIBRARY DESTINATION ${ZLIB_INSTALL_LIB_DIR} COMPONENT libraries
      ARCHIVE DESTINATION ${ZLIB_INSTALL_LIB_DIR} COMPONENT libraries
      RUNTIME DESTINATION ${ZLIB_INSTALL_BIN_DIR} COMPONENT libraries
  )
ENDIF (ZLIB_EXPORTED_TARGETS)

#-----------------------------------------------------------------------------
# Generate the zconf.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
CONFIGURE_FILE (${HDF_RESOURCES_DIR}/zconf.h.in ${ZLIB_BINARY_DIR}/zconf.h @ONLY)

IF (NOT SKIP_INSTALL_FILES AND NOT SKIP_INSTALL_ALL)
  INSTALL (FILES zlib.3 DESTINATION share/man/man3)
ENDIF (NOT SKIP_INSTALL_FILES AND NOT SKIP_INSTALL_ALL)

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
IF (NOT ZLIB_INSTALL_NO_DEVELOPMENT)
  INSTALL (
      FILES ${PROJECT_BINARY_DIR}/zconf.h
      DESTINATION ${ZLIB_INSTALL_INCLUDE_DIR}
      COMPONENT headers
  )
ENDIF (NOT ZLIB_INSTALL_NO_DEVELOPMENT)

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
IF (NOT ZLIB_EXTERNALLY_CONFIGURED)
  INSTALL (
      EXPORT ${ZLIB_EXPORTED_TARGETS}
      DESTINATION ${ZLIB_INSTALL_CMAKE_DIR}/ZLIB
      FILE ${ZLIB_PACKAGE}${HDF_PACKAGE_EXT}-targets.cmake
      COMPONENT configinstall
  )
ENDIF (NOT ZLIB_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Export all exported targets to the build tree for use by parent project
#-----------------------------------------------------------------------------
IF (NOT ZLIB_EXTERNALLY_CONFIGURED)
  EXPORT (
      TARGETS ${ZLIB_LIBRARIES_TO_EXPORT} ${ZLIB_LIB_DEPENDENCIES}
      FILE ${ZLIB_PACKAGE}${HDF_PACKAGE_EXT}-targets.cmake
  )
ENDIF (NOT ZLIB_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Configure the ZLIB-config.cmake file for the build directory
#-----------------------------------------------------------------------------
SET (ZLIB_INCLUDES_BUILD_TIME
    ${ZLIB_SRC_DIR} ${ZLIB_BINARY_DIR}
)
SET (ZLIB_VERSION_STRING @ZLIB_PACKAGE_VERSION@)
SET (ZLIB_VERSION_MAJOR  @ZLIB_PACKAGE_VERSION_MAJOR@)
SET (ZLIB_VERSION_MINOR  @ZLIB_PACKAGE_VERSION_MINOR@)

CONFIGURE_FILE (
    ${HDF_RESOURCES_DIR}/ZLIB-config.cmake.build.in 
    ${ZLIB_BINARY_DIR}/${ZLIB_PACKAGE}${HDF_PACKAGE_EXT}-config.cmake @ONLY
)

#-----------------------------------------------------------------------------
# Configure the ZLIB-config.cmake file for the install directory
#-----------------------------------------------------------------------------
IF (NOT ZLIB_EXTERNALLY_CONFIGURED)
  CONFIGURE_FILE (
      ${HDF_RESOURCES_DIR}/ZLIB-config.cmake.install.in
      ${ZLIB_BINARY_DIR}/CMakeFiles/${ZLIB_PACKAGE}${HDF_PACKAGE_EXT}-config.cmake @ONLY
  )
  INSTALL (
      FILES ${ZLIB_BINARY_DIR}/CMakeFiles/${ZLIB_PACKAGE}${HDF_PACKAGE_EXT}-config.cmake
      DESTINATION ${ZLIB_INSTALL_CMAKE_DIR}/ZLIB
      COMPONENT configinstall
  )
ENDIF (NOT ZLIB_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Configure the ZLIB-config-version.cmake file for the install directory
#-----------------------------------------------------------------------------
IF (NOT ZLIB_EXTERNALLY_CONFIGURED)
  CONFIGURE_FILE (
      ${HDF_RESOURCES_DIR}/ZLIB-config-version.cmake.in
      ${ZLIB_BINARY_DIR}/CMakeFiles/${ZLIB_PACKAGE}${HDF_PACKAGE_EXT}-config-version.cmake @ONLY
  )
  INSTALL (
      FILES ${ZLIB_BINARY_DIR}/CMakeFiles/${ZLIB_PACKAGE}${HDF_PACKAGE_EXT}-config-version.cmake
      DESTINATION ${ZLIB_INSTALL_CMAKE_DIR}/ZLIB
      COMPONENT configinstall
  )
ENDIF (NOT ZLIB_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Add Document File(s) to CMake Install
#-----------------------------------------------------------------------------
IF (NOT ZLIB_EXTERNALLY_CONFIGURED)
  INSTALL (
      FILES
          ${ZLIB_SOURCE_DIR}/FAQ
          ${ZLIB_SOURCE_DIR}/README
          ${ZLIB_SOURCE_DIR}/INDEX
          ${ZLIB_SOURCE_DIR}/RELEASE_HDF.txt
      DESTINATION ${ZLIB_INSTALL_DATA_DIR}
      COMPONENT hdfdocuments
  )
ENDIF (NOT ZLIB_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Set the cpack variables
#-----------------------------------------------------------------------------
IF (NOT ZLIB_EXTERNALLY_CONFIGURED)
  SET (CPACK_PACKAGE_VENDOR "HDF_Group")
  SET (CPACK_PACKAGE_NAME "${ZLIB_PACKAGE_NAME}")
  SET (CPACK_PACKAGE_INSTALL_DIRECTORY "${ZLIB_PACKAGE_NAME}")
  SET (CPACK_PACKAGE_VERSION "${ZLIB_PACKAGE_VERSION}")
  SET (CPACK_PACKAGE_VERSION_MAJOR "${ZLIB_PACKAGE_VERSION_MAJOR}")
  SET (CPACK_PACKAGE_VERSION_MINOR "${ZLIB_PACKAGE_VERSION_MINOR}")
  SET (CPACK_PACKAGE_VERSION_PATCH "")
  SET (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/RELEASE_HDF.txt")
  SET (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/README")
  SET (CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/RELEASE_HDF.txt")
  SET (CPACK_PACKAGE_RELOCATABLE TRUE)
  IF (WIN32)
    SET (CPACK_MONOLITHIC_INSTALL ON)
    SET (CPACK_NSIS_CONTACT "help@hdfgroup.org")
    SET (CPACK_NSIS_MODIFY_PATH ON)
    SET (CPACK_NSIS_PACKAGE_NAME "ZLIB ${ZLIB_PACKAGE_VERSION}")
  ELSE (WIN32)
    SET (CPACK_PACKAGING_INSTALL_PREFIX "/usr")
    SET (CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)

    SET (CPACK_DEBIAN_PACKAGE_SECTION "Libraries")
    SET (CPACK_DEBIAN_PACKAGE_MAINTAINER "${ZLIB_PACKAGE_BUGREPORT}")
    
    SET (CPACK_RPM_COMPONENT_INSTALL ON)
    SET (CPACK_RPM_PACKAGE_RELOCATABLE ON)
  ENDIF (WIN32)
  
  INCLUDE(InstallRequiredSystemLibraries)

  SET (CPACK_INSTALL_CMAKE_PROJECTS "${ZLIB_BINARY_DIR};ZLIB;ALL;/")
  
  INCLUDE (CPack)

  CPACK_ADD_COMPONENT (libraries 
      DISPLAY_NAME "ZLIB Libraries"
      GROUP Runtime
  )
  CPACK_ADD_COMPONENT (headers 
      DISPLAY_NAME "ZLIB Headers" 
      DEPENDS libraries
      GROUP Development
  )
  CPACK_ADD_COMPONENT (hdfdocuments 
      DISPLAY_NAME "ZLIB Documents"
      GROUP Documents
  )
  CPACK_ADD_COMPONENT (configinstall 
      DISPLAY_NAME "ZLIB CMake files" 
      DEPENDS libraries
      GROUP Development
  )
  
ENDIF (NOT ZLIB_EXTERNALLY_CONFIGURED)
  