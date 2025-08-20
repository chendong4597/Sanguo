/*!
 * \file File_Zip.h
 *
 * \author zjhlogo
 * \date 2019/06/21
 *
 * 
 */
#pragma once
#include "File.h"
#include <unzip.h>

NS_BEGIN

class File_Zip : public File
{
public:
    File_Zip() {};
    virtual ~File_Zip() {};

    bool open(unzFile zipPackage, const tstring& filePath, unz_file_pos& filePos);
    static bool readFileIntoBuffer(BufferData& bufferOut, unzFile zipPackage, const tstring& filePath, unz_file_pos& filePos);
    virtual void close() override;

    virtual I64 seek(I64 offset, SeekType seekType = SeekType::Current) override;
    virtual I64 tell() override;

    virtual I64 read(void* dataOut, I64 size) override;
    virtual I64 read(BufferData& dataOut, I64 size) override;

    virtual I64 write(const void* data, I64 size) override;
    virtual I64 write(const BufferData& data) override;

private:
    BufferData m_buffer;
    I64 m_currentPos{};

};

NS_END
