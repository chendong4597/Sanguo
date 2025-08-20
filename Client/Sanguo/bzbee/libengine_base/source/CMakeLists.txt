cmake_minimum_required (VERSION 3.1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/utils.cmake)

project (static_engine_base)

include_directories(${PROJECT_SOURCE_DIR}/../../libbaseutil/source)

set(ALL_FILES
RichTextUtil.cpp
RichTextUtil.h
)

group_sources("${ALL_FILES}")

add_library(${PROJECT_NAME} ${ALL_FILES})

# Creates folder "bzbee" and adds target project
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER bzbee)
