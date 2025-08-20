/*!
 * \file File_Ios.h
 *
 * \author zjhlogo
 * \date 6/27/2019
 *
 *
 */
#pragma once

#include "../../file/File.h"

NS_BEGIN

class File_Ios : public File
{
public:
    File_Ios();
    virtual ~File_Ios();
    
    bool open(const tstring& filePath, int flag = AM_READ);
    virtual void close() override;
    
    virtual int64 seek(int64 offset, SeekType seekType = SeekType::Current) override;
    virtual int64 tell() override;
    
    virtual int64 read(void* dataOut, int64 size) override;
    virtual int64 read(BufferData& dataOut, int64 size) override;
    
    virtual int64 write(const void* data, int64 size) override;
    virtual int64 write(const BufferData& data) override;

	virtual void flush(void) override;
    
	virtual bool setFileSize(int64 size) override;

private:
    FILE* m_pFile{};
};

NS_END
