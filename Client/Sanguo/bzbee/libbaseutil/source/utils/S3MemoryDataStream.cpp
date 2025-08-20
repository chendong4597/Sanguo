/*!
* \file S3MemoryDataStream.cpp
* \adopted from S3 engine
* \author kechen
* \date 2018/11/22
*
*
*/

#include "S3MemoryDataStream.h"

NS_BEGIN

CMemoryDataStream::CMemoryDataStream(void* pMem, uint32 nSize, bool bReadOnly, bool bFreeMemOnClose)
: CDataStream(bReadOnly?AM_READ:(AM_READ|AM_WRITE))
{
	m_pData = m_pPos = static_cast<uint8*>(pMem);
	m_nSize = nSize;
	m_pEnd = m_pData + m_nSize;
	ASSERT(m_pEnd >= m_pPos);
	m_bFreeOnClose = bFreeMemOnClose;
}

CMemoryDataStream::CMemoryDataStream(const char* pszName, void* pMem, uint32 nSize, bool bReadOnly, bool bFreeMemOnClose)
: CDataStream(pszName, bReadOnly?AM_READ:(AM_READ|AM_WRITE))
{
	m_pData = m_pPos = static_cast<uint8*>(pMem);
	m_nSize = nSize;
	m_pEnd = m_pData + m_nSize;
	ASSERT(m_pEnd >= m_pPos);
	m_bFreeOnClose = bFreeMemOnClose;
}

CMemoryDataStream::CMemoryDataStream(CDataStream* srcStream, bool bReadOnly)
: CDataStream(bReadOnly?AM_READ:(AM_READ|AM_WRITE))
{
	m_nSize = srcStream->GetSize();
	m_pData = new uint8[m_nSize+1];
    m_pData[m_nSize] = '\0';
	m_pPos = m_pData;

	srcStream->Seek(0);
	m_pEnd = m_pData + srcStream->Read(m_pData, m_nSize);
	ASSERT(m_pEnd >= m_pPos);
	m_bFreeOnClose = true;
}

CMemoryDataStream::CMemoryDataStream(const char* pszName, CDataStream* srcStream, bool bReadOnly)
: CDataStream(pszName, bReadOnly?AM_READ:(AM_READ|AM_WRITE))
{
	m_nSize = srcStream->GetSize();
	m_pData = new uint8[m_nSize+1];
    m_pData[m_nSize] = '\0';
	m_pPos = m_pData;

	srcStream->Seek(0);
	m_pEnd = m_pData + srcStream->Read(m_pData, m_nSize);
	ASSERT(m_pEnd >= m_pPos);
	m_bFreeOnClose = true;
}

CMemoryDataStream::CMemoryDataStream(const char* pszName, CDataStream* srcStream, uint32 nSize, bool bReadOnly)
: CDataStream(pszName, bReadOnly?AM_READ:(AM_READ|AM_WRITE))
{
	m_nSize = nSize;
	m_pData = new uint8[m_nSize+1];
    m_pData[m_nSize] = '\0';
	m_pPos = m_pData;

	srcStream->Seek(0);
	if (m_nSize > srcStream->GetSize())
	{
		uint32 uReadSize = srcStream->Read(m_pData, srcStream->GetSize());
		memset(m_pData + uReadSize, 0, m_nSize-uReadSize);
		m_pEnd = m_pData + m_nSize;
	}
	else
		m_pEnd = m_pData + srcStream->Read(m_pData, m_nSize);

	ASSERT(m_pEnd >= m_pPos);
	m_bFreeOnClose = true;
}

CMemoryDataStream::CMemoryDataStream(uint32 size, bool readOnly)
:CDataStream(static_cast<uint16>(readOnly ? AM_READ : (AM_READ | AM_WRITE)))
{
	m_nSize = size;
	m_pData = new uint8[m_nSize+1];
    m_pData[m_nSize] = '\0';
	m_pPos = m_pData;
	m_pEnd = m_pData + m_nSize;
	ASSERT(m_pEnd >= m_pPos);
	m_bFreeOnClose = true;
}

CMemoryDataStream::~CMemoryDataStream()
{
	this->Close();
}

// ---------------------------------------------------------------------
// virtual function
// ---------------------------------------------------------------------
// Read the requisite number of bytes from the stream, stopping at the end of the file.
uint32 CMemoryDataStream::Read(void* pBuffer, uint32 nCount)
{
	size_t cnt = nCount;
	// Read over end of memory?
	if (m_pPos + cnt > m_pEnd)
	{
		cnt = m_pEnd - m_pPos;
	}
	if (cnt == 0)
		return 0;

	ASSERT (cnt<= nCount);

	memcpy(pBuffer, m_pPos, cnt);
	m_pPos += cnt;
	return cnt;
}

// Write the requisite number of bytes from the stream (only applicable to streams that are not read-only)
uint32 CMemoryDataStream::Write(const void* pBuffer, uint32 nCount)
{
	size_t written = 0;
	if (IsWritable())
	{
		written = nCount;
		// we only allow writing within the extents of allocated memory
		// check for buffer overrun & disallow
		if (m_pPos + written > m_pEnd)
		{
			ASSERT(false);
			written = m_pEnd - m_pPos;
		}
		if (written == 0)
			return 0;

		memcpy(m_pPos, pBuffer, written);
		m_pPos += written;
	}
	return written;
}


uint32 CMemoryDataStream::ReadLine( char* buf, uint32 maxCount, const char* pszdelim /*= "\n"*/ )
{
	// Deal with both Unix & Windows LFs
	bool trimCR = false;
    tstring delim = pszdelim;
	if (delim.find_first_of('\n') != tstring::npos)
	{
		trimCR = true;
	}

	size_t pos = 0;

	// Make sure pos can never go past the end of the data 
	while (pos < maxCount && m_pPos < m_pEnd)
	{
		if (delim.find(*m_pPos) != tstring::npos)
		{
			// Trim off trailing CR if this was a CR/LF entry
			if (trimCR && pos && buf[pos-1] == '\r')
			{
				// terminate 1 character early
				--pos;
			}

			// Found terminator, skip and break out
			++m_pPos;
			break;
		}

		buf[pos++] = *m_pPos++;
	}

	// terminate
	buf[pos] = '\0';

	return pos;
}

/** Skip a defined number of bytes. This can also be a negative value, in which case
the file pointer rewinds a defined number of bytes*/
void CMemoryDataStream::Skip(int nCount)
{
	size_t newpos = (size_t)( ( m_pPos - m_pData ) + nCount );
	ASSERT( m_pData + newpos <= m_pEnd );        

	m_pPos = m_pData + newpos;
}

// Repositions the read point to a specified byte.
void CMemoryDataStream::Seek(uint32 nPos)
{
	ASSERT( m_pData + nPos <= m_pEnd );
	m_pPos = m_pData + nPos;
}

// Returns the current byte offset from beginning
uint32 CMemoryDataStream::Tell() const
{
	//mData is start, mPos is current location
	return m_pPos - m_pData;
}

// Returns true if the stream has reached the end.
bool CMemoryDataStream::Eof() const
{
	return m_pPos >= m_pEnd;
}

// Close the stream; this makes further operations invalid.
void CMemoryDataStream::Close()
{
	if (m_bFreeOnClose && m_pData)
	{
		SAFE_DELETE_ARRAY(m_pData);
	}
}

ref_ptr<CMemoryDataStream> CreateMemoryDataStream( const char* pszName, CDataStream* srcStream, uint32 nSize, bool bReadOnly /*= false*/ )
{
    return new CMemoryDataStream(pszName, srcStream, nSize, bReadOnly);
}

ref_ptr<CMemoryDataStream> CreateMemoryDataStream( const char* pszName, void* pMem, uint32 nSize, bool bReadOnly, bool bFreeMemOnClose )
{
    return new CMemoryDataStream(pszName, pMem, nSize, bReadOnly, bFreeMemOnClose);
}

ref_ptr<CMemoryDataStream> CreateMemoryDataStream( uint32 size, bool readOnly )
{
    return new CMemoryDataStream(size, readOnly);
}

NS_END