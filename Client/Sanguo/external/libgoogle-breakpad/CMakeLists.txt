cmake_minimum_required (VERSION 3.1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/utils.cmake)

project (static_google_breakpad)

include_directories(${PROJECT_SOURCE_DIR}/src/common/android/include)
include_directories(${PROJECT_SOURCE_DIR}/src)

add_definitions(-D ANDROID_ARM_MODE=arm)

set(ALL_FILES
src/client/linux/crash_generation/crash_generation_client.cc
src/client/linux/dump_writer_common/thread_info.cc
src/client/linux/dump_writer_common/ucontext_reader.cc
src/client/linux/handler/exception_handler.cc
src/client/linux/handler/minidump_descriptor.cc
src/client/linux/log/log.cc
src/client/linux/microdump_writer/microdump_writer.cc
src/client/linux/minidump_writer/linux_dumper.cc
src/client/linux/minidump_writer/linux_ptrace_dumper.cc
src/client/linux/minidump_writer/minidump_writer.cc
src/client/minidump_file_writer.cc
src/common/android/breakpad_getcontext.S
src/common/convert_UTF.c
src/common/md5.cc
src/common/string_conversion.cc
src/common/linux/elfutils.cc
src/common/linux/file_id.cc
src/common/linux/guid_creator.cc
src/common/linux/linux_libc_support.cc
src/common/linux/memory_mapped_file.cc
src/common/linux/safe_readlink.cc
)

group_sources("${ALL_FILES}")

set_property(SOURCE src/common/android/breakpad_getcontext.S PROPERTY LANGUAGE C)

add_library(${PROJECT_NAME} ${ALL_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER external)
