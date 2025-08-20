/*!
 * \file File_Win32.cpp
 *
 * \author zjhlogo
 * \date 24/04/2016
 *
 * 
 */
#include "File_Win32.h"
#include "../../LogUtil.h"
#include <dirent_win32.h>
#include <io.h>

NS_BEGIN

File_Win32::File_Win32()
{

}

File_Win32::~File_Win32()
{
	close();
}

bool File_Win32::open(const tstring& filePath, int flag /*= AM_READ*/)
{
	tstring strMode;
    if (flag & AM_APPEND) strMode += "a";
	if (flag & AM_READ) strMode += "r";
	if (flag & AM_WRITE) strMode += "w";
    strMode += "b";
    if (flag & AM_PLUS) strMode += "+";

    m_pFile = _fsopen(filePath.c_str(), strMode.c_str(), _SH_DENYWR);
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

void File_Win32::close()
{
    if (m_pFile)
    {
        fclose(m_pFile);
        m_pFile = nullptr;
    }
}

I64 File_Win32::seek(I64 offset, SeekType seekType /* = SeekType::Current */)
{
	int seekFrom = SEEK_CUR;
	if (seekType == SeekType::Begin) seekFrom = SEEK_SET;
	else if (seekType == SeekType::End) seekFrom = SEEK_END;

	return fseek(m_pFile, static_cast<size_t>(offset), seekFrom);
}

I64 File_Win32::tell()
{
	if (!m_pFile) return 0;

	return ftell(m_pFile);
}

I64 File_Win32::read(void* dataOut, I64 size)
{
	if ((m_mode & (AM_READ | AM_PLUS)) == 0) return 0;
	if (size <= 0) return 0;

	return fread(dataOut, 1, static_cast<size_t>(size), m_pFile);
}

I64 File_Win32::read(BufferData& dataOut, I64 size)
{
	if ((m_mode & (AM_READ | AM_PLUS)) == 0) return 0;
	if (size <= 0) return 0;

	dataOut.resize(static_cast<size_t>(size));
	return fread(dataOut.data(), 1, static_cast<size_t>(size), m_pFile);
}

I64 File_Win32::write(const void* data, I64 size)
{
	if ((m_mode & (AM_APPEND | AM_WRITE | AM_PLUS)) == 0) return 0;
	if (size <= 0) return 0;

	return fwrite(data, 1, static_cast<size_t>(size), m_pFile);
}

I64 File_Win32::write(const BufferData& data)
{
	if ((m_mode & (AM_APPEND | AM_WRITE | AM_PLUS)) == 0) return 0;

	return fwrite(data.data(), 1, data.size(), m_pFile);
}

void File_Win32::flush(void)
{
	fflush(m_pFile);
}

bool File_Win32::setFileSize(I64 size)
{
	if ((m_mode & (AM_WRITE | AM_PLUS)) == 0) return false;

	if (_fseeki64(m_pFile, size, SEEK_SET) != 0) return false;
	if (!SetEndOfFile(reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(m_pFile))))) return false;

	m_fileSize = size;
	return true;
}

NS_END
