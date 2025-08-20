/*!
 * \file IosAssetFileSystem.h
 *
 * \author zjhlogo
 * \date 2019/07/01
 *
 *
 */
#pragma once
#include "../../file/SubFileSystem.h"

NS_BEGIN

class IosAssetFileSystem : public SubFileSystem
{
public:
    IosAssetFileSystem(int prop, int storgeSpace, const PathRedirectCallback& cb = SubFileSystem::getDefaultRedirectionCb());
    virtual ~IosAssetFileSystem();
    
    bool getFileLocation(tstring& filePath, long& offset, long& length) override;
    virtual bool searchFile(const tstring& filePath) override;
    virtual std::shared_ptr<File> openFile(const tstring& filePath, int flag) override;
    virtual bool deleteFile(const tstring& filePath) override;
    
    bool getFilePath(tstring& fullPathOut, const tstring& filePath);
    
};

NS_END
