cmake_minimum_required (VERSION 3.1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/utils.cmake)

project (static_zlib)

set(ALL_FILES
adler32.c
compress.c
crc32.c
crc32.h
deflate.c
deflate.h
gzclose.c
gzguts.h
gzlib.c
gzread.c
gzwrite.c
infback.c
inffast.c
inffast.h
inffixed.h
inflate.c
inflate.h
inftrees.c
inftrees.h
trees.c
trees.h
uncompr.c
zconf.h
zlib.h
zutil.c
zutil.h
)

group_sources("${ALL_FILES}")

add_library(${PROJECT_NAME} ${ALL_FILES})

# Creates folder "external" and adds target project
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER external)
