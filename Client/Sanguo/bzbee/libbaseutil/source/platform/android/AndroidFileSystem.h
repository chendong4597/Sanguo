/*!
 * \file AndroidFileSystem.h
 *
 * \author zjhlogo
 * \date 2019/06/24
 *
 * 
 */
#pragma once
#include "../../file/SubFileSystem.h"

NS_BEGIN

class AndroidFileSystem : public SubFileSystem
{
public:
    AndroidFileSystem(const tstring& rootDir, int prop = FP_READABLE, int storage = 0, const PathRedirectCallback& cb = nullptr);
    virtual ~AndroidFileSystem();

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
