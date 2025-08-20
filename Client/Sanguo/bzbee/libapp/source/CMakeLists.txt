cmake_minimum_required (VERSION 3.1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/utils.cmake)

project (static_app)

include_directories(${PROJECT_SOURCE_DIR}/../../libbaseutil/source)
include_directories(${PROJECT_SOURCE_DIR}/../../libfont/source)
include_directories(${PROJECT_SOURCE_DIR}/../../libframework/source)
include_directories(${PROJECT_SOURCE_DIR}/../../libmodel/source)
include_directories(${PROJECT_SOURCE_DIR}/../../libstyle/source)
include_directories(${PROJECT_SOURCE_DIR}/../../librender/source)
include_directories(${PROJECT_SOURCE_DIR}/../../libscene/source)
include_directories(${PROJECT_SOURCE_DIR}/../../libui/source)
include_directories(${PROJECT_SOURCE_DIR}/../../libspine_render/source)
include_directories(${PROJECT_SOURCE_DIR}/../../libmagic_particle_render/source)
include_directories(${PROJECT_SOURCE_DIR}/../../libaudio/source)
include_directories(${PROJECT_SOURCE_DIR}/../../libnetwork/source)

set(ALL_FILES
AppModule.cpp
AppModule.h
)

group_sources("${ALL_FILES}")

add_library(${PROJECT_NAME} ${ALL_FILES})

# Creates folder "bzbee" and adds target project
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER bzbee)
