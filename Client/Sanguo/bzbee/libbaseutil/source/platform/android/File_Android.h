/*!
 * \file File_Android.h
 *
 * \author zjhlogo
 * \date 24/06/2019
 *
 * 
 */
#pragma once

#include "../../file/File.h"

NS_BEGIN

class File_Android : public File
{
public:
    File_Android();
    virtual ~File_Android();

    bool open(const tstring& filePath, int flag = AM_READ);
    virtual void close() override;

    virtual I64 seek(I64 offset, SeekType seekType = SeekType::Current) override;
    virtual I64 tell() override;

    virtual I64 read(void* dataOut, I64 size) override;
    virtual I64 read(BufferData& dataOut, I64 size) override;

    virtual I64 write(const void* data, I64 size) override;
    virtual I64 write(const BufferData& data) override;

	virtual void flush(void) override;

	virtual bool setFileSize(I64 size) override;

private:
    FILE* m_pFile{};
};

NS_END
