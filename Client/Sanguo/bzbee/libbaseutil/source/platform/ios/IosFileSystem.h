/*!
 * \file IosFileSystem.h
 *
 * \author zjhlogo
 * \date 2019/06/27
 *
 *
 */
#pragma once
#include "../../file/SubFileSystem.h"

NS_BEGIN

class IosFileSystem : public SubFileSystem
{
public:
    IosFileSystem(const tstring& rootDir, int prop, int storage, const PathRedirectCallback& cb = nullptr);
    virtual ~IosFileSystem();
    
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
