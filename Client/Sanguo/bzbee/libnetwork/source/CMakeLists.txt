cmake_minimum_required(VERSION 3.1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/utils.cmake)

project (static_network)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/../../libbaseutil/source)
include_directories(${PROJECT_SOURCE_DIR}/../../libframework/source)
include_directories(${PROJECT_SOURCE_DIR}/../../../external/libprotobuf/source)
include_directories(${PROJECT_SOURCE_DIR}/../../../external/libcurl/source)

set(ALL_FILES
MsgNetWork.pb.cc
MsgNetWork.pb.h
MsgDecoder.cpp
MsgDecoder.h
MsgHandler.cpp
MsgHandler.h
DispatchMsg.h
DispatchMsg.cpp
NetworkModule.cpp
NetworkModule.h
loader/ILoaderBase.h
loader/ILoaderBase.cpp
loader/HttpLoader.h
loader/HttpLoader.cpp
loader/FileLoader.h
loader/FileLoader.cpp
)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(ALL_FILES ${ALL_FILES}
		platform/win32/NetworkModule_Impl_Win32.cpp
		platform/win32/NetworkModule_Impl_Win32.h)
elseif (CMAKE_SYSTEM_NAME MATCHES "Android")
	set(ALL_FILES ${ALL_FILES}
		platform/android/NetworkModule_Impl_Android.cpp
		platform/android/NetworkModule_Impl_Android.h)
elseif (CMAKE_SYSTEM_NAME MATCHES "Ios")
	set(ALL_FILES ${ALL_FILES}
		platform/android/NetworkModule_Impl_Ios.cpp
		platform/android/NetworkModule_Impl_Ios.h)
endif()

group_sources("${ALL_FILES}")

add_library(${PROJECT_NAME} ${ALL_FILES})

# Creates folder "bzbee" and adds target project
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER bzbee)
