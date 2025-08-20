/*!
 * \file BufferStream.h
 * \date 10-17-2014 13:41:43
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#pragma once

#include "../BaseType.h" 
#include <memory>  

NS_BEGIN

class BufferStreamReader
{
public:
	BufferStreamReader(const BufferData& bufferData);
	virtual ~BufferStreamReader();

	bool read(void* pDataOut, std::size_t size);
	bool skip(int size);

	void reset() { m_readPos = 0; };
	std::size_t getSize() const { return m_bufferData.size() - m_readPos; };
	std::size_t getPosition() const { return m_readPos; };
	const uint8* getData() const { return m_bufferData .data()+m_readPos; }
	
private:
	BufferData m_bufferData;
	std::size_t m_readPos{};

};

class BufferStreamWriter
{
public:
	BufferStreamWriter(std::size_t initSize = 0, std::size_t stepSize = 1);
	virtual ~BufferStreamWriter();

	bool write(const void* pData, std::size_t size);
	void reset();

	const void* getData() const { return m_bufferData.data(); };
	std::size_t getSize() const { return m_currentPos; };
	const BufferData& getBuffData() const { return m_bufferData; }

private:
	std::size_t m_stepSize{};
	BufferData m_bufferData;
	std::size_t m_currentPos{};

};


template <typename T>
std::shared_ptr<T> make_shared_array(size_t size)
{
	//default_delete是STL中的默认删除器  
	return std::shared_ptr<T>(new T[size], std::default_delete<T[]>());
}
class MemBuffer
{
public:
	MemBuffer(uint uiSize = 0);
	MemBuffer(MemBuffer& srcMemBuffer);
	~MemBuffer();

	MemBuffer& operator = (MemBuffer& srcMemBuffer);
	bool resize(uint uiSize);
	void clear();

	//inline const char*  getBuffer() const { return m_pMemBuffer.get(); };
	inline char* getBuffer() { return m_pMemBuffer.get(); };
	inline uint getSize() const { return m_uiMemSize; };

private:
	//char* m_pMemBuffer{nullptr};
	std::shared_ptr<char> m_pMemBuffer{};
	uint m_uiMemSize{0};

};


NS_END
