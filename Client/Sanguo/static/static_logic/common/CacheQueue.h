#pragma once
#include "BaseTypeDefine.h"
#include <file/File.h>
#include <queue>
#include <functional>

NS_BEGIN

class CacheQueue final
{
public:
	struct FileInfo final
	{
		tstring path;
		std::queue<uint32> offset_queue;	// 注意：swap 无法直接处理成员变量。
	};

public:
	CacheQueue(NS::UI64 iggId, tstring ip);
	void load(std::function<bool(const BufferData&)> callback);

	// 完成一次数据同步后，清除一份数据。
	void pop(void);
	// 将数据块写入文件。
	void push(const void* p_buffer, uint32 size, bool formal);
	// 放弃所有缓存文件的内容。仅用于更换设备之后的处理。
	void discardFiles(void);
	// 判断队列是否为空。
	inline bool empty(void) const { return m_read.offset_queue.empty() && m_write.offset_queue.empty(); }

private:
	uint32 m_serial{};
	uint32 m_crc{};
	uint32 m_realtime_seek{};
	FileInfo m_read;
	FileInfo m_write;
};

NS_END
