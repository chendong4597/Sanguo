cmake_minimum_required (VERSION 3.1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/utils.cmake)

project (static_framework)

include_directories(${PROJECT_SOURCE_DIR}/../../libbaseutil/source)

set(ALL_FILES
component/Component.cpp
component/Component.h
component/ComponentSerializer.cpp
component/ComponentSerializer.h
component/CullTree.cpp
component/CullTree.h
component/RenderableComponent.cpp
component/RenderableComponent.h
component/SerializeFieldList.cpp
component/SerializeFieldList.h
core/ManagedObject.cpp
core/ManagedObject.h
core/ManagedObjectUtils.cpp
core/ManagedObjectUtils.h
core/ManagedInitializer.cpp
core/ManagedInitializer.h
core/ManagedClass.cpp
core/ManagedClass.h
core/ManagedPackage.cpp
core/ManagedPackage.h
core/ManagedFactory.cpp
core/ManagedFactory.h
input/TouchDelegate.cpp
input/TouchDelegate.h
action/ActionComponent.cpp
action/ActionComponent.h
Framework.cpp
Framework.h
CustomHeader.h
LuaBindFramework.cpp
LuaBindFramework.h
)

group_sources("${ALL_FILES}")

add_library(${PROJECT_NAME} ${ALL_FILES})

# Creates folder "bzbee" and adds target project
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER bzbee)
