/*!
 * \file ZipFileSystem.cpp
 *
 * \author zjhlogo
 * \date 2019/06/21
 *
 * 
 */
#include "ZipFileSystem.h"
#include "File_Zip.h"
#include "../FileUtil.h"
#include "../LogUtil.h"

NS_BEGIN

ZipFileSystem::ZipFileSystem(const tstring& zipFilePath, int prop /* = FP_READABLE */, int storage /* = 0 */, const PathRedirectCallback& cb /* = SubFileSystem::getDefaultRedirectionCb() */)
    :SubFileSystem(prop, storage, cb)
    , m_zipFilePath(zipFilePath)
{
	m_bOk = openZipPackage();
}

ZipFileSystem::~ZipFileSystem()
{
    closeZipPackage();
}

bool ZipFileSystem::getFileLocation(tstring& filePath, long& offset, long& length)
{
    int offsetOut{};
    int lengthOut{};

	if (getZipFileInfo(offsetOut, lengthOut, filePath))
    {
		filePath = m_zipFilePath;
		offset = offsetOut;
		length = lengthOut;
		return true;
	}

	return false;
}

bool ZipFileSystem::searchFile(const tstring& filePath)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_entriesMap.find(filePath) != m_entriesMap.end())
    {
        return true;
    }

    return false;
}

std::shared_ptr<File> ZipFileSystem::openFile(const tstring& filePath, int flag)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    ZipEntryMap::iterator it = m_entriesMap.find(filePath);
    if (it == m_entriesMap.end()) return nullptr;

    auto file = std::make_shared<File_Zip>();
    if (file->open(m_unzipFile, filePath, it->second)) return file;

    // open failed
    LOGE("Open file failed: {}", filePath);
    return nullptr;
}

bool ZipFileSystem::readFileIntoBuffer(BufferData& bufferOut, const tstring& filePath, int flag)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    ZipEntryMap::iterator it = m_entriesMap.find(filePath);
    if (it == m_entriesMap.end()) return false;

    if (!File_Zip::readFileIntoBuffer(bufferOut, m_unzipFile, filePath, it->second))
    {
        // open failed
        LOGE("Open file failed: {}", filePath);
        return false;
    }

    return true;
}

bool ZipFileSystem::getZipFileInfo(int& offsetOut, int& sizeOut, const tstring& filePath)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_entriesMap.find(filePath);
    if (it != m_entriesMap.end())
    {
        if (UNZ_OK != unzGoToFilePos(m_unzipFile, &it->second)) return false;

        unz_file_info fileInfo;
        if (UNZ_OK != unzGetCurrentFileInfo(m_unzipFile, &fileInfo, nullptr, 0, nullptr, 0, nullptr, 0)) return false;
        sizeOut = fileInfo.compressed_size;
        
		if (UNZ_OK == unzOpenCurrentFile(m_unzipFile))
		{
			offsetOut = static_cast<int>(unzGetCurrentFileZStreamPos64(m_unzipFile));
			unzCloseCurrentFile(m_unzipFile);
		}

        return true;
    }

    return false;
}

bool ZipFileSystem::openZipPackage()
{
    static const int MAX_NAME_LENGTH = 2048;
    char nameBuffer[MAX_NAME_LENGTH];

    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_unzipFile) return true;

    m_unzipFile = unzOpen(m_zipFilePath.c_str());
    if (!m_unzipFile)
    {
        LOGE("Open zip package failed: {}", m_zipFilePath);
        return false;
    }

    m_entriesMap.clear();
    for (int ret = unzGoToFirstFile(m_unzipFile); ret == UNZ_OK; ret = unzGoToNextFile(m_unzipFile))
    {
        unzGetCurrentFileInfo(m_unzipFile, nullptr, nameBuffer, MAX_NAME_LENGTH, nullptr, 0, nullptr, 0);
        unzGetFilePos(m_unzipFile, &m_entriesMap[FileUtil::formatFilePath(nameBuffer)]);
    }

    return true;
}

void ZipFileSystem::closeZipPackage()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    unzClose(m_unzipFile);
    m_unzipFile = nullptr;
}

NS_END
