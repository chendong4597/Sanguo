cmake_minimum_required(VERSION 3.1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/utils.cmake)

project (static_protobuf)

include_directories(${PROJECT_SOURCE_DIR})

set(ALL_FILES
	google/protobuf/compiler/importer.cc
	google/protobuf/compiler/importer.h
	google/protobuf/compiler/parser.cc
	google/protobuf/compiler/parser.h
	google/protobuf/io/coded_stream.cc
	google/protobuf/io/coded_stream.h
	google/protobuf/io/gzip_stream.cc
	google/protobuf/io/gzip_stream.h
	google/protobuf/io/printer.cc
	google/protobuf/io/printer.h
	google/protobuf/io/strtod.cc
	google/protobuf/io/strtod.h
	google/protobuf/io/tokenizer.cc
	google/protobuf/io/tokenizer.h
	google/protobuf/io/zero_copy_stream.cc
	google/protobuf/io/zero_copy_stream.h
	google/protobuf/io/zero_copy_stream_impl.cc
	google/protobuf/io/zero_copy_stream_impl.h
	google/protobuf/io/zero_copy_stream_impl_lite.cc
	google/protobuf/io/zero_copy_stream_impl_lite.h
	google/protobuf/stubs/common.cc
	google/protobuf/stubs/common.h
	google/protobuf/stubs/once.cc
	google/protobuf/stubs/once.h
	google/protobuf/stubs/stringprintf.cc
	google/protobuf/stubs/stringprintf.h
	google/protobuf/stubs/structurally_valid.cc
	google/protobuf/stubs/strutil.cc
	google/protobuf/stubs/strutil.h
	google/protobuf/stubs/substitute.cc
	google/protobuf/stubs/substitute.h
	google/protobuf/text_format.cc
	google/protobuf/text_format.h
	google/protobuf/descriptor.cc
	google/protobuf/descriptor.h
	google/protobuf/descriptor.pb.cc
	google/protobuf/descriptor.pb.h
	google/protobuf/descriptor_database.cc
	google/protobuf/descriptor_database.h
	google/protobuf/dynamic_message.cc
	google/protobuf/dynamic_message.h
	google/protobuf/extension_set.cc
	google/protobuf/extension_set.h
	google/protobuf/extension_set_heavy.cc
	google/protobuf/generated_message_reflection.cc
	google/protobuf/generated_message_reflection.h
	google/protobuf/generated_message_util.cc
	google/protobuf/generated_message_util.h
	google/protobuf/message.cc
	google/protobuf/message.h
	google/protobuf/message_lite.cc
	google/protobuf/message_lite.h
	google/protobuf/reflection_ops.cc
	google/protobuf/reflection_ops.h
	google/protobuf/repeated_field.cc
	google/protobuf/repeated_field.h
	google/protobuf/service.cc
	google/protobuf/service.h
	google/protobuf/unknown_field_set.cc
	google/protobuf/unknown_field_set.h
	google/protobuf/wire_format.cc
	google/protobuf/wire_format.h
	google/protobuf/wire_format_lite.cc
	google/protobuf/wire_format_lite.h
)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	include_directories(google/protobuf/msvc)
	set(ALL_FILES ${ALL_FILES}
		google/protobuf/stubs/atomicops_internals_x86_msvc.h
		google/protobuf/stubs/atomicops_internals_x86_msvc.cc
		google/protobuf/msvc/config.h)
elseif (CMAKE_SYSTEM_NAME MATCHES "Android")
	include_directories(google/protobuf/android)
	if (ANDROID_ABI STREQUAL "x86" OR ANDROID_ABI STREQUAL "x86_64")
		set(ALL_FILES ${ALL_FILES}
			google/protobuf/stubs/atomicops_internals_x86_gcc.h
			google/protobuf/stubs/atomicops_internals_x86_gcc.cc)
	endif()
	set(ALL_FILES ${ALL_FILES}
		google/protobuf/android/config.h)
elseif (CMAKE_SYSTEM_NAME MATCHES "Ios")
	include_directories(google/protobuf/ios)
	set(ALL_FILES ${ALL_FILES}
		google/protobuf/ios/config.h)
endif()

group_sources("${ALL_FILES}")

add_definitions(-D _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS)

add_library(${PROJECT_NAME} ${ALL_FILES})

# Creates folder "external" and adds target project
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER external)
