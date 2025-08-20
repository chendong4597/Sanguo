/*!
 * \file File.h
 *
 * \author zjhlogo
 * \date 24/04/2016
 *
 * 
 */
#pragma once
#include "../BaseType.h"

NS_BEGIN

class File
{
public:
	// access mode
    static const int AM_APPEND = 1 << 0;
    static const int AM_READ = 1 << 1;
	static const int AM_WRITE = 1 << 2;
    static const int AM_PLUS = 1 << 3;

	enum class SeekType
	{
		Current = 0,
		Begin,
		End,
	};

public:
    virtual ~File() {};

	virtual void close() = 0;

    virtual I64 seek(I64 offset, SeekType seekType = SeekType::Current) = 0;
    virtual I64 tell() = 0;

    virtual I64 read(void* dataOut, I64 size) = 0;
    virtual I64 read(BufferData& dataOut, I64 size) = 0;

    virtual I64 write(const void* data, I64 size) = 0;
    virtual I64 write(const BufferData& data) = 0;

	virtual void flush(void) {}

	I64 getFileSize() const { return m_fileSize; }
	virtual bool setFileSize(I64 size) { assert(false); return false; }

protected:
    tstring m_filePath;
	I64 m_fileSize{};
	int m_mode{ AM_READ };

};

NS_END
