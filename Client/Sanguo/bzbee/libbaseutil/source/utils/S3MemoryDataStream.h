/*!
* \file S3MemoryDataStream.h
* \adopted from S3 engine
* \author kechen
* \date 2018/11/22
*
*
*/
#pragma once

#include "S3DataStream.h"
NS_BEGIN

class CMemoryDataStream : public CDataStream
{
public:
	CMemoryDataStream(void* pMem, uint32 nSize, bool bReadOnly, bool bFreeMemOnClose);
	CMemoryDataStream(const char* pszName, void* pMem, uint32 nSize, bool bReadOnly, bool bFreeMemOnClose);
	CMemoryDataStream(CDataStream* srcStream, bool bReadOnly);
	CMemoryDataStream(const char* pszName, CDataStream* srcStream, bool bReadOnly);
	CMemoryDataStream(const char* pszName, CDataStream* srcStream, uint32 nSize, bool bReadOnly);
	CMemoryDataStream(uint32 size, bool readOnly);
	~CMemoryDataStream(void);

	// ---------------------------------------------------------------------
	// virtual function
	// ---------------------------------------------------------------------
	// Read the requisite number of bytes from the stream, stopping at the end of the file.
	virtual uint32 Read(void* pBuffer, uint32 nCount);

	// Write the requisite number of bytes from the stream (only applicable to streams that are not read-only)
	virtual uint32 Write(const void* pBuffer, uint32 nCount);

	virtual uint32 ReadLine(char* buf, uint32 maxCount, const char* delim = "\n");

	/** Skip a defined number of bytes. This can also be a negative value, in which case
	the file pointer rewinds a defined number of bytes*/
	virtual void Skip(int nCount);

	// Repositions the read point to a specified byte.
	virtual void Seek(uint32 nPos);

	// Returns the current byte offset from beginning
	virtual uint32 Tell() const;

	// Returns true if the stream has reached the end.
	virtual bool Eof() const;

	// Close the stream; this makes further operations invalid.
	virtual void Close();

	// ---------------------------------------------------------------------
	// Self
	// ---------------------------------------------------------------------
public:
	uint8* GetPtr() const{return m_pData;}
	uint8* GetCurrentPtr() const{return m_pPos;}
    uint8* GetEndPtr() const{return m_pEnd;}
private:
	// Pointer to the start of the data area
	uint8* m_pData;

	// Pointer to the current position in the memory
	uint8* m_pPos;

	// Pointer to the end of the memory
	uint8* m_pEnd;

	// Do we delete the memory on close
	bool m_bFreeOnClose;
};

ref_ptr<CMemoryDataStream> CreateMemoryDataStream(const char* pszName, CDataStream* srcStream, uint32 nSize, bool bReadOnly);
ref_ptr<CMemoryDataStream> CreateMemoryDataStream(const char* pszName, void* pMem, uint32 nSize, bool bReadOnly, bool bFreeMemOnClose);
ref_ptr<CMemoryDataStream> CreateMemoryDataStream(uint32 size, bool readOnly);

NS_END