/*!
 * \file FileUtil.cpp
 * \date 7-18-2012
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#include "FileUtil.h"
#include "StringUtil.h"
#include "LogUtil.h"
#include "file/FileSystem.h"

#if defined(_WIN32)
#include <direct.h>
#include <dirent_win32.h>
#elif defined(__unix__) || defined(__linux__) || defined(__APPLE__)
#include <dirent.h>
#endif

NS_BEGIN

const tstring FileUtil::CURRENT_DIR = ".";
const tstring FileUtil::PARENT_DIR = "..";

tstring FileUtil::m_writableDir{};
tstring FileUtil::m_writableAssetsDir{};
tstring FileUtil::m_revisionUpdateDir{};
tstring FileUtil::m_mainObbPath{"Resource/main.obb"};
tstring FileUtil::m_patchObbPath{};
int FileUtil::m_obbVersionCode{ 0 };

bool FileUtil::readFileIntoString(tstring& stringOut, const tstring& filePath, int storage)
{
	BufferData bufferData;
    if (!FileSystem::readFileIntoBuffer(bufferData, filePath, storage))
    {
        return false;
    }

    stringOut.append((char*)bufferData.data(), bufferData.size());
	return true;
}

std::future<tstring> FileUtil::asyncReadFileIntoString(const tstring & filePath, int storage)
{
    return std::async(std::launch::async, [filePath, storage]
    {
        tstring stringOut;
        readFileIntoString(stringOut, filePath, storage);
        return stringOut;
    });
}

void getFileDir(tstring& strOut, const tstring& strIn)
{
	size_t nPosEnd = strIn.find_last_of("/\\");

	if (nPosEnd == tstring::npos)
	{
		strOut.clear();
		return;
	}

	strOut = strIn.substr(0, nPosEnd);
}

bool FileUtil::writeStringIntoFile(const tstring& stringIn, const tstring& filePath, int storage /* = SubFileSystem::FS_EXTERNAL */)
{
    return writeBufferIntoFile(stringIn.data(), stringIn.length(), filePath, storage);
}

bool FileUtil::writeBufferIntoFile(const void* bufferData, int bufferSize, const tstring& filePath, int storage /* = SubFileSystem::FS_EXTERNAL */)
{
    auto file = FileSystem::createFile(filePath, storage);
    if (!file) return false;

    file->write(bufferData, bufferSize);
    file->close();

    return true;
}

size_t FileUtil::listFilesInDirectory(StringSet& filesOut, const tstring& dir, bool recursive, const StringSet& includeExts /* = TS_STRING() */, const StringSet& excludeExts /* = TS_STRING() */)
{
	enumlateFiles(filesOut, dir, recursive);
	filterFilesByExtension(filesOut, includeExts, excludeExts);
	return filesOut.size();
}

size_t FileUtil::filterFilesByExtension(StringSet& filesInOut, const StringSet& includeExts /* = TS_STRING() */, const StringSet& excludeExts /* = TS_STRING() */)
{
	if (includeExts.empty() && !excludeExts.empty())
	{
		StringSet oldFiles = filesInOut;
		filesInOut.clear();
		for (const auto& filePath : oldFiles)
		{
			if (isFileExtension(filePath, excludeExts)) continue;
			filesInOut.insert(filePath);
		}
	}
	else if (!includeExts.empty() && excludeExts.empty())
	{
		StringSet oldFiles = filesInOut;
		filesInOut.clear();
		for (const auto& filePath : oldFiles)
		{
			if (isFileExtension(filePath, includeExts))
			{
				filesInOut.insert(filePath);
			}
		}
	}
	else if (!includeExts.empty() && !excludeExts.empty())
	{
		StringSet newIncludeExt = includeExts;
		for (const auto& ext : excludeExts)
		{
			newIncludeExt.erase(ext);
		}

		StringSet oldFiles = filesInOut;
		filesInOut.clear();
		for (const auto& filePath : oldFiles)
		{
			if (isFileExtension(filePath, newIncludeExt))
			{
				filesInOut.insert(filePath);
			}
		}
	}

	return filesInOut.size();
}

bool FileUtil::isFileExtension(const tstring& filePath, const StringSet& exts, bool caseSensitive /* = false */)
{
	if (exts.size() <= 0) return true;

	tstring fileExt = getFileExtension(filePath);
	if (!caseSensitive)
	{
		tstring lowerFileExt = fileExt;
		std::transform(lowerFileExt.begin(), lowerFileExt.end(), lowerFileExt.begin(), ::tolower);

		StringSet lowerExts;
		for (const auto& ext : exts)
		{
			tstring lowerExt = ext;
			std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);
			lowerExts.insert(lowerExt);
		}

		if (lowerExts.find(lowerFileExt) != lowerExts.end()) return true;
	}
	else
	{
		if (exts.find(fileExt) != exts.end()) return true;
	}

	return false;
}

tstring FileUtil::getFileExtension(const tstring& strIn)
{
	size_t nPos = strIn.rfind('.');
	if (nPos != tstring::npos)
	{
		return strIn.substr(nPos + 1, tstring::npos);
	}

	return EMPTY_STRING;
}

tstring FileUtil::getFileDirectory(const tstring& strIn)
{
	size_t nPos = strIn.rfind('/');
	if (nPos != tstring::npos)
	{
		return formatFileDir(strIn.substr(0, nPos));
	}

	nPos = strIn.rfind('\\');
	if (nPos != tstring::npos)
	{
		return formatFileDir(strIn.substr(0, nPos));
	}

	return EMPTY_STRING;
}

tstring FileUtil::getFileName(const tstring& strIn)
{
	size_t nPosBegin = strIn.rfind('/');
	if (nPosBegin == tstring::npos)
	{
		nPosBegin = strIn.rfind('\\');
		if (nPosBegin == tstring::npos) nPosBegin = -1;
	}
	++nPosBegin;

	size_t nPosEnd = strIn.length();
	return strIn.substr(nPosBegin, nPosEnd);
}

tstring FileUtil::getFileNameWithoutExtension(const tstring& strIn)
{
	size_t nPosBegin = strIn.rfind('/');
	if (nPosBegin == tstring::npos)
	{
		nPosBegin = strIn.rfind('\\');
		if (nPosBegin == tstring::npos) nPosBegin = -1;
	}
	++nPosBegin;

	size_t nPosEnd = strIn.rfind('.');
	if (nPosEnd == tstring::npos) nPosEnd = strIn.length();

	return strIn.substr(nPosBegin, nPosEnd - nPosBegin);
}

tstring FileUtil::getFileNameWithExtension(const tstring& strIn, const tstring& strExt)
{
	size_t nPos = strIn.rfind('.');
	if (nPos != tstring::npos)
	{
		return strIn.substr(0, nPos) + strExt;
	}

	return EMPTY_STRING;
}

tstring FileUtil::combinePath(const tstring& path1, const tstring& path2)
{
	if (path1.length() > 0)
	{
		const char lastChar = path1[path1.length() - 1];
		if (lastChar == '/' || lastChar == '\\')
		{
			return (path1 + path2);
		}
		else
		{
			return (path1 + '/') + path2;
		}
	}
	else
	{
		return path2;
	}
}

bool FileUtil::renameFile(const tstring& strOldFile, const tstring& strNewFile)
{
	return(rename(strOldFile.c_str(), strNewFile.c_str()) == 0);
}

tstring FileUtil::formatFilePath(const tstring& strPath)
{
	tstring strOut = strPath;
	for (auto& ch : strOut)
	{
		if (ch == '\\') ch = '/';
	}

	return strOut;
}

tstring FileUtil::formatFileDir(const tstring& strDir)
{
	tstring strOut = formatFilePath(strDir);

	if (strOut.rbegin() != strOut.rend() && (*strOut.rbegin()) != '/')
	{
		strOut += "/";
	}

	return strOut;
}

tstring FileUtil::getRelativePath(const tstring& src, const tstring& dest)
{
	tstring formatedSrc = formatFilePath(src);
	tstring formatedDest = formatFilePath(dest);

	StringList dirSrcArray;
	StringUtil::splitString(dirSrcArray, formatedSrc, "/");

	StringList dirDestArray;
	StringUtil::splitString(dirDestArray, formatedDest, "/");

	while (dirSrcArray.size() > 0 && dirDestArray.size() > 0 && dirSrcArray[0] == dirDestArray[0])
	{
		dirSrcArray.erase(dirSrcArray.begin());
		dirDestArray.erase(dirDestArray.begin());
	}

	for (size_t i = 1; i < dirSrcArray.size(); ++i)
	{
		dirDestArray.insert(dirDestArray.begin(), "..");
	}

	return StringUtil::joinString(dirDestArray, "/");
}

tstring FileUtil::getAbsolutePath(const tstring& baseDir, const tstring& relativeFilePath)
{
    return getAbsolutePath(formatFileDir(baseDir) + relativeFilePath);
}

tstring FileUtil::getAbsolutePath(const tstring& filePath)
{
    tstring formatedPath = formatFilePath(filePath);

    StringList pathParts;
    StringUtil::splitString(pathParts, formatedPath, "/");

    int index = 0;
    while (index < static_cast<int>(pathParts.size()))
    {
        if (pathParts[index] == "" || pathParts[index] == ".")
        {
            pathParts.erase(pathParts.begin() + index);
        }
        else if (pathParts[index] == "..")
        {
            int prevIndex = index - 1;
            if (prevIndex < 0) prevIndex = 0;

            pathParts.erase(pathParts.begin() + prevIndex, pathParts.begin() + index + 1);

            index = prevIndex;
        }
        else
        {
            ++index;
        }
    }

    return StringUtil::joinString(pathParts, "/");
}

int FileUtil::enumlateFiles(StringSet& filesOut, const tstring& dir, bool recursive)
{
	int numFilesFound = 0;

	DIR* pDir = opendir(dir.c_str());
	if (pDir)
	{
		/* print all the files and directories within directory */
		dirent* pEnt = nullptr;
		while ((pEnt = readdir(pDir)) != nullptr)
		{
			if (pEnt->d_type == DT_DIR)
			{
				if (CURRENT_DIR != pEnt->d_name && PARENT_DIR != pEnt->d_name)
				{
					tstring currDir = combinePath(dir, pEnt->d_name);
					if (recursive)
					{
						numFilesFound += enumlateFiles(filesOut, currDir, recursive);
					}
					else
					{
						filesOut.insert(currDir);
						++numFilesFound;
					}
				}
			}
			else if (pEnt->d_type == DT_REG)
			{
				tstring currFilePath = combinePath(dir, pEnt->d_name);
				filesOut.insert(currFilePath);
				++numFilesFound;
			}
		}
		closedir(pDir);
	}

	return numFilesFound;
}

NS_END
