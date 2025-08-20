cmake_minimum_required (VERSION 3.1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/utils.cmake)

project (static_asyn-script)

include_directories(${PROJECT_SOURCE_DIR})


set(ALL_FILES
AsynScript.h
AsysValue.h
AsysValueT.h
AsysVariable.cpp
AsysVariable.h
AsysVariableT.h
Debug.cpp
Debug.h
Define.h
Function.cpp
Function.h
Instructions.cpp
Instructions.h
Machine.cpp
Machine.h
Stack.cpp
Stack.h
)

group_sources("${ALL_FILES}")

add_definitions(-D OPJ_STATIC)
add_definitions(-D LIBRAW_NODLL)
add_definitions(-D FREEIMAGE_LIB)

add_library(${PROJECT_NAME} ${ALL_FILES})

# Creates folder "external" and adds target project
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER external)
