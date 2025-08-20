/*!
* \file S3DataStream.h
* \adopted from S3 engine
* \author kechen
* \date 2018/11/22
*
*
*/
#pragma once

#include "../BaseType.h"
#include "S3Referenced.h"
#include <algorithm>

NS_BEGIN

class CDataStream : public CReferenced
{
public:
	enum AccessMode
	{
		AM_READ = (1<<0), 
		AM_WRITE = (1<<1),
	};

public:
	CDataStream(uint16 eAccessMode = AM_READ)
	{
		m_nSize = 0;
		m_eAccessMode = eAccessMode;
		m_strName = "";
	}

	CDataStream(const char* pszName, uint16 eAccessMode = AM_READ)
	{
		m_nSize = 0;
		m_eAccessMode = eAccessMode;
		m_strName = pszName;
	}

	virtual ~CDataStream(void){}

public:
	// ---------------------------------------------------------------------
	// Get Attribute
	// ---------------------------------------------------------------------
	const char* GetName() const{return m_strName.c_str();}
	uint16 GetAccessMode() const{return m_eAccessMode;}
	uint32 GetSize() const{return m_nSize;}

	bool IsReadable() const{return (m_eAccessMode&AM_READ) != 0;}
	bool IsWritable() const{return (m_eAccessMode&AM_WRITE) != 0;}

	// ---------------------------------------------------------------------
	// virtual function
	// ---------------------------------------------------------------------
	// Read the requisite number of bytes from the stream, stopping at the end of the file.
	virtual uint32 Read(void* pBuffer, uint32 nCount) = 0;

	// Write the requisite number of bytes from the stream (only applicable to streams that are not read-only)
	virtual uint32 Write(const void* pBuffer, uint32 nCount) = 0;

	/** Get a single line from the stream.
	@remarks
		The delimiter character is not included in the data
		returned, and it is skipped over so the next read will occur
		after it. The buffer contents will include a
		terminating character.
    @note
        If you used this function, you <b>must</b> open the stream in <b>binary mode</b>,
        otherwise, it'll produce unexpected results.
	@param buf Reference to a buffer pointer
	@param maxCount The maximum length of data to be read, excluding the terminating character
	@param delim The delimiter to stop at
	@returns The number of bytes read, excluding the terminating character
	*/
	virtual uint32 ReadLine(char* buf, uint32 maxCount, const char* delim = "\n")
	{
		// Deal with both Unix & Windows LFs
		bool trimCR = false;
		if (tstring(delim).find_first_of('\n') != tstring::npos)
		{
			trimCR = true;
		}

		char tmpBuf[128];
		uint32 chunkSize = std::min(maxCount, (uint32)128-1);
		uint32 totalCount = 0;
		uint32 readCount; 
		while (chunkSize && (readCount = Read(tmpBuf, chunkSize)) != 0)
		{
			// Terminate
			tmpBuf[readCount] = '\0';

			// Find first delimiter
			uint32 pos = (uint32)strcspn(tmpBuf, delim);

			if (pos < readCount)
			{
				// Found terminator, reposition backwards
				Skip((long)(pos + 1 - readCount));
			}

			// Are we genuinely copying?
			if (buf)
			{
				memcpy(buf+totalCount, tmpBuf, pos);
			}
			totalCount += pos;

			if (pos < readCount)
			{
				// Trim off trailing CR if this was a CR/LF entry
				if (trimCR && totalCount && buf && buf[totalCount-1] == '\r')
				{
					--totalCount;
				}

				// Found terminator, break out
				break;
			}

			// Adjust chunkSize for next time
			chunkSize = std::min(maxCount-totalCount, (uint32)128-1);
		}

		// Terminate
		if(buf)
			buf[totalCount] = '\0';

		return totalCount;
	}

	/** Skip a defined number of bytes. This can also be a negative value, in which case
	the file pointer rewinds a defined number of bytes*/
	virtual void Skip(int nCount) = 0;

	// Repositions the read point to a specified byte.
	virtual void Seek(uint32 nPos) = 0;

	// Returns the current byte offset from beginning
	virtual uint32 Tell() const = 0;

	// Returns true if the stream has reached the end.
	virtual bool Eof() const = 0;

	// Close the stream; this makes further operations invalid.
	virtual void Close() = 0;

	// ---------------------------------------------------------------------
	// Extend function
	// ---------------------------------------------------------------------
	 
protected:
	uint32 m_nSize;
	uint16 m_eAccessMode;
	tstring m_strName;

	#define OGRE_STREAM_TEMP_SIZE 128
};

NS_END
