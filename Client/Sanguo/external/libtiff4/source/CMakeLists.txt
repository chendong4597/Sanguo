cmake_minimum_required (VERSION 3.1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/utils.cmake)

project (static_tiff4)

include_directories(${PROJECT_SOURCE_DIR}/../../libjpeg/source)
include_directories(${PROJECT_SOURCE_DIR}/../../libzlib/source)

set(ALL_FILES
t4.h
tif_aux.c
tif_close.c
tif_codec.c
tif_color.c
tif_compress.c
tif_config.h
tif_dir.c
tif_dir.h
tif_dirinfo.c
tif_dirread.c
tif_dirwrite.c
tif_dumpmode.c
tif_error.c
tif_extension.c
tif_fax3.c
tif_fax3.h
tif_fax3sm.c
tif_flush.c
tif_getimage.c
tif_jpeg.c
tif_luv.c
tif_lzma.c
tif_lzw.c
tif_next.c
tif_ojpeg.c
tif_open.c
tif_packbits.c
tif_pixarlog.c
tif_predict.c
tif_predict.h
tif_print.c
tif_read.c
tif_strip.c
tif_swab.c
tif_thunder.c
tif_tile.c
tif_version.c
tif_warning.c
tif_write.c
tif_zip.c
tiff.h
tiffconf.h
tiffio.h
tiffiop.h
tiffvers.h
uvcode.h
)

group_sources("${ALL_FILES}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")

elseif (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISABLE_PERF_MEASUREMENT -D__ANSI__")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fPIC -fexceptions -fvisibility=hidden -Wno-ctor-dtor-privacy -D__ANSI__")
endif ()

add_library(${PROJECT_NAME} ${ALL_FILES})

# Creates folder "external" and adds target project
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER external)
