/*!
 * \file AssetFileSystem.cpp
 *
 * \author zjhlogo
 * \date 2019/06/24
 *
 * 
 */
#include "AssetFileSystem.h"
#include "File_Android_Asset.h"
#include "../../LogUtil.h"

NS_BEGIN

AssetFileSystem::AssetFileSystem(AAssetManager* asssetsMgr, int storage /* = FS_ASSET */, const PathRedirectCallback& cb /* = SubFileSystem::getDefaultRedirectionCb() */)
    :SubFileSystem(FP_READABLE, storage, cb)
    , m_assetManager(asssetsMgr)
{

}

AssetFileSystem::~AssetFileSystem()
{

}

bool AssetFileSystem::getFileLocation(tstring& filePath, long& offset, long& length)
{
	if (searchFile(filePath))
    {
		offset = length = 0;
		return true;
	}

	return false;
}

bool AssetFileSystem::searchFile(const tstring& filePath)
{
    return isFile(filePath);
}

std::shared_ptr<File> AssetFileSystem::openFile(const tstring& filePath, int flag)
{
    auto file = std::make_shared<File_Android_Asset>(m_assetManager);
    if (file->open(filePath)) return file;

    // open failed
    LOGE("Open file failed: {}", filePath);
    return nullptr;
}

bool AssetFileSystem::deleteFile(const tstring&)
{
    // we don't support delete file on apk
    return false;
}

bool AssetFileSystem::isFile(const tstring& filePath)
{
    AAsset* file = AAssetManager_open(m_assetManager, filePath.c_str(), AASSET_MODE_UNKNOWN);
    if (!file) return false;

    AAsset_close(file);
    return true;
}

NS_END
