/*!
 * \file BufferStream.cpp
 * \date 10-17-2014 13:41:52
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#include "BufferStream.h"
#include <memory>

NS_BEGIN

BufferStreamReader::BufferStreamReader(const BufferData& bufferData)
{
	m_bufferData = bufferData;
}

BufferStreamReader::~BufferStreamReader()
{
	// TODO: 
}

bool BufferStreamReader::read(void* pDataOut, std::size_t size)
{
	if (!pDataOut || size + m_readPos > m_bufferData.size()) return false;
	if (size <= 0) return true;

	memcpy(pDataOut, m_bufferData .data()+m_readPos, size);
	m_readPos += size;

	return true;
}

bool BufferStreamReader::skip(int size)
{
	if (size + m_readPos > m_bufferData.size()) return false;
	m_readPos += size;
	return true;
}


BufferStreamWriter::BufferStreamWriter(std::size_t initSize, std::size_t stepSize)
	:m_stepSize(stepSize)
{
	if (initSize > 0) m_bufferData.resize(initSize);
}

BufferStreamWriter::~BufferStreamWriter()
{
	m_bufferData.clear();
}

bool BufferStreamWriter::write(const void* pData, std::size_t size)
{
	if (!pData || size <= 0) return false;

	if (m_currentPos + size > m_bufferData.size())
	{
		std::size_t adjustStep = ((size + m_stepSize - 1) / m_stepSize) * m_stepSize;
		m_bufferData.resize(m_bufferData.size() + adjustStep);
		if (m_currentPos + size > m_bufferData.size()) return false;
	}

	memcpy(&m_bufferData[m_currentPos], pData, size);
	m_currentPos += size;

	return true;
}

void BufferStreamWriter::reset()
{
	m_bufferData.clear();
	m_currentPos = 0;
}

MemBuffer::MemBuffer(uint uiSize /* = 0 */)

{
	if (uiSize == 0) return;

	m_uiMemSize = uiSize;
	m_pMemBuffer = make_shared_array<char>(m_uiMemSize);
	// remove the memory reset for optimize
	//    memset(m_pMemBuffer, 0, sizeof(char)*m_uiMemSize);
}

MemBuffer::MemBuffer(MemBuffer& srcMemBuffer)
{
	*this = srcMemBuffer;
}

MemBuffer::~MemBuffer()
{
	clear();
}

MemBuffer& MemBuffer::operator=(MemBuffer& srcMemBuffer)
{
	clear();
	m_uiMemSize = srcMemBuffer.m_uiMemSize;
	m_pMemBuffer = srcMemBuffer.m_pMemBuffer;
	return *this;
}

bool MemBuffer::resize(uint uiSize)
{
	clear();
	if (uiSize == 0) return false;

	m_uiMemSize = uiSize;
	m_pMemBuffer = make_shared_array<char>(m_uiMemSize);
	return true;
}

void MemBuffer::clear()
{
	m_pMemBuffer.reset();
}


NS_END
