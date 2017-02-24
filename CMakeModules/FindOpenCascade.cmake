# LocateOPENCASCADE
# This module defines
# OPENCASCADE_LIBRARY
# OPENCASCADE_FOUND, if false, do not try to link to OPENCASCADE
# OPENCASCADE_INCLUDE_DIR, where to find the headers
#
# $OPENCASCADE_DIR is an environment variable that would
# correspond to the ./configure --prefix=$OPENCASCADE_DIR
# used in building OPENCASCADE.

FIND_PATH(OPENCASCADE_INCLUDE_DIR Standard_Version.hxx
  PATHS
    ${OPENCASCADE_DIR}
    $ENV{OPENCASCADE_DIR}
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local
    /usr
    /sw # Fink
    /opt/local # DarwinPorts
    /opt/csw # Blastwave
    /opt
    /usr/freeware
  PATH_SUFFIXES
    opencascade
    inc
    include
    inc/cascade
    include/cascade
)

MACRO(FIND_OPENCASCADE_LIBRARY MYLIBRARY MYLIBRARYNAME)

    FIND_LIBRARY("${MYLIBRARY}"
        NAMES "${MYLIBRARYNAME}"
        PATHS
        $ENV{OPENCASCADE_DIR}/lib}
        $ENV{OPENCASCADE_LIB}
        ${OPENCASCADE_DIR}/lib
        $ENV{OPENCASCADE_DIR}/lib
        ${OPENCASCADE_DIR}/lib/
        ~/Library/Frameworks
        /Library/Frameworks
        /usr/local/lib
        /usr/lib
        /sw/lib
        /opt/local/lib
        /opt/csw/lib
        /opt/lib
        /usr/freeware/lib64
        )
    FIND_LIBRARY("${MYLIBRARY}_DEBUG"
        NAMES "${MYLIBRARYNAME}"
        PATHS
        $ENV{OPENCASCADE_DIR}/libd}
        $ENV{OPENCASCADE_LIB}d
        ${OPENCASCADE_DIR}/libd
        $ENV{OPENCASCADE_DIR}/libd
        ${OPENCASCADE_DIR}/libd/
        /usr/local/libd
        /usr/libd
        /sw/libd
        /opt/local/libd
        /opt/csw/libd
        /opt/libd
        /usr/freeware/lib64d
        )

ENDMACRO()



FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKBinL TKBinL)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKBin TKBin)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKBinTObj TKBinTObj)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKBinXCAF TKBinXCAF)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKBool TKBool)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKBO TKBO)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKBRep TKBRep)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKCAF TKCAF)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKCDF TKCDF)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKDCAF TKDCAF)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKernel TKernel)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKFeat TKFeat)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKFillet TKFillet)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKG2d TKG2d)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKG3d TKG3d)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKGeomAlgo TKGeomAlgo)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKGeomBase TKGeomBase)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKHLR TKHLR)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKIGES TKIGES)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKLCAF TKLCAF)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKMath TKMath)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKMesh TKMesh)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKMeshVS TKMeshVS)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKOffset TKOffset)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKOpenGl TKOpenGl)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKPrim TKPrim)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKService TKService)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKShHealing TKShHealing)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKSTEP209 TKSTEP209)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKSTEPAttr TKSTEPAttr)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKSTEPBase TKSTEPBase)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKSTEP TKSTEP)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKSTL TKSTL)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKTObj TKTObj)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKTopAlgo TKTopAlgo)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKV3d TKV3d)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKVCAF TKVCAF)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKVRML TKVRML)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKXCAF TKXCAF)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKXDEIGES TKXDEIGES)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKXDESTEP TKXDESTEP)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKXMesh TKXMesh)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKXmlL TKXmlL)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKXml TKXml)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKXmlTObj TKXmlTObj)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKXmlXCAF TKXmlXCAF)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKXSBase TKXSBase)
FIND_OPENCASCADE_LIBRARY(OPENCASCADE_TKXCAF TKXCAF)

set (OPENCASCADE_LIBRARY_TYPE_DESCR 
"Specifies the type of library to be used. 'Shared' libraries
are linked dynamically and loaded at runtime. 'Static' libraries
are archives of object files for use when linking other targets")

# set type of OCCT libraries
if (NOT OPENCASCADE_LIBRARY_TYPE)
  set (OPENCASCADE_LIBRARY_TYPE "Static" CACHE STRING "${OPENCASCADE_LIBRARY_TYPE_DESCR}" FORCE)
  SET_PROPERTY(CACHE OPENCASCADE_LIBRARY_TYPE PROPERTY STRINGS Shared Static)
endif()

if ("${OPENCASCADE_LIBRARY_TYPE}" STREQUAL "Shared")

SET(OPENCASCADE_LIBRARY ${OPENCASCADE_LIBRARY}
    ${OPENCASCADE_TKernel}
    ${OPENCASCADE_TKMath}
    ${OPENCASCADE_TKXCAF}
    ${OPENCASCADE_TKXSBase}
    ${OPENCASCADE_TKSTEPAttr}
    ${OPENCASCADE_TKSTEPBase}
    ${OPENCASCADE_TKSTEP209}
    ${OPENCASCADE_TKSTEP}
    ${OPENCASCADE_TKXDESTEP}
    ${OPENCASCADE_TKIGES}
    ${OPENCASCADE_TKXDEIGES}
    ${OPENCASCADE_TKLCAF}
    ${OPENCASCADE_TKBRep}
    ${OPENCASCADE_TKMesh}
    ${OPENCASCADE_TKTopAlgo}
    ${OPENCASCADE_TKShHealing}
)
SET(OPENCASCADE_LIBRARY_DEBUG ${OPENCASCADE_LIBRARY_DEBUG}
    ${OPENCASCADE_TKernel_DEBUG}
    ${OPENCASCADE_TKMath_DEBUG}
    ${OPENCASCADE_TKXCAF_DEBUG}
    ${OPENCASCADE_TKXSBase_DEBUG}
    ${OPENCASCADE_TKSTEPAttr_DEBUG}
    ${OPENCASCADE_TKSTEPBase_DEBUG}
    ${OPENCASCADE_TKSTEP209_DEBUG}
    ${OPENCASCADE_TKSTEP_DEBUG}
    ${OPENCASCADE_TKXDESTEP_DEBUG}
    ${OPENCASCADE_TKIGES_DEBUG}
    ${OPENCASCADE_TKXDEIGES_DEBUG}
    ${OPENCASCADE_TKLCAF_DEBUG}
    ${OPENCASCADE_TKBRep_DEBUG}
    ${OPENCASCADE_TKMesh_DEBUG}
    ${OPENCASCADE_TKTopAlgo_DEBUG}
    ${OPENCASCADE_TKShHealing_DEBUG}
)
else ("${OPENCASCADE_LIBRARY_TYPE}" STREQUAL "Shared")
SET(OPENCASCADE_LIBRARY ${OPENCASCADE_LIBRARY}
    ${OPENCASCADE_TKBO}
    ${OPENCASCADE_TKBool}
    ${OPENCASCADE_TKBRep}
    ${OPENCASCADE_TKCAF}
    ${OPENCASCADE_TKCDF}
    ${OPENCASCADE_TKDCAF}
    ${OPENCASCADE_TKernel}
    ${OPENCASCADE_TKG2d}
    ${OPENCASCADE_TKG3d}
    ${OPENCASCADE_TKGeomAlgo}
    ${OPENCASCADE_TKGeomBase}
    ${OPENCASCADE_TKHLR}
    ${OPENCASCADE_TKIGES}
    ${OPENCASCADE_TKLCAF}
    ${OPENCASCADE_TKMath}
    ${OPENCASCADE_TKMesh}
    ${OPENCASCADE_TKPrim}
    ${OPENCASCADE_TKService}
    ${OPENCASCADE_TKShHealing}
    ${OPENCASCADE_TKSTEP}
    ${OPENCASCADE_TKSTEP209}
    ${OPENCASCADE_TKSTEPAttr}
    ${OPENCASCADE_TKSTEPBase}
    ${OPENCASCADE_TKTopAlgo}
    ${OPENCASCADE_TKV3d}
    ${OPENCASCADE_TKVCAF}
    ${OPENCASCADE_TKXCAF}
    ${OPENCASCADE_TKXDEIGES}
    ${OPENCASCADE_TKXDESTEP}
    ${OPENCASCADE_TKXSBase}
)
SET(OPENCASCADE_LIBRARY_DEBUG ${OPENCASCADE_LIBRARY_DEBUG}
    ${OPENCASCADE_TKBO_DEBUG}
    ${OPENCASCADE_TKBool_DEBUG}
    ${OPENCASCADE_TKBRep_DEBUG}
    ${OPENCASCADE_TKCAF_DEBUG}
    ${OPENCASCADE_TKCDF_DEBUG}
    ${OPENCASCADE_TKDCAF_DEBUG}
    ${OPENCASCADE_TKernel_DEBUG}
    ${OPENCASCADE_TKG2d_DEBUG}
    ${OPENCASCADE_TKG3d_DEBUG}
    ${OPENCASCADE_TKGeomAlgo_DEBUG}
    ${OPENCASCADE_TKGeomBase_DEBUG}
    ${OPENCASCADE_TKHLR_DEBUG}
    ${OPENCASCADE_TKIGES_DEBUG}
    ${OPENCASCADE_TKLCAF_DEBUG}
    ${OPENCASCADE_TKMath_DEBUG}
    ${OPENCASCADE_TKMesh_DEBUG}
    ${OPENCASCADE_TKPrim_DEBUG}
    ${OPENCASCADE_TKService_DEBUG}
    ${OPENCASCADE_TKShHealing_DEBUG}
    ${OPENCASCADE_TKSTEP_DEBUG}
    ${OPENCASCADE_TKSTEP209_DEBUG}
    ${OPENCASCADE_TKSTEPAttr_DEBUG}
    ${OPENCASCADE_TKSTEPBase_DEBUG}
    ${OPENCASCADE_TKTopAlgo_DEBUG}
    ${OPENCASCADE_TKV3d_DEBUG}
    ${OPENCASCADE_TKVCAF_DEBUG}
    ${OPENCASCADE_TKXCAF_DEBUG}
    ${OPENCASCADE_TKXDEIGES_DEBUG}
    ${OPENCASCADE_TKXDESTEP_DEBUG}
    ${OPENCASCADE_TKXSBase_DEBUG}
)
endif ("${OPENCASCADE_LIBRARY_TYPE}" STREQUAL "Shared")

SET(OPENCASCADE_FOUND "NO")
IF(OPENCASCADE_LIBRARY AND OPENCASCADE_INCLUDE_DIR)
    SET(OPENCASCADE_FOUND "YES")
ENDIF(OPENCASCADE_LIBRARY AND OPENCASCADE_INCLUDE_DIR)

IF(OPENCASCADE_INCLUDE_DIR)
    SET(OPENCASCADE_FOUND "YES")
ENDIF(OPENCASCADE_INCLUDE_DIR)
