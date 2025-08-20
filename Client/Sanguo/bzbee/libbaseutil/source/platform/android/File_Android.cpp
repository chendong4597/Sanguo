/*!
 * \file File_Android.cpp
 *
 * \author zjhlogo
 * \date 24/04/2016
 *
 * 
 */
#include "File_Android.h"
#include "../../LogUtil.h"
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>

NS_BEGIN

File_Android::File_Android()
{

}

File_Android::~File_Android()
{
	close();
}

bool File_Android::open(const tstring& filePath, int flag /*= AM_READ*/)
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

void File_Android::close()
{
    if (m_pFile)
    {
        fclose(m_pFile);
        m_pFile = nullptr;
    }
}

I64 File_Android::seek(I64 offset, SeekType seekType /* = SeekType::Current */)
{
	int seekFrom = SEEK_CUR;
	if (seekType == SeekType::Begin) seekFrom = SEEK_SET;
	else if (seekType == SeekType::End) seekFrom = SEEK_END;

	return fseek(m_pFile, static_cast<size_t>(offset), seekFrom);
}

I64 File_Android::tell()
{
	return ftell(m_pFile);
}

I64 File_Android::read(void* dataOut, I64 size)
{
	if ((m_mode & (AM_READ | AM_PLUS)) == 0) return 0;
	if (size <= 0) return 0;

	return fread(dataOut, 1, static_cast<size_t>(size), m_pFile);
}

I64 File_Android::read(BufferData& dataOut, I64 size)
{
	if ((m_mode & (AM_READ | AM_PLUS)) == 0) return 0;
	if (size <= 0) return 0;

	dataOut.resize(static_cast<size_t>(size));
	return fread(dataOut.data(), 1, static_cast<size_t>(size), m_pFile);
}

I64 File_Android::write(const void* data, I64 size)
{
	if ((m_mode & (AM_APPEND | AM_WRITE | AM_PLUS)) == 0) return 0;
	if (size <= 0) return 0;

	return fwrite(data, 1, static_cast<size_t>(size), m_pFile);
}

I64 File_Android::write(const BufferData& data)
{
	if ((m_mode & (AM_APPEND | AM_WRITE | AM_PLUS)) == 0) return 0;

	return fwrite(data.data(), 1, data.size(), m_pFile);
}

void File_Android::flush(void)
{
	fflush(m_pFile);
}

bool File_Android::setFileSize(I64 size)
{
	if ((m_mode & (AM_WRITE | AM_PLUS)) == 0) return false;

	if (fseek(m_pFile, size, SEEK_SET) != 0) return false;
	if (ftruncate(fileno(m_pFile), size) != 0) return false;

	m_fileSize = size;
	return true;
}

NS_END
