cmake_minimum_required (VERSION 3.1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/utils.cmake)

project (static_zstd)

set(ALL_FILES
entropy_common.c
fse_decompress.c
threading.c
pool.c
zstd_common.c
error_private.c
xxhash.c
hist.c
fse_compress.c
huf_compress.c
zstd_compress.c
zstdmt_compress.c
zstd_fast.c
zstd_double_fast.c
zstd_lazy.c
zstd_opt.c
zstd_ldm.c
huf_decompress.c
zstd_decompress.c
zstd_decompress_block.c
zstd_ddict.c
cover.c
fastcover.c
divsufsort.c
zdict.c
zbuff_common.c
zbuff_compress.c
zbuff_decompress.c
zstd.h
debug.h
pool.h
threading.h
bitstream.h
error_private.h
zstd_errors.h
fse.h
huf.h
mem.h
zstd_internal.h
hist.h
zstd_compress_internal.h
zstd_fast.h
zstd_double_fast.h
zstd_lazy.h
zstd_opt.h
zstd_ldm.h
zstdmt_compress.h
zstd_decompress_internal.h
zstd_decompress_block.h
zstd_ddict.h
zdict.h
cover.h
zbuff.h
)

group_sources("${ALL_FILES}")

add_library(${PROJECT_NAME} ${ALL_FILES})

# Creates folder "external" and adds target project
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER external)
