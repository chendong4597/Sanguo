cmake_minimum_required (VERSION 3.1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/utils.cmake)

project (static_webp)

set(ALL_FILES
dec/alphai.h
dec/dec.alpha.c
dec/dec.buffer.c
dec/dec.frame.c
dec/dec.idec.c
dec/dec.io.c
dec/dec.quant.c
dec/dec.tree.c
dec/dec.vp8.c
dec/dec.vp8l.c
dec/dec.webp.c
dec/decode_vp8.h
dec/vp8i.h
dec/vp8li.h
dec/webpi.h
demux/demux.demux.c
dsp/dsp.h
dsp/dsp.alpha_processing.c
dsp/dsp.alpha_processing_mips_dsp_r2.c
dsp/dsp.alpha_processing_sse2.c
dsp/dsp.argb.c
dsp/dsp.argb_mips_dsp_r2.c
dsp/dsp.argb_sse2.c
dsp/dsp.cost.c
dsp/dsp.cost_mips_dsp_r2.c
dsp/dsp.cost_mips32.c
dsp/dsp.cost_sse2.c
dsp/dsp.cpu.c
dsp/dsp.dec.c
dsp/dsp.dec_clip_tables.c
dsp/dsp.dec_mips_dsp_r2.c
dsp/dsp.dec_mips32.c
dsp/dsp.dec_neon.c
dsp/dsp.dec_sse2.c
dsp/dsp.enc.c
dsp/dsp.enc_avx2.c
dsp/dsp.enc_mips_dsp_r2.c
dsp/dsp.enc_mips32.c
dsp/dsp.enc_neon.c
dsp/dsp.enc_sse2.c
dsp/dsp.filters.c
dsp/dsp.filters_mips_dsp_r2.c
dsp/dsp.filters_sse2.c
dsp/dsp.lossless.c
dsp/dsp.lossless_mips_dsp_r2.c
dsp/dsp.lossless_mips32.c
dsp/dsp.lossless_neon.c
dsp/dsp.lossless_sse2.c
dsp/dsp.rescaler.c
dsp/dsp.rescaler_mips_dsp_r2.c
dsp/dsp.rescaler_mips32.c
dsp/dsp.upsampling.c
dsp/dsp.upsampling_mips_dsp_r2.c
dsp/dsp.upsampling_neon.c
dsp/dsp.upsampling_sse2.c
dsp/dsp.yuv.c
dsp/dsp.yuv_mips_dsp_r2.c
dsp/dsp.yuv_mips32.c
dsp/dsp.yuv_sse2.c
dsp/lossless.h
dsp/mips_macro.h
dsp/neon.h
dsp/yuv.h
dsp/yuv_tables_sse2.h
enc/backward_references.h
enc/cost.h
enc/enc.alpha.c
enc/enc.analysis.c
enc/enc.backward_references.c
enc/enc.config.c
enc/enc.cost.c
enc/enc.filter.c
enc/enc.frame.c
enc/enc.histogram.c
enc/enc.iterator.c
enc/enc.near_lossless.c
enc/enc.picture.c
enc/enc.picture_csp.c
enc/enc.picture_psnr.c
enc/enc.picture_rescale.c
enc/enc.picture_tools.c
enc/enc.quant.c
enc/enc.syntax.c
enc/enc.token.c
enc/enc.tree.c
enc/enc.vp8l.c
enc/enc.webpenc.c
enc/histogram.h
enc/vp8enci.h
enc/vp8li.h
mux/mux.anim_encode.c
mux/mux.muxedit.c
mux/mux.muxinternal.c
mux/mux.muxread.c
mux/muxi.h
utils/bit_reader.h
utils/bit_reader_inl.h
utils/bit_writer.h
utils/color_cache.h
utils/endian_inl.h
utils/filters.h
utils/huffman.h
utils/huffman_encode.h
utils/quant_levels.h
utils/quant_levels_dec.h
utils/random.h
utils/rescaler.h
utils/thread.h
utils/utils.h
utils/utils.bit_reader.c
utils/utils.bit_writer.c
utils/utils.color_cache.c
utils/utils.filters.c
utils/utils.huffman.c
utils/utils.huffman_encode.c
utils/utils.quant_levels.c
utils/utils.quant_levels_dec.c
utils/utils.random.c
utils/utils.rescaler.c
utils/utils.thread.c
utils/utils.utils.c
webp/decode.h
webp/demux.h
webp/encode.h
webp/format_constants.h
webp/mux.h
webp/mux_types.h
webp/types.h
)

group_sources("${ALL_FILES}")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
elseif (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif ()

add_library(${PROJECT_NAME} ${ALL_FILES})

# Creates folder "external" and adds target project
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER external)
