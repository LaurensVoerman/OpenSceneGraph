# Locate directinput
# This module defines
# DIRECTINPUT_LIBRARIES
# DIRECTINPUT_FOUND, if false, do not try to link to directinput
# DIRECTINPUT_INCLUDE_DIR, where to find the headers
#
# $DIRECTINPUT_DIR is an environment variable that would
# point to the this path in the plateform devkit (Samples\Multimedia\DirectShow)
#
# Created by Cedric Pinson.
#

SET( DIRECTINPUT_FOUND FALSE )

IF( WIN32 )
    set(PFX86 "PROGRAMFILES(X86)") 
    FIND_PATH( DIRECTINPUT_ROOT_DIR Include/D3D10.h
               PATHS
               $ENV{PATH}
               $ENV{PROGRAMFILES}
               "$ENV{${PFX86}}/Windows Kits/8.1"
               "$ENV{ProgramFiles}/Windows Kits/8.1"
               PATH_SUFFIXES "Include" "Include/um"
    )
    
    FIND_PATH( DIRECTINPUT_INCLUDE_DIR dinput.h
               PATHS
               ${DIRECTINPUT_ROOT_DIR}
               PATH_SUFFIXES "Include" "Include/um"
    )
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(DIRECTINPUT_LIBPATHSUFFIX /lib/x64 /Lib/winv6.3/um/x64)
ELSE()
    SET(DIRECTINPUT_LIBPATHSUFFIX /lib/x86 /Lib/winv6.3/um/x86)
ENDIF()    
    FIND_LIBRARY( DIRECTINPUT_LIBRARY dinput7.lib dinput8.lib
                  PATHS
                  ${DIRECTINPUT_ROOT_DIR}
                  PATH_SUFFIXES ${DIRECTINPUT_LIBPATHSUFFIX}
    )
    
    FIND_LIBRARY( DIRECTINPUT_GUID_LIBRARY dxguid.lib
                  PATHS
                  ${DIRECTINPUT_ROOT_DIR}
                  PATH_SUFFIXES ${DIRECTINPUT_LIBPATHSUFFIX}
    )
    
    FIND_LIBRARY( DIRECTINPUT_ERR_LIBRARY dxerr.lib
                  PATHS
                  ${DIRECTINPUT_ROOT_DIR}
                  PATH_SUFFIXES ${DIRECTINPUT_LIBPATHSUFFIX}
    )
    
    SET( DIRECTINPUT_LIBRARIES
         ${DIRECTINPUT_LIBRARY}
         ${DIRECTINPUT_GUID_LIBRARY}
#         ${DIRECTINPUT_ERR_LIBRARY}
    )
    
    IF ( DIRECTINPUT_INCLUDE_DIR AND DIRECTINPUT_LIBRARIES )
        SET( DIRECTINPUT_FOUND TRUE )
    ENDIF ( DIRECTINPUT_INCLUDE_DIR AND DIRECTINPUT_LIBRARIES )
ENDIF( WIN32 )

MARK_AS_ADVANCED( DIRECTINPUT_FOUND )
