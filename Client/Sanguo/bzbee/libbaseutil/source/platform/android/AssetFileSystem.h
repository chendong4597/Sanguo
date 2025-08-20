/*!
 * \file AssetFileSystem.h
 *
 * \author zjhlogo
 * \date 2019/06/24
 *
 * 
 */
#pragma once
#include "../../file/FileSystem.h"
#include <android/asset_manager_jni.h>

NS_BEGIN

class AssetFileSystem : public SubFileSystem
{
public:
    AssetFileSystem(AAssetManager* asssetsMgr, int storage = FS_ASSET, const PathRedirectCallback& cb = SubFileSystem::getDefaultRedirectionCb());
    virtual ~AssetFileSystem();

	bool getFileLocation(tstring& filePath, long& offset, long& length) override;
    virtual bool searchFile(const tstring& filePath) override;
    virtual std::shared_ptr<File> openFile(const tstring& filePath, int flag) override;
    virtual bool deleteFile(const tstring& filePath) override;

    bool isFile(const tstring& filePath);

private:
    AAssetManager* m_assetManager{};
};

NS_END
