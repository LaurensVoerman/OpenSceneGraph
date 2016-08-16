IF  (DCMTK_FOUND)

    # note, we have to include a '/' in front of the directory string to prevent a CMake bug from ignoring the directory
    INCLUDE_DIRECTORIES(${DCMTK_ROOT_INCLUDE_DIR})

    SET(TARGET_SRC ReaderWriterDICOM.cpp )

#    LINK_LIBRARIES(${DCMTK_LIBRARIES} ${ZLIB_LIBRARY})
# link debug versions with d postfix
#    SET(TARGET_LIBRARIES_VARS ${DCMTK_LIBRARIES} ${ZLIB_LIBRARY}) <- this won't work, need the variables, not the values

    SET(TARGET_LIBRARIES_VARS DCMTK_dcmdata_LIBRARY DCMTK_dcmimage_LIBRARY DCMTK_dcmimgle_LIBRARY DCMTK_dcmnet_LIBRARY DCMTK_oflog_LIBRARY DCMTK_ofstd_LIBRARY ZLIB_LIBRARY)

    ADD_DEFINITIONS(-DUSE_DCMTK)

    IF (WIN32)
        SET(TARGET_EXTERNAL_LIBRARIES wsock32.lib)
    ENDIF()

ELSEIF(ITK_FOUND)

    INCLUDE(${ITK_USE_FILE})

    SET(TARGET_SRC ReaderWriterDICOM.cpp )

    LINK_LIBRARIES(ITKCommon ITKIO)

    ADD_DEFINITIONS(-DUSE_ITK)
    ADD_DEFINITIONS(-DVCL_CAN_STATIC_CONST_INIT_FLOAT=0)

ENDIF()

SET(TARGET_ADDED_LIBRARIES osgVolume )

IF(CMAKE_COMPILER_IS_GNUCXX)
    # Remove -pedantic flag as it barfs on ffmoeg headers
   STRING(REGEX REPLACE "-pedantic" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
ENDIF()

#### end var setup  ###
SETUP_PLUGIN(dicom dicom)

IF  (DCMTK_FOUND)
        IF(MSVC)
            #msvc complains about multiple defined symbols - link anyway
            set_target_properties(${TARGET_TARGETNAME} PROPERTIES LINK_FLAGS "/FORCE:MULTIPLE" )
        ENDIF(MSVC)
ENDIF  (DCMTK_FOUND)
