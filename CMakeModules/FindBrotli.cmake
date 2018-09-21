# Finds Brotli header and library
#
# This script defines the following:
#  BROTLI_FOUND // Set to TRUE if Brotli is found
#  BROTLI_INCLUDE_DIR // Parent directory of directory brotli/decode.h header.
#
# EGL_DIR can be set as an environment variable or a CMake variable,
# to the parent directory of the EGL header.
#


FIND_PATH( BROTLI_INCLUDE_DIR
    NAMES brotli/decode.h
    HINTS ENV BROTLI_DIR
    PATH_SUFFIXES include
)

FIND_LIBRARY(BROTLI_COMMON_LIBRARY
    NAMES brotlicommon-static
    HINTS ENV BROTLI_DIR
    PATH_SUFFIXES lib
)
FIND_LIBRARY(BROTLI_ENC_LIBRARY
    NAMES brotlienc-static
    HINTS ENV BROTLI_DIR
    PATH_SUFFIXES lib
)
FIND_LIBRARY(BROTLI_DEC_LIBRARY
    NAMES brotlidec-static
    HINTS ENV BROTLI_DIR
    PATH_SUFFIXES lib
)

if(BROTLI_COMMON_LIBRARY AND BROTLI_ENC_LIBRARY AND BROTLI_DEC_LIBRARY)
    set( BROTLI_LIBRARIES "${BROTLI_COMMON_LIBRARY};${BROTLI_ENC_LIBRARY};${BROTLI_DEC_LIBRARY}" CACHE STRING "Brotli Libraries")
endif()

# handle the QUIETLY and REQUIRED arguments and set
# EGL_FOUND to TRUE as appropriate
INCLUDE( FindPackageHandleStandardArgs )

FIND_PACKAGE_HANDLE_STANDARD_ARGS(BROTLI
                                  REQUIRED_VARS BROTLI_LIBRARIES BROTLI_INCLUDE_DIR)


