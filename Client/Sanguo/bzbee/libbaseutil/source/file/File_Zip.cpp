/*!
 * \file File_Zip.cpp
 *
 * \author zjhlogo
 * \date 2019/06/21
 *
 * 
 */
#include "File_Zip.h"

NS_BEGIN

bool File_Zip::open(unzFile zipPackage, const tstring& filePath, unz_file_pos& filePos)
{
    if (UNZ_OK != unzGoToFilePos(zipPackage, &filePos)) return false;

    unz_file_info fileInfo;
    if (UNZ_OK != unzGetCurrentFileInfo(zipPackage, &fileInfo, nullptr, 0, nullptr, 0, nullptr, 0)) return false;

    if (UNZ_OK != unzOpenCurrentFile(zipPackage)) return false;

    m_buffer.resize(fileInfo.uncompressed_size);
    unzReadCurrentFile(zipPackage, m_buffer.data(), fileInfo.uncompressed_size);
    unzCloseCurrentFile(zipPackage);

    m_filePath = filePath;
    m_mode = AM_READ;
    m_fileSize = fileInfo.uncompressed_size;

    return true;
}

bool File_Zip::readFileIntoBuffer(BufferData& bufferOut, unzFile zipPackage, const tstring& filePath, unz_file_pos& filePos)
{
    if (UNZ_OK != unzGoToFilePos(zipPackage, &filePos)) return false;

    unz_file_info fileInfo;
    if (UNZ_OK != unzGetCurrentFileInfo(zipPackage, &fileInfo, nullptr, 0, nullptr, 0, nullptr, 0)) return false;

    if (UNZ_OK != unzOpenCurrentFile(zipPackage)) return false;

    bufferOut.resize(fileInfo.uncompressed_size);
    unzReadCurrentFile(zipPackage, bufferOut.data(), fileInfo.uncompressed_size);
    unzCloseCurrentFile(zipPackage);

    return true;
}

void File_Zip::close()
{
    // nothing to do
}

I64 File_Zip::seek(I64 offset, SeekType seekType /*= SeekType::Current*/)
{
    switch (seekType)
    {
    case File::SeekType::Current:
        m_currentPos += offset;
        break;
    case File::SeekType::Begin:
        m_currentPos = offset;
        break;
    case File::SeekType::End:
        m_currentPos = m_fileSize;
        break;
    default:
        break;
    }

    if (m_currentPos < 0) m_currentPos = 0;
    if (m_currentPos > m_fileSize) m_currentPos = m_fileSize;

    return 0;
}

I64 File_Zip::tell()
{
    return m_currentPos;
}

I64 File_Zip::read(BufferData& dataOut, I64 size)
{
    if ((m_mode & AM_READ) == 0) return 0;
    if (m_fileSize <= 0) return 0;
    if (size + m_currentPos > m_buffer.size()) return 0;

    dataOut.resize(static_cast<size_t>(size));
    memcpy(dataOut.data(), m_buffer.data() + m_currentPos, static_cast<size_t>(size));
    m_currentPos += size;

    return size;
}

I64 File_Zip::read(void* dataOut, I64 size)
{
    if ((m_mode & AM_READ) == 0) return 0;
    if (m_fileSize <= 0) return 0;
    if (size + m_currentPos > m_buffer.size()) return 0;

    memcpy(dataOut, m_buffer.data() + m_currentPos, static_cast<size_t>(size));
    m_currentPos += size;

    return size;
}

I64 File_Zip::write(const BufferData& data)
{
    return 0;
}

I64 File_Zip::write(const void* data, I64 size)
{
    return 0;
}

NS_END
