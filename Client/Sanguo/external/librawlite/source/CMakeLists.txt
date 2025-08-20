cmake_minimum_required (VERSION 3.1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/utils.cmake)

project (static_rawlite)

set(ALL_FILES
internal/dcraw_common.cpp
internal/dcraw_fileio.cpp
internal/defines.h
internal/demosaic_packs.cpp
internal/libraw_internal_funcs.h
internal/var_defines.h
libraw/libraw.h
libraw/libraw_alloc.h
libraw/libraw_c_api.cpp
libraw/libraw_const.h
libraw/libraw_cxx.cpp
libraw/libraw_datastream.cpp
libraw/libraw_datastream.h
libraw/libraw_internal.h
libraw/libraw_types.h
libraw/libraw_version.h
)

group_sources("${ALL_FILES}")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLIBRAW_NODLL")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLIBRAW_NODLL")
elseif (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNO_LCMS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DNO_LCMS")
endif ()

add_library(${PROJECT_NAME} ${ALL_FILES})

# Creates folder "external" and adds target project
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER external)
