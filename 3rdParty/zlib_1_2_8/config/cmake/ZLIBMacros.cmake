#-------------------------------------------------------------------------------
MACRO (ZLIB_SET_LIB_OPTIONS libtarget defaultlibname libtype)
  SET (libname "${defaultlibname}")

  IF (NOT WIN32)
    # On unix-like platforms the library is almost always called libz
    SET (libname "z")
    SET_TARGET_PROPERTIES (${libtarget} PROPERTIES OUTPUT_NAME z)
  ENDIF (NOT WIN32)

  # message (STATUS "${libname} libtype: ${libtype}")
  HDF_SET_LIB_OPTIONS (${libtarget} ${libname} ${libtype})

  IF (${libtype} MATCHES "SHARED")
    IF (WIN32)
      SET (LIBZLIB_VERSION ${ZLIB_PACKAGE_VERSION_MAJOR})
    ELSE (WIN32)
      SET (LIBZLIB_VERSION ${ZLIB_PACKAGE_VERSION})
    ENDIF (WIN32)

    IF (NOT CYGWIN)
      # This property causes shared libraries on Linux to have the full version
      # encoded into their final filename.  We disable this on Cygwin because
      # it causes cygz-${ZLIB_FULL_VERSION}.dll to be created when cygz.dll
      # seems to be the default.
      #
      # This has no effect with MSVC, on that platform the version info for
      # the DLL comes from the resource file win32/zlib1.rc
      SET_TARGET_PROPERTIES (${libtarget} PROPERTIES VERSION ${LIBZLIB_VERSION})
    ENDIF (NOT CYGWIN)
    SET_TARGET_PROPERTIES (${libtarget} PROPERTIES SOVERSION ${LIBZLIB_VERSION})
  ENDIF (${libtype} MATCHES "SHARED")

  #-- Apple Specific install_name for libraries
  IF (APPLE)
    OPTION (ZLIB_BUILD_WITH_INSTALL_NAME "Build with library install_name set to the installation path" OFF)
    IF (ZLIB_BUILD_WITH_INSTALL_NAME)
      SET_TARGET_PROPERTIES(${libtarget} PROPERTIES
          LINK_FLAGS "-current_version ${ZLIB_PACKAGE_VERSION} -compatibility_version ${ZLIB_PACKAGE_VERSION}"
          INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
          BUILD_WITH_INSTALL_RPATH ${ZLIB_BUILD_WITH_INSTALL_NAME}
      )
    ENDIF (ZLIB_BUILD_WITH_INSTALL_NAME)
  ENDIF (APPLE)

ENDMACRO (ZLIB_SET_LIB_OPTIONS)

