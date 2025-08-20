/*!
 * \file WindowsFileSystem.h
 *
 * \author zjhlogo
 * \date 2019/06/20
 *
 * 
 */
#pragma once
#include "../../file/SubFileSystem.h"

NS_BEGIN

class WindowsFileSystem : public SubFileSystem
{
public:
    WindowsFileSystem(const tstring& rootDir, int prop = FP_READABLE, int storage = FS_ASSET, const PathRedirectCallback& cb = SubFileSystem::getDefaultRedirectionCb());
    virtual ~WindowsFileSystem();

	bool getFileLocation(tstring& filePath, long& offset, long& length) override;
    virtual bool searchFile(const tstring& filePath) override;
    virtual std::shared_ptr<File> openFile(const tstring& filePath, int flag) override;
    virtual bool deleteFile(const tstring& filePath) override;

    bool isFile(const tstring& filePath);
    bool isDirectory(const tstring& filePath);

private:
    bool makeDirectory(const tstring& fileDir);

private:
    tstring m_rootDir;

};

NS_END
