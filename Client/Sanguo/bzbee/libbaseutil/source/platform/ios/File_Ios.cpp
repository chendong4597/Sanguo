/*!
 * \file File_Ios.cpp
 *
 * \author zjhlogo
 * \date 6/27/2019
 *
 *
 */
#include "File_Ios.h"
#include "../../LogUtil.h"
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>

NS_BEGIN

File_Ios::File_Ios()
{
    
}

File_Ios::~File_Ios()
{
    close();
}

bool File_Ios::open(const tstring& filePath, int flag)
{
    tstring strMode;
    if (flag & AM_APPEND) strMode += "a";
    if (flag & AM_READ) strMode += "r";
    if (flag & AM_WRITE) strMode += "w";
    strMode += "b";
    if (flag & AM_PLUS) strMode += "+";
    
    m_pFile = fopen(filePath.c_str(), strMode.c_str());
    if (!m_pFile)
    {
        LOGE("open file failed {0}, errno: {1}.", filePath, errno);
        return false;
    }
    
    m_mode = flag;
    
    struct stat sb;
    if (stat(filePath.c_str(), &sb) == 0 && S_ISREG(sb.st_mode))
    {
        m_fileSize = sb.st_size;
    }
    
    m_filePath = filePath;
    
    return true;
}

void File_Ios::close()
{
    if (m_pFile)
    {
        fclose(m_pFile);
        m_pFile = nullptr;
    }
}

int64 File_Ios::seek(int64 offset, SeekType seekType /* = SeekType::Current */)
{
    int seekFrom = SEEK_CUR;
    if (seekType == SeekType::Begin) seekFrom = SEEK_SET;
    else if (seekType == SeekType::End) seekFrom = SEEK_END;
    
    return fseek(m_pFile, static_cast<size_t>(offset), seekFrom);
}

int64 File_Ios::tell()
{
    return ftell(m_pFile);
}

int64 File_Ios::read(void* dataOut, int64 size)
{
    if ((m_mode & (AM_READ | AM_PLUS)) == 0) return 0;
    if (size <= 0) return 0;
    
    return fread(dataOut, 1, static_cast<size_t>(size), m_pFile);
}

int64 File_Ios::read(BufferData& dataOut, int64 size)
{
    if ((m_mode & (AM_READ | AM_PLUS)) == 0) return 0;
    if (size <= 0) return 0;
    
    dataOut.resize(static_cast<size_t>(size));
    return fread(dataOut.data(), 1, static_cast<size_t>(size), m_pFile);
}

int64 File_Ios::write(const void* data, int64 size)
{
    if ((m_mode & (AM_APPEND | AM_WRITE | AM_PLUS)) == 0) return 0;
    if (size <= 0) return 0;
    
    return fwrite(data, 1, static_cast<size_t>(size), m_pFile);
}

int64 File_Ios::write(const BufferData& data)
{
    if ((m_mode & (AM_APPEND | AM_WRITE | AM_PLUS)) == 0) return 0;
    
    return fwrite(data.data(), 1, data.size(), m_pFile);
}

void File_Ios::flush(void)
{
	fflush(m_pFile);
}

bool File_Ios::setFileSize(int64 size)
{
	if ((m_mode & (AM_WRITE | AM_PLUS)) == 0) return false;

	if (fseek(m_pFile, size, SEEK_SET) != 0) return false;
	if (ftruncate(fileno(m_pFile), size) != 0) return false;

	m_fileSize = size;
	return true;
}

NS_END
