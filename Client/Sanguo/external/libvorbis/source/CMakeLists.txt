cmake_minimum_required (VERSION 3.1.0)
project (static_vorbis)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/../../libogg/source)
include_directories(${PROJECT_SOURCE_DIR}/../../libogg/source/ogg)

set(ALL_FILES
lib/analysis.c
lib/bitrate.c
lib/block.c
lib/codebook.c
lib/envelope.c
lib/floor0.c
lib/floor1.c
lib/info.c
lib/lookup.c
lib/lpc.c
lib/lsp.c
lib/mapping0.c
lib/mdct.c
lib/psy.c
lib/registry.c
lib/res0.c
lib/sharedbook.c
lib/smallft.c
lib/synthesis.c
lib/vorbisenc.c
lib/window.c
lib/backends.h
lib/bitrate.h
lib/codebook.h
include/vorbis/codec.h
lib/codec_internal.h
lib/envelope.h
lib/modes/floor_all.h
lib/books/floor/floor_books.h
lib/highlevel.h
lib/lookup.h
lib/lookup_data.h
lib/lpc.h
lib/lsp.h
lib/masking.h
lib/mdct.h
lib/misc.h
lib/os.h
lib/psy.h
lib/modes/psych_11.h
lib/modes/psych_16.h
lib/modes/psych_44.h
lib/modes/psych_8.h
lib/registry.h
lib/books/coupled/res_books_stereo.h
lib/books/uncoupled/res_books_uncoupled.h
lib/modes/residue_16.h
lib/modes/residue_44.h
lib/modes/residue_44u.h
lib/modes/residue_8.h
lib/scales.h
lib/modes/setup_11.h
lib/modes/setup_16.h
lib/modes/setup_22.h
lib/modes/setup_32.h
lib/modes/setup_44.h
lib/modes/setup_44u.h
lib/modes/setup_8.h
lib/modes/setup_X.h
lib/smallft.h
include/vorbis/vorbisenc.h
include/vorbis/vorbisfile.h
lib/window.h
lib/vorbisfile.c
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOPJ_STATIC -DFREEIMAGE_LIB -DLIBRAW_NODLL")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
elseif (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISABLE_PERF_MEASUREMENT -D__ANSI__ -DOPJ_STATIC -DNO_LCMS")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -fPIC -fexceptions -fvisibility=hidden -Wno-ctor-dtor-privacy -D__ANSI__ -DOPJ_STATIC -DNO_LCMS")
else ()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
endif ()

add_library(${PROJECT_NAME} ${ALL_FILES})

# Creates folder "external" and adds target project
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER external)

target_link_libraries(${PROJECT_NAME} static_ogg)

foreach(FILE ${ALL_FILES}) 
    get_filename_component(PARENT_DIR "${FILE}" PATH)

    # skip source and changes /'s to \\'s
    string(REGEX REPLACE "(\\./)?(source)/?" "" GROUP "${PARENT_DIR}")
    string(REPLACE "/" "\\" GROUP "${GROUP}")
    source_group("${GROUP}" FILES "${FILE}")
endforeach()
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/wd4244 /wd4305 /wd4996")
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin)
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../bin)
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../bin)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME_DEBUG ${PROJECT_NAME}_D)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME_RELEASE ${PROJECT_NAME})
