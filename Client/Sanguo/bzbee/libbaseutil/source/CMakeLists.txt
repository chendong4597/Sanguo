cmake_minimum_required (VERSION 3.1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/utils.cmake)

project (static_baseutil)

include_directories(${PROJECT_SOURCE_DIR}/../../../external/libjsoncpp/source)
include_directories(${PROJECT_SOURCE_DIR}/../../../external/libzlib/source)
include_directories(${PROJECT_SOURCE_DIR}/../../../external/libminizip/source)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
include_directories(${PROJECT_SOURCE_DIR}/platform/win32)
endif ()

set(ALL_FILES
serialize/FieldAttributes.cpp
serialize/FieldAttributes.h
serialize/SerializeField.cpp
serialize/SerializeField.h
serialize/SerializeOp.cpp
serialize/SerializeOp.h

utils/BufferStream.cpp
utils/BufferStream.h
utils/IntrusiveList.h
utils/ContainerMemory.h
utils/DynamicArray.h
utils/IndirectArray.h
utils/ChunckedArray.h
utils/Sorting.h
utils/RefCountObject.h
utils/Optional.h
utils/SimpleMest.h
utils/Delegate.h
utils/STLUtils.h
utils/HashSet.h
utils/HashMap.h
utils/FunctionRef.h
utils/TupleArray.h
utils/s3_ref_ptr.h
utils/S3Referenced.h
utils/S3DataStream.h
utils/S3MemoryDataStream.cpp
utils/S3MemoryDataStream.h
utils/idList.h
utils/idSort.h
utils/idStaticList.h
utils/LogFile.cpp
utils/LogFile.h

utf8/utf8.h
utf8/unchecked.h
utf8/core.h
utf8/checked.h

math/MathCommon.cpp
math/MathCommon.h
math/ColourValue.cpp
math/ColourValue.h
math/Bitwise.h

allocator/BlockAllocator.h
allocator/AllocCommon.cpp
allocator/AllocCommon.h

i18n/ITranslator.cpp
i18n/ITranslator.h
i18n/Localization.cpp
i18n/Localization.h

debugs/Debug.cpp
debugs/Debug.h

platform/Platform.h
platform/sys_threading.h
platform/sys_threading.cpp

lua/LuaBind.cpp
lua/LuaBind.h
lua/LuaCallback.cpp
lua/LuaCallback.h
lua/ScriptVM.cpp
lua/ScriptVM.h
lua/LuaPlus/lapi.c
lua/LuaPlus/lapi.h
lua/LuaPlus/lauxlib.c
lua/LuaPlus/lauxlib.h
lua/LuaPlus/lbaselib.c
lua/LuaPlus/lbitlib.c
lua/LuaPlus/lcode.c
lua/LuaPlus/lcode.h
lua/LuaPlus/lcorolib.c
lua/LuaPlus/lctype.c
lua/LuaPlus/lctype.h
lua/LuaPlus/ldblib.c
lua/LuaPlus/ldebug.c
lua/LuaPlus/ldebug.h
lua/LuaPlus/ldo.c
lua/LuaPlus/ldo.h
lua/LuaPlus/ldump.c
lua/LuaPlus/lfunc.c
lua/LuaPlus/lfunc.h
lua/LuaPlus/lgc.c
lua/LuaPlus/lgc.h
lua/LuaPlus/linit.c
lua/LuaPlus/liolib.c
lua/LuaPlus/llex.c
lua/LuaPlus/llex.h
lua/LuaPlus/llimits.h
lua/LuaPlus/lmathlib.c
lua/LuaPlus/lmem.c
lua/LuaPlus/lmem.h
lua/LuaPlus/loadlib.c
lua/LuaPlus/lobject.c
lua/LuaPlus/lobject.h
lua/LuaPlus/lopcodes.c
lua/LuaPlus/lopcodes.h
lua/LuaPlus/loslib.c
lua/LuaPlus/lparser.c
lua/LuaPlus/lparser.h
lua/LuaPlus/lprefix.h
lua/LuaPlus/lstate.c
lua/LuaPlus/lstate.h
lua/LuaPlus/lstring.c
lua/LuaPlus/lstring.h
lua/LuaPlus/lstrlib.c
lua/LuaPlus/ltable.c
lua/LuaPlus/ltable.h
lua/LuaPlus/ltablib.c
lua/LuaPlus/ltm.c
lua/LuaPlus/ltm.h
lua/LuaPlus/lua.h
lua/LuaPlus/lua.hpp
lua/LuaPlus/luaconf.h
lua/LuaPlus/lualib.h
lua/LuaPlus/lundump.c
lua/LuaPlus/lundump.h
lua/LuaPlus/lutf8lib.c
lua/LuaPlus/lvm.c
lua/LuaPlus/lvm.h
lua/LuaPlus/lzio.c
lua/LuaPlus/lzio.h
lua/LuaPlus/LuaAutoBlock.h
lua/LuaPlus/LuaCall.cpp
lua/LuaPlus/LuaCall.h
lua/LuaPlus/LuaHelper.cpp
lua/LuaPlus/LuaHelper.h
lua/LuaPlus/LuaHelper_Object.h
lua/LuaPlus/LuaHelper_StackObject.h
lua/LuaPlus/LuaObject.cpp
lua/LuaPlus/LuaObject.h
lua/LuaPlus/LuaPlus.cpp
lua/LuaPlus/LuaPlus.h
lua/LuaPlus/LuaPlusAddons.c
lua/LuaPlus/LuaPlusCD.cpp
lua/LuaPlus/LuaPlusCD.h
lua/LuaPlus/LuaPlusConfig.h
lua/LuaPlus/LuaPlusInternal.h
lua/LuaPlus/LuaStackObject.cpp
lua/LuaPlus/LuaStackObject.h
lua/LuaPlus/LuaStackTableIterator.h
lua/LuaPlus/LuaState.cpp
lua/LuaPlus/LuaState.h
lua/LuaPlus/LuaStateCD.cpp
lua/LuaPlus/LuaStateCD.h
lua/LuaPlus/LuaStateOutFile.cpp
lua/LuaPlus/LuaStateOutFile.h
lua/LuaPlus/LuaStateOutString.h
lua/LuaPlus/LuaState_DumpObject.cpp
lua/LuaPlus/LuaStackTableIterator.cpp
lua/LuaPlus/LuaTableIterator.cpp
lua/LuaPlus/LuaTableIterator.h

id_jobsystem/Thread.h
id_jobsystem/Thread.cpp
id_jobsystem/ParallelJobList.h
id_jobsystem/ParallelJobList.cpp

file/File.h
file/File_Zip.cpp
file/File_Zip.h
file/FileSystem.cpp
file/FileSystem.h
file/SubFileSystem.cpp
file/SubFileSystem.h
file/ZipFileSystem.cpp
file/ZipFileSystem.h

base64.cpp
base64.h
BaseType.cpp
BaseType.h
BaseTypeDefine.h
BaseUtil_Config.h
baseutil.h
Cache.cpp
Cache.h
Cpp11EventSystem.cpp
Cpp11EventSystem.h
EnumUtil.h
AssertionMacros.h
FileUtil.cpp
FileUtil.h
format.cc
format.h
LogUtil.cpp
LogUtil.h
md5.cpp
md5.h
Name.cpp
Name.h
Rtti.cpp
Rtti.h
StringUtil.cpp
StringUtil.h
TextInputUtil.cpp
TextInputUtil.h
TimeUtil.cpp
TimeUtil.h
SystemUtil.cpp
SystemUtil.h
tinyxml2.cpp
tinyxml2.h
XmlUtil.cpp
XmlUtil.h
CompileSpec.h
crc_8x256_tables.cpp
crc_8x256_tables.h
MemCrc.cpp
MemCrc.h
Md5Hash.h
RandomUtil.cpp
RandomUtil.h
GarbageCollection.cpp
GarbageCollection.h
Guid.cpp
Guid.h
MacroUtility.h
htmlescape.h
)

include(profiler/CMakeLists.txt)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
set(ALL_FILES ${ALL_FILES}
platform/win32/dirent_win32.h
platform/win32/File_Win32.cpp
platform/win32/File_Win32.h
platform/win32/LogUtil_Win32.cpp
platform/win32/TimeUtil_Win32.cpp
platform/win32/TimeUtil_Win32.h
platform/win32/TextInputUtil_Win32.cpp
platform/win32/TextInputUtil_Win32.h
platform/win32/SystemUtil_Win32.cpp
platform/win32/SystemUtil_Win32.h
platform/win32/MinWindows.h
platform/win32/Win32SystemIncludes.h
platform/win32/WindowsFileSystem.cpp
platform/win32/WindowsFileSystem.h
)
elseif (CMAKE_SYSTEM_NAME MATCHES "Android")
set(ALL_FILES ${ALL_FILES}
platform/android/AndroidFileSystem.cpp
platform/android/AndroidFileSystem.h
platform/android/AndroidSystemIncludes.h
platform/android/AssetFileSystem.cpp
platform/android/AssetFileSystem.h
platform/android/File_Android.cpp
platform/android/File_Android.h
platform/android/File_Android_Asset.cpp
platform/android/File_Android_Asset.h
platform/android/JniHelper.cpp
platform/android/JniHelper.h
platform/android/LogUtil_Android.cpp
platform/android/System_Info_Android.h
platform/android/SystemUtil_Android.cpp
platform/android/SystemUtil_Android.h
platform/android/TextInputUtil_Android.cpp
platform/android/TextInputUtil_Android.h
platform/android/TimeUtil_Android.cpp
platform/android/TimeUtil_Android.h
)
endif ()

group_sources("${ALL_FILES}")

add_library(${PROJECT_NAME} ${ALL_FILES})

# Creates folder "bzbee" and adds target project
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER bzbee)
