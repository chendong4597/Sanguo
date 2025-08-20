cmake_minimum_required (VERSION 3.1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/utils.cmake)

project (static_jxr)

set(ALL_FILES
common/include/guiddef.h
common/include/wmsal.h
common/include/wmspecstring.h
common/include/wmspecstrings_adt.h
common/include/wmspecstrings_strict.h
common/include/wmspecstrings_undef.h
image/decode/decode.c
image/decode/decode.h
image/decode/JXRTranscode.c
image/decode/postprocess.c
image/decode/segdec.c
image/decode/strdec.c
image/decode/strdec_x86.c
image/decode/strInvTransform.c
image/decode/strPredQuantDec.c
image/encode/encode.c
image/encode/encode.h
image/encode/segenc.c
image/encode/strenc.c
image/encode/strenc_x86.c
image/encode/strFwdTransform.c
image/encode/strPredQuantEnc.c
image/sys/adapthuff.c
image/sys/ansi.h
image/sys/common.h
image/sys/image.c
image/sys/perfTimer.h
image/sys/perfTimerANSI.c
image/sys/strcodec.c
image/sys/strcodec.h
image/sys/strPredQuant.c
image/sys/strTransform.c
image/sys/strTransform.h
image/sys/windowsmediaphoto.h
image/sys/xplatform_image.h
image/x86/x86.h
jxrgluelib/JXRGlue.c
jxrgluelib/JXRGlue.h
jxrgluelib/JXRGlueJxr.c
jxrgluelib/JXRGluePFC.c
jxrgluelib/JXRMeta.c
jxrgluelib/JXRMeta.h
)

group_sources("${ALL_FILES}")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
elseif (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISABLE_PERF_MEASUREMENT -D__ANSI__")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -fPIC -fexceptions -fvisibility=hidden -Wno-ctor-dtor-privacy -D__ANSI__")
endif ()

add_library(${PROJECT_NAME} ${ALL_FILES})

# Creates folder "external" and adds target project
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER external)
