/*!
 * \file AndroidFileSystem.cpp
 *
 * \author zjhlogo
 * \date 2019/06/24
 *
 *
 */
#include "AndroidFileSystem.h"
#include "File_Android.h"
#include "../../FileUtil.h"
#include "../../LogUtil.h"
#include "../../StringUtil.h"
#include <sys/stat.h>
#include <dirent.h>

NS_BEGIN

AndroidFileSystem::AndroidFileSystem(const tstring& rootDir, int prop /* = FP_READABLE */, int storage /* = 0 */, const PathRedirectCallback& cb /* = nullptr */)
    :SubFileSystem(prop, storage, cb)
    , m_rootDir(rootDir)
{

}

AndroidFileSystem::~AndroidFileSystem()
{

}

bool AndroidFileSystem::getFileLocation(tstring& filePath, long& offset, long& length)
{
	if (searchFile(filePath))
	{
		filePath = FileUtil::combinePath(m_rootDir, filePath);
		offset = length = 0;
		return true;
	}
	return false;
}

bool AndroidFileSystem::searchFile(const tstring & filePath)
{
    return isFile(filePath);
}

std::shared_ptr<File> AndroidFileSystem::openFile(const tstring & filePath, int flag)
{
    tstring fullPath = FileUtil::combinePath(m_rootDir, filePath);

    // if our storage is used for writting and we are creating writable file
    // just make sure the directories is created before creating file
    if (checkProperty(FP_WRITABLE) && (flag & (File::AM_WRITE | File::AM_APPEND)))
    {
        // creating directories
        makeDirectory(FileUtil::getFileDirectory(fullPath));
    }

    auto file = std::make_shared<File_Android>();
    if (file->open(fullPath, flag)) return file;

    // open failed
    LOGE("Open file failed: {}", fullPath);
    return nullptr;
}

bool AndroidFileSystem::deleteFile(const tstring & filePath)
{
    // delete file
    tstring fullPath = FileUtil::combinePath(m_rootDir, filePath);
    return (remove(fullPath.c_str()) == 0);
}

bool AndroidFileSystem::isFile(const tstring & filePath)
{
    tstring fullPath = FileUtil::combinePath(m_rootDir, filePath);

    struct stat sb;
    if (stat(fullPath.c_str(), &sb) == 0 && S_ISREG(sb.st_mode))
    {
        return true;
    }

    return false;
}

bool AndroidFileSystem::isDirectory(const tstring & filePath)
{
    tstring fullPath = FileUtil::combinePath(m_rootDir, filePath);

    struct stat sb;
    if (stat(fullPath.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
    {
        return true;
    }

    return false;
}

bool AndroidFileSystem::makeDirectory(const tstring & fileDir)
{
    if (fileDir.empty() || fileDir == "/") return false;

    if (isDirectory(fileDir)) return true;

    StringList arrDir;
    int numDirs = StringUtil::splitString(arrDir, fileDir, "/");
    if (numDirs <= 0) return false;

    tstring strSpliter;
    tstring strFileDirToMake;
    for (int i = 0; i < numDirs; ++i)
    {
        strFileDirToMake += strSpliter;
        strFileDirToMake += arrDir[i];
        mkdir(strFileDirToMake.c_str(), 0777);
        strSpliter = "/";
    }

    return true;
}

NS_END
