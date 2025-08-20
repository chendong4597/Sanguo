cmake_minimum_required (VERSION 3.1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/utils.cmake)

project (static_minizip)
include_directories(${PROJECT_SOURCE_DIR}/../../libzlib/source)


set(ALL_FILES
crypt.h
ioapi.c
ioapi.h
miniunz.c
minizip.c
unzip.c
unzip.h
zip.c
zip.h
)
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
set(ALL_FILES ${ALL_FILES}
iowin32.c
iowin32.h
mztools.c
mztools.h
)
else (CMAKE_SYSTEM_NAME MATCHES "Android")

endif ()

group_sources("${ALL_FILES}")

add_library(${PROJECT_NAME} ${ALL_FILES})

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	target_link_libraries(${PROJECT_NAME} static_zlib)
elseif (CMAKE_SYSTEM_NAME MATCHES "Android")
	target_link_libraries(${PROJECT_NAME} z)
endif ()

# Creates folder "external" and adds target project
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER external)
