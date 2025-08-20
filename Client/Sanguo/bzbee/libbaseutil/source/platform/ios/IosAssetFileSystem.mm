
/*!
 * \file IosAssetFileSystem.mm
 *
 * \author zjhlogo
 * \date 2019/07/01
 *
 *
 */
#include "IosAssetFileSystem.h"
#include "File_Ios.h"
#include "../../FileUtil.h"
#include "../../LogUtil.h"

#import <Foundation/Foundation.h>

NS_BEGIN

IosAssetFileSystem::IosAssetFileSystem(int prop, int storgeSpace, const PathRedirectCallback& cb /* =SubFileSystem::getDefaultRedirectionCb() */)
:SubFileSystem(prop, storgeSpace, cb)
{
    
}

IosAssetFileSystem::~IosAssetFileSystem()
{
    
}

bool IosAssetFileSystem::getFileLocation(tstring& filePath, long& offset, long& length)
{
    tstring fullPath;
    
    if (getFilePath(fullPath, filePath))
    {
        fullPath = filePath;
        offset = 0;
        length = 0;
        return true;
    }
    return false;
}

bool IosAssetFileSystem::searchFile(const tstring & filePath)
{
    tstring fullPath;
    return getFilePath(fullPath, filePath);
}

std::shared_ptr<File> IosAssetFileSystem::openFile(const tstring & filePath, int flag)
{
    tstring fullPath;
    
    if (!getFilePath(fullPath, filePath)) return  nullptr;
    
    auto file = std::make_shared<File_Ios>();
    if (file->open(fullPath, flag)) return file;
    
    // open failed
    LOGE("Open file failed: {}", fullPath);
    return nullptr;
}

bool IosAssetFileSystem::deleteFile(const tstring & filePath)
{
    return false;
}

bool IosAssetFileSystem::getFilePath(tstring& fullPathOut, const tstring & filePath)
{
    NSString* relPath = [NSString stringWithUTF8String:filePath.c_str()];

    // pathForResource also searches in .lproj directories. issue #1230
    NSString* lastPathComponent = [relPath lastPathComponent];
    
    NSString* imageDirectory = [relPath stringByDeletingLastPathComponent];
    
    // search path from directory set by setResourceDirectory
    NSString* fullpath = [[NSBundle mainBundle] pathForResource:lastPathComponent ofType:nil inDirectory:imageDirectory];

    if (fullpath == nil) return false;
    
    fullPathOut = [fullpath UTF8String];
    return true;
}

NS_END
