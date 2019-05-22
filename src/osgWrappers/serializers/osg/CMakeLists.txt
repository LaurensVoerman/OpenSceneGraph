FILE(GLOB TARGET_SRC *.cpp)
FILE(GLOB TARGET_H *.h)

SET(DO_NOT_CONCAT_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/AnimationPath.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AnimationPathCallback.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Callback.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ClusterCullingCallback.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NodeTrackerCallback.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NodeVisitor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ScriptNodeCallback.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StateSet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Uniform.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LibraryWrapper.cpp
)
OSG_COMPILE_CONCAT_GLOB_FIXMYCLASS(TARGET_SRC "${DO_NOT_CONCAT_SRC}")
#### end var setup  ###
SETUP_PLUGIN(osg)
