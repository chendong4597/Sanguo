/*!
 * \file FileUtil.h
 * \date 7-18-2012 12:55:49
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#pragma once

#include "BaseType.h"
#include "file/FileSystem.h"
#include <future>

NS_BEGIN

class FileUtil
{
public:
	static const tstring CURRENT_DIR;
	static const tstring PARENT_DIR;

public:
	static bool readFileIntoString(tstring& stringOut, const tstring& filePath, int storage = 0);
	static std::future<tstring> asyncReadFileIntoString(const tstring& filePath, int storage = 0);

    static bool writeStringIntoFile(const tstring& stringIn, const tstring& filePath, int storage = SubFileSystem::FS_EXTERNAL);
    static bool writeBufferIntoFile(const void* bufferData, int bufferSize, const tstring& filePath, int storage = SubFileSystem::FS_EXTERNAL);

	static size_t listFilesInDirectory(StringSet& filesOut, const tstring& dir, bool recursive, const StringSet& includeExts = StringSet(), const StringSet& excludeExts = StringSet());
	static size_t filterFilesByExtension(StringSet& filesInOut, const StringSet& includeExts = StringSet(), const StringSet& excludeExts = StringSet());

	static bool isFileExtension(const tstring& filePath, const StringSet& exts, bool caseSensitive = false);

	static tstring getFileExtension(const tstring& strIn);
	static tstring getFileDirectory(const tstring& strIn);
	static tstring getFileName(const tstring& strIn);
	static tstring getFileNameWithoutExtension(const tstring& strIn);
	static tstring getFileNameWithExtension(const tstring& strIn, const tstring& strExt);

	static tstring combinePath(const tstring& path1, const tstring& path2);

	static bool renameFile(const tstring& strOldFile, const tstring& strNewFile);
	static tstring formatFilePath(const tstring& strPath);
	static tstring formatFileDir(const tstring& strDir);

	static tstring getRelativePath(const tstring& src, const tstring& dest);
	static tstring getAbsolutePath(const tstring& baseDir, const tstring& relativeFilePath);
    static tstring getAbsolutePath(const tstring& filePath);

	static const tstring& getMainObbPath() { return m_mainObbPath; }
	static void setMainObbPath(const tstring& path) { m_mainObbPath = path; }

	static const tstring& getPatchObbPath() { return m_patchObbPath; }
	static void setPatchObbPath(const tstring& path) { m_patchObbPath = path; }
	
	static int getObbVersionCode() { return m_obbVersionCode; }
	static void setObbVersionCode(int obbVersionCode) { m_obbVersionCode = obbVersionCode; }

private:
	static int enumlateFiles(StringSet& filesOut, const tstring& dir, bool recursive);

private:
	static tstring m_writableDir;
	static tstring m_writableAssetsDir;
	static tstring m_revisionUpdateDir;

	static tstring m_mainObbPath;
	static tstring m_patchObbPath;
	static int m_obbVersionCode;

};

NS_END
