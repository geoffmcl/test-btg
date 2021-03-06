#-------------------------------------------------------------------------------
MACRO (SET_GLOBAL_VARIABLE name value)
  SET (${name} ${value} CACHE INTERNAL "Used to pass variables between directories" FORCE)
ENDMACRO (SET_GLOBAL_VARIABLE)

#-------------------------------------------------------------------------------
MACRO (IDE_GENERATED_PROPERTIES SOURCE_PATH HEADERS SOURCES)
  #set(source_group_path "Source/AIM/${NAME}")
  STRING (REPLACE "/" "\\\\" source_group_path ${SOURCE_PATH})
  source_group (${source_group_path} FILES ${HEADERS} ${SOURCES})

  #-- The following is needed if we ever start to use OS X Frameworks but only
  #--  works on CMake 2.6 and greater
  #SET_PROPERTY (SOURCE ${HEADERS}
  #       PROPERTY MACOSX_PACKAGE_LOCATION Headers/${NAME}
  #)
ENDMACRO (IDE_GENERATED_PROPERTIES)

#-------------------------------------------------------------------------------
MACRO (IDE_SOURCE_PROPERTIES SOURCE_PATH HEADERS SOURCES)
  #  INSTALL (FILES ${HEADERS}
  #       DESTINATION include/R3D/${NAME}
  #       COMPONENT Headers       
  #  )

  STRING (REPLACE "/" "\\\\" source_group_path ${SOURCE_PATH}  )
  source_group (${source_group_path} FILES ${HEADERS} ${SOURCES})

  #-- The following is needed if we ever start to use OS X Frameworks but only
  #--  works on CMake 2.6 and greater
  #SET_PROPERTY (SOURCE ${HEADERS}
  #       PROPERTY MACOSX_PACKAGE_LOCATION Headers/${NAME}
  #)
ENDMACRO (IDE_SOURCE_PROPERTIES)

#-------------------------------------------------------------------------------
MACRO (TARGET_NAMING libtarget libtype)
  IF (WIN32)
    IF (${libtype} MATCHES "SHARED")
      SET_TARGET_PROPERTIES (${libtarget} PROPERTIES OUTPUT_NAME "${libtarget}dll")
    ENDIF (${libtype} MATCHES "SHARED")
  ENDIF (WIN32)
ENDMACRO (TARGET_NAMING)

#-------------------------------------------------------------------------------
MACRO (INSTALL_TARGET_PDB libtarget targetdestination targetcomponent)
  IF (WIN32 AND MSVC)
    GET_TARGET_PROPERTY (target_name ${libtarget} RELWITHDEBINFO_OUTPUT_NAME)
    INSTALL (
      FILES
          ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/${CMAKE_IMPORT_LIBRARY_PREFIX}${target_name}.pdb
      DESTINATION
          ${targetdestination}
      CONFIGURATIONS RelWithDebInfo
      COMPONENT ${targetcomponent}
  )
  ENDIF (WIN32 AND MSVC)
ENDMACRO (INSTALL_TARGET_PDB)

#-------------------------------------------------------------------------------
MACRO (INSTALL_PROGRAM_PDB progtarget targetdestination targetcomponent)
  IF (WIN32 AND MSVC)
    GET_TARGET_PROPERTY (target_name ${progtarget} RELWITHDEBINFO_OUTPUT_NAME)
    GET_TARGET_PROPERTY (target_prefix ${progtarget} PREFIX)
    INSTALL (
      FILES
          ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/${target_prefix}${target_name}.pdb
      DESTINATION
          ${targetdestination}
      CONFIGURATIONS RelWithDebInfo
      COMPONENT ${targetcomponent}
  )
  ENDIF (WIN32 AND MSVC)
ENDMACRO (INSTALL_PROGRAM_PDB)

#-------------------------------------------------------------------------------
MACRO (HDF_SET_LIB_OPTIONS libtarget libname libtype)
  # message (STATUS "${libname} libtype: ${libtype}")
  IF (${libtype} MATCHES "SHARED")
    IF (WIN32)
      SET (LIB_RELEASE_NAME "${libname}")
      SET (LIB_DEBUG_NAME "${libname}_D")
    ELSE (WIN32)
      SET (LIB_RELEASE_NAME "${libname}")
      SET (LIB_DEBUG_NAME "${libname}_debug")
    ENDIF (WIN32)
  ELSE (${libtype} MATCHES "SHARED")
    IF (WIN32)
      SET (LIB_RELEASE_NAME "lib${libname}")
      SET (LIB_DEBUG_NAME "lib${libname}_D")
    ELSE (WIN32)
      # if the generator supports configuration types or if the CMAKE_BUILD_TYPE has a value
      IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
        SET (LIB_RELEASE_NAME "${libname}")
        SET (LIB_DEBUG_NAME "${libname}_debug")
      ELSE (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
        SET (LIB_RELEASE_NAME "lib${libname}")
        SET (LIB_DEBUG_NAME "lib${libname}_debug")
      ENDIF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
    ENDIF (WIN32)
  ENDIF (${libtype} MATCHES "SHARED")
  
  SET_TARGET_PROPERTIES (${libtarget}
      PROPERTIES
      DEBUG_OUTPUT_NAME          ${LIB_DEBUG_NAME}
      RELEASE_OUTPUT_NAME        ${LIB_RELEASE_NAME}
      MINSIZEREL_OUTPUT_NAME     ${LIB_RELEASE_NAME}
      RELWITHDEBINFO_OUTPUT_NAME ${LIB_RELEASE_NAME}
  )
  
  #----- Use MSVC Naming conventions for Shared Libraries
  IF (MINGW AND ${libtype} MATCHES "SHARED")
    SET_TARGET_PROPERTIES (${libtarget}
        PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
    )
  ENDIF (MINGW AND ${libtype} MATCHES "SHARED")

ENDMACRO (HDF_SET_LIB_OPTIONS)

#-------------------------------------------------------------------------------
MACRO (TARGET_C_PROPERTIES wintarget addcompileflags addlinkflags)
  IF (MSVC)
    TARGET_MSVC_PROPERTIES (${wintarget} "${addcompileflags} ${WIN_COMPILE_FLAGS}" "${addlinkflags} ${WIN_LINK_FLAGS}")
  ELSE (MSVC)
    IF (BUILD_SHARED_LIBS)
      SET_TARGET_PROPERTIES (${wintarget}
          PROPERTIES
              COMPILE_FLAGS "${addcompileflags}"
              LINK_FLAGS "${addlinkflags}"
      ) 
    ELSE (BUILD_SHARED_LIBS)
      SET_TARGET_PROPERTIES (${wintarget}
          PROPERTIES
              COMPILE_FLAGS "${addcompileflags}"
              LINK_FLAGS "${addlinkflags}"
      ) 
    ENDIF (BUILD_SHARED_LIBS)
  ENDIF (MSVC)
ENDMACRO (TARGET_C_PROPERTIES)

#-------------------------------------------------------------------------------
MACRO (TARGET_MSVC_PROPERTIES wintarget addcompileflags addlinkflags)
  IF (MSVC)
    IF (BUILD_SHARED_LIBS)
      SET_TARGET_PROPERTIES (${wintarget}
          PROPERTIES
              COMPILE_FLAGS "${addcompileflags}"
              LINK_FLAGS "${addlinkflags}"
      ) 
    ELSE (BUILD_SHARED_LIBS)
      SET_TARGET_PROPERTIES (${wintarget}
          PROPERTIES
              COMPILE_FLAGS "${addcompileflags}"
              LINK_FLAGS "${addlinkflags}"
      ) 
    ENDIF (BUILD_SHARED_LIBS)
  ENDIF (MSVC)
ENDMACRO (TARGET_MSVC_PROPERTIES)

#-------------------------------------------------------------------------------
MACRO (TARGET_FORTRAN_PROPERTIES forttarget addcompileflags addlinkflags)
  IF (WIN32)
    TARGET_FORTRAN_WIN_PROPERTIES (${forttarget} "${addcompileflags} ${WIN_COMPILE_FLAGS}" "${addlinkflags} ${WIN_LINK_FLAGS}")
  ENDIF (WIN32)
ENDMACRO (TARGET_FORTRAN_PROPERTIES)

#-------------------------------------------------------------------------------
MACRO (TARGET_FORTRAN_WIN_PROPERTIES forttarget addcompileflags addlinkflags)
  IF (MSVC)
    IF (BUILD_SHARED_LIBS)
      SET_TARGET_PROPERTIES (${forttarget}
          PROPERTIES
              COMPILE_FLAGS "/dll ${addcompileflags}"
              LINK_FLAGS "/SUBSYSTEM:CONSOLE ${addlinkflags}"
      ) 
    ELSE (BUILD_SHARED_LIBS)
      SET_TARGET_PROPERTIES (${forttarget}
          PROPERTIES
              COMPILE_FLAGS "${addcompileflags}"
              LINK_FLAGS "/SUBSYSTEM:CONSOLE ${addlinkflags}"
      ) 
    ENDIF (BUILD_SHARED_LIBS)
  ENDIF (MSVC)
ENDMACRO (TARGET_FORTRAN_WIN_PROPERTIES)
