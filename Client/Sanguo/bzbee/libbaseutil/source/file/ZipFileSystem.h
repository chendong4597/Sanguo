/*!
 * \file ZipFileSystem.h
 *
 * \author zjhlogo
 * \date 2019/06/21
 *
 * 
 */
#pragma once
#include "SubFileSystem.h"
#include <unzip.h>
#include <unordered_map>
#include <mutex>

NS_BEGIN

class ZipFileSystem : public SubFileSystem
{
public:
    typedef std::unordered_map<tstring, unz_file_pos> ZipEntryMap;

public:
    ZipFileSystem(const tstring& zipFilePath, int prop = FP_READABLE, int storage = 0, const PathRedirectCallback& cb = SubFileSystem::getDefaultRedirectionCb());
    virtual ~ZipFileSystem();

	bool getFileLocation(tstring& filePath, long& offset, long& length) override;
    virtual bool searchFile(const tstring& filePath) override;
    virtual std::shared_ptr<File> openFile(const tstring& filePath, int flag) override;
    virtual bool deleteFile(const tstring& filePath) override { return false; };

    virtual bool readFileIntoBuffer(BufferData& bufferOut, const tstring& filePath, int flag) override;
    bool getZipFileInfo(int& offsetOut, int& sizeOut, const tstring& filePath);
	bool isOk() const { return m_bOk; };

private:
    bool openZipPackage();
    void closeZipPackage();

private:
    std::mutex m_mutex;
    tstring m_zipFilePath;
    unzFile m_unzipFile{};
    ZipEntryMap m_entriesMap;
	bool m_bOk{};

};

NS_END
