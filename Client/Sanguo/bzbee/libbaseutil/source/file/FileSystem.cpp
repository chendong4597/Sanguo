/*!
 * \file FileSystem.cpp
 *
 * \author zjhlogo
 * \date 2019/06/20
 *
 * 
 */
#include "FileSystem.h"
#include "SubFileSystem.h"
#include "../LogUtil.h"
#include <algorithm>

NS_BEGIN

FileSystem::SubFileSystemList FileSystem::m_subFileSystems;

bool FileSystem::isFileExist(const tstring& filePath, int storage /* = 0 */)
{
    for (SubFileSystemList::reverse_iterator it = m_subFileSystems.rbegin(); it != m_subFileSystems.rend(); ++it)
    {
        SubFileSystem* subFileSystem = *it;
        if (!subFileSystem->acceptStorage(storage)) continue;

        auto&& newFilePath = subFileSystem->redirectPath(filePath);
        if (subFileSystem->searchFile(newFilePath))
        {
            return true;
        }
    }

    return false;
}

bool FileSystem::getFileLocation(tstring& filePath, long& offset, long& length)
{
	for (auto it = m_subFileSystems.rbegin(); it != m_subFileSystems.rend(); ++it)
    {
		SubFileSystem* subFileSystem = *it;

        auto&& newFilePath = subFileSystem->redirectPath(filePath);
        if (subFileSystem->getFileLocation(newFilePath, offset, length))
        {
            filePath = newFilePath;
			return true;
		}
	}

	return false;
}

std::shared_ptr<File> FileSystem::openFile(const tstring& filePath, int storage /* = 0 */)
{
    for (SubFileSystemList::reverse_iterator it = m_subFileSystems.rbegin(); it != m_subFileSystems.rend(); ++it)
    {
        SubFileSystem* subFileSystem = *it;
        if (!subFileSystem->acceptStorage(storage)) continue;

        auto&& newFilePath = subFileSystem->redirectPath(filePath);
        if (subFileSystem->searchFile(newFilePath))
        {
            auto file = subFileSystem->openFile(newFilePath, File::AM_READ);
            if (file) return file;

            LOGE("Error, Open file failed: {0}", filePath);
            return nullptr;
        }
    }

    return nullptr;
}

bool FileSystem::readFileIntoBuffer(BufferData& bufferOut, const tstring& filePath, int storage /*= 0*/)
{
    for (SubFileSystemList::reverse_iterator it = m_subFileSystems.rbegin(); it != m_subFileSystems.rend(); ++it)
    {
        SubFileSystem* subFileSystem = *it;
        if (!subFileSystem->acceptStorage(storage)) continue;

        auto&& newFilePath = subFileSystem->redirectPath(filePath);
        if (subFileSystem->searchFile(newFilePath))
        {
            if (!subFileSystem->readFileIntoBuffer(bufferOut, newFilePath, File::AM_READ))
            {
                LOGE("Error, Open file failed: {0}", filePath);
                return false;
            }

            return true;
        }
    }

    return false;
}

std::shared_ptr<File> FileSystem::createFile(const tstring& filePath, int storage /* = 0 */, int mode /* = File::AM_WRITE */)
{
    for (SubFileSystemList::reverse_iterator it = m_subFileSystems.rbegin(); it != m_subFileSystems.rend(); ++it)
    {
        SubFileSystem* subFileSystem = *it;
        if (!subFileSystem->acceptStorage(storage)) continue;

        if (!subFileSystem->checkProperty(SubFileSystem::FP_WRITABLE)) continue;

        // check the mode, only accept ether "append", "writing" or "plus"
        if ((mode & (File::AM_WRITE | File::AM_APPEND | File::AM_PLUS)) == 0) continue;

        auto&& newFilePath = subFileSystem->redirectPath(filePath);
        auto file = subFileSystem->openFile(newFilePath, mode);
        if (file) return file;

        LOGE("Error, Create file failed: {0}", filePath);
        return nullptr;
    }

    return nullptr;
}

bool FileSystem::deleteFile(const tstring& filePath, int storage)
{
    for (SubFileSystemList::reverse_iterator it = m_subFileSystems.rbegin(); it != m_subFileSystems.rend(); ++it)
    {
        SubFileSystem* subFileSystem = *it;
        if (!subFileSystem->acceptStorage(storage)) continue;

        if (!subFileSystem->checkProperty(SubFileSystem::FP_WRITABLE)) continue;

        auto&& newFilePath = subFileSystem->redirectPath(filePath);
        if (!subFileSystem->deleteFile(newFilePath))
        {
            LOGE("Error, Delete file failed: {0}", filePath);
            return false;
        }
    }

    return false;
}

bool FileSystem::addSubFileSystem(SubFileSystem* subFileSystem, int priority /* = -1 */)
{
    if (priority == -1) priority = static_cast<int>(m_subFileSystems.size());
    subFileSystem->setPriority(priority);

    m_subFileSystems.push_back(subFileSystem);

    std::sort(m_subFileSystems.begin(), m_subFileSystems.end(), [](const SubFileSystem* a, const SubFileSystem* b)
    {
        return a->getPriority() < b->getPriority();
    });

    return true;
}

bool FileSystem::removeSubFileSystem(SubFileSystem* subFileSystem)
{
	if (!subFileSystem)
	{
		return true;
	}
	m_subFileSystems.erase(std::remove(m_subFileSystems.begin(), m_subFileSystems.end(), subFileSystem), m_subFileSystems.end());
	delete subFileSystem;

	return false;
}

void FileSystem::cleanup()
{
    for (SubFileSystemList::reverse_iterator it = m_subFileSystems.rbegin(); it != m_subFileSystems.rend(); ++it)
    {
        SAFE_DELETE(*it);
    }
    m_subFileSystems.clear();
}

NS_END
