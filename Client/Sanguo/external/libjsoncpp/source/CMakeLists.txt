cmake_minimum_required (VERSION 3.1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/utils.cmake)

project (static_jsoncpp)

set(ALL_FILES
json/config.h
json/features.h
json/forwards.h
json/json_batchallocator.h
json/json_internalarray.inl
json/json_internalmap.inl
json/json_lib.h
json/json_reader.cpp
json/json_tool.h
json/json_value.cpp
json/json_valueiterator.inl
json/json_writer.cpp
json/reader.h
json/value.h
json/writer.h
)

group_sources("${ALL_FILES}")

add_library(${PROJECT_NAME} ${ALL_FILES})

# Creates folder "external" and adds target project
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER external)
