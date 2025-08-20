/*!
 * \file File_ios.cpp
 *
 * \author lzj
 * \date 10/09/2018
 *
 * 
 */
#include <File.h>
#include "FileUtil.h"
#include "../../LogUtil.h"

#import <Foundation/Foundation.h>

NS_BEGIN

File::File()
{

}

File::File(const tstring& filePath, int flag /*= AM_READ*/)
{
	open(filePath, flag);
}

File::~File()
{
	close();
}

bool File::open(const tstring& filePath, int flag /*= AM_READ*/)
{
    tstring strMode;
    if (flag & AM_READ) strMode += "r";
    if (flag & AM_WRITE) strMode += "w";
    strMode += "b";
	FILE* pFile{};
	tstring realFilePath{ filePath };
	bool isAbsolutePath{ !filePath.empty() && filePath.front() == '/' };
	if (isAbsolutePath) {
		pFile = fopen(filePath.c_str(), strMode.c_str());
	} else {
		// Firstly, load file from revision update directory
		tstring revisionUpdateFilePath{ FileUtil::combinePath(FileUtil::getRevisionUpdateDir(), filePath) };
		auto&& fullFilePath = FileUtil::fullPathFromRelativePath(filePath);
		for (const auto& path : { revisionUpdateFilePath, fullFilePath }) {
			pFile = fopen(path.c_str(), strMode.c_str());
			if (pFile) {
				realFilePath = path;
				break;
			}
		}
	}
    if (!pFile)
    {
		LOGE("open file failed {0}, errno: {1}.", realFilePath, errno);
		return false;
    }

    m_mode = flag;
    m_fileHandler = pFile;
    m_fileSize = FileUtil::getFileSize(realFilePath);

    return true;
}

void File::close()
{
	if (m_fileHandler)
	{
		fclose((FILE*)m_fileHandler);
		m_fileHandler = nullptr;
	}
}

int64 File::seek(int64 offset, SeekType seekType /* = SeekType::Current */)
{
	if (!m_fileHandler) return 0;

	int seekFrom = SEEK_CUR;
	if (seekType == SeekType::Begin) seekFrom = SEEK_SET;
	else if (seekType == SeekType::End) seekFrom = SEEK_END;

	return fseek((FILE*)m_fileHandler, static_cast<size_t>(offset), seekFrom);
}

int64 File::tell()
{
	if (!m_fileHandler) return 0;

	return ftell((FILE*)m_fileHandler);
}

int64 File::read(void* dataOut, int64 size)
{
	if (!m_fileHandler) return 0;
	if ((m_mode & AM_READ) == 0) return 0;
	if (size <= 0) return 0;

	return fread(dataOut, 1, static_cast<size_t>(size), (FILE*)m_fileHandler);
}

int64 File::read(BufferData& dataOut, int64 size)
{
	if (!m_fileHandler) return 0;
	if ((m_mode & AM_READ) == 0) return 0;
	if (size <= 0) return 0;

	dataOut.resize(static_cast<size_t>(size));
	return fread(dataOut.data(), 1, static_cast<size_t>(size), (FILE*)m_fileHandler);
}

int64 File::write(const void* data, int64 size)
{
	if (!m_fileHandler) return 0;
	if ((m_mode & AM_WRITE) == 0) return 0;
	if (size <= 0) return 0;

	return fwrite(data, 1, static_cast<size_t>(size), (FILE*)m_fileHandler);
}

int64 File::write(const BufferData& data)
{
	if (!m_fileHandler) return 0;
	if ((m_mode & AM_WRITE) == 0) return 0;

	return fwrite(data.data(), 1, data.size(), (FILE*)m_fileHandler);
}

void File::unpack(const tstring&) {}

bool FileUtil::readOuterFileIntoString(tstring& stringOut, const tstring& filePath)
{
	return readFileIntoString(stringOut, filePath);
}

bool FileUtil::readOuterFileIntoBuffer(BufferData& bufferOut, const tstring& filePath)
{
	return readFileIntoBuffer(bufferOut, filePath);
}

bool FileUtil::writeStringIntoFile(const char* pszStringIn, const tstring& filePath, size_t len /* = -1 */)
{
	File file;
	if (!file.open(filePath, File::AM_WRITE)) return false;

	if (len == -1) len = strlen(pszStringIn);
	file.write(pszStringIn, len);
	file.close();

	return true;
}

const tstring FileUtil::fullPathFromRelativePath(const tstring& relativePath)
{
    NSString *fullpath = nil;
    NSString *relPath = [NSString stringWithUTF8String:relativePath.c_str()];
    
    // only if it is not an absolute path
    if( ! [relPath isAbsolutePath] ) {
        
        // pathForResource also searches in .lproj directories. issue #1230
        NSString *lastPathComponent = [relPath lastPathComponent];
        
        NSString *imageDirectory = [relPath stringByDeletingLastPathComponent];

        // search path from directory set by setResourceDirectory
        fullpath = [[NSBundle mainBundle] pathForResource:lastPathComponent
                                                   ofType:nil
                                              inDirectory:imageDirectory];
        if (fullpath == nil)
        {
            // search from root directory
            fullpath = [[NSBundle mainBundle] pathForResource:lastPathComponent
                                                       ofType:nil
                                                  inDirectory:imageDirectory];
        }
    }
    
    if (fullpath == nil)
    {
        fullpath = relPath;
    }
    
    return [fullpath UTF8String];
}

NS_END

