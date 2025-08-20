#include "CacheQueue.h"
#include <LogUtil.h>
#include <file/FileSystem.h>
#include <FileUtil.h>
#include <MemCrc.h>
#include "stdafx.h"

using namespace NS;

static const size_t MAX_BLOCK = 64 * 1024;
static const uint32 INITIAL_OFFSET = 8u;


static std::shared_ptr<File> createFile(const tstring& filePath, int storage, int mode)
{
	auto file = FileSystem::createFile(filePath, storage, mode);
	if (!file)
		throw std::runtime_error(fmt::format("Open file failed: {0}", filePath));
	return file;
}


static void seek(std::shared_ptr<File>& pFile, NS::I64 offset, File::SeekType seekType)
{
	if (pFile->seek(offset, seekType) != 0)
		throw std::runtime_error("seek fail");
}


static void read(std::shared_ptr<File>& pFile, void* pBuffer, size_t size)
{
	if (!pBuffer)
		throw std::runtime_error("Read file failed -> invalid buffer.");
	if (size == 0 || size > MAX_BLOCK)
		throw std::runtime_error(fmt::format("Read file failed -> invalid size: {0}", size));

	for (auto p = reinterpret_cast<char*>(pBuffer); size > 0;)
	{
		size_t n = static_cast<size_t>(pFile->read(p, size));
		if (n == 0)
			throw std::runtime_error("Read file failed.");
		else if (size < n)
			throw std::runtime_error("Read file failed -> unknown error.");

		size -= n;
		p += n;
	}
}


static void write(std::shared_ptr<File>& pFile, const void* pBuffer, size_t size)
{
	if (!pBuffer)
		throw std::runtime_error("Write file failed -> invalid buffer.");
	if (size == 0 || size > MAX_BLOCK)
		throw std::runtime_error(fmt::format("Write file failed -> invalid size: {0}", size));

	for (auto p = reinterpret_cast<const char*>(pBuffer); size > 0;)
	{
		size_t n = static_cast<size_t>(pFile->write(p, size));
		if (n == 0)
			throw std::runtime_error("Write file failed.");
		else if (size < n)
			throw std::runtime_error("Write file failed -> unknown error.");

		size -= n;
		p += n;
	}
}


template <typename T> static inline void read (std::shared_ptr<File>& pFile,       T& v) { read (pFile, &v, sizeof(v)); }
template <typename T> static inline void write(std::shared_ptr<File>& pFile, const T& v) { write(pFile, &v, sizeof(v)); }


static void newFile(const tstring& path, uint32 serial)
{
	// 打开文件并清空内容。
	auto file = createFile(path, SubFileSystem::FS_EXTERNAL, File::AM_WRITE);
	// 写入新的文件序列号。
	write(file, serial);
	// 写入初始的偏移量。
	write(file, INITIAL_OFFSET);
}


static void swap(CacheQueue::FileInfo& a, CacheQueue::FileInfo& b) noexcept
{
	using std::swap;
	swap(a.path, b.path);
	swap(a.offset_queue, b.offset_queue);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

CacheQueue::CacheQueue(NS::UI64 iggId, tstring ip)
{
	auto path = fmt::format("{0}@{1}", iggId, ip);
	m_crc = memcrc32(path.data(), path.length());
	m_read .path = path + ".a.dat";
	m_write.path = path + ".b.dat";
}


void CacheQueue::load(std::function<bool(const BufferData&)> callback)
{
	FileInfo* fileInfo[] = { &m_read, &m_write };
	m_realtime_seek = INITIAL_OFFSET;

	std::shared_ptr<File> files[2];
	for (int i = 0; i < 2; ++i)
	{
		// 打开文件。
		files[i] = FileSystem::openFile(fileInfo[i]->path, SubFileSystem::FS_EXTERNAL);
		// 检查文件大小。
		if (files[i] && files[i]->getFileSize() < INITIAL_OFFSET)
			throw std::runtime_error(fmt::format("Invalid file size: {0}", files[i]->getFileSize()));
	}

	if (!files[0])
	{
		swap(files[0], files[1]);
		if (!files[0])	// 两个文件都不存在，即创建新号的情况。
		{
			newFile(fileInfo[0]->path, 0u);
			newFile(fileInfo[1]->path, 1u);
			m_serial = 1u;
			return;
		}
		else
		{
			swap(m_read, m_write);
		}
	}

	if (!files[1])	// 其中一个文件不存在，认为是一种意外的情况。
	{
		// 读取序列号。
		read(files[0], m_serial);
		// 创建缺失的文件。
		newFile(fileInfo[1]->path, ++m_serial);
	}
	else	// 两个文件都存在。
	{
		// 读取序列号。
		uint32 serials[2];
		for (int i = 0; i < 2; ++i)
			read(files[i], serials[i]);
		// 确定读写顺序。
		if (serials[0] > serials[1])
		{
			std::swap(serials[0], serials[1]);
			std::swap(files[0], files[1]);
			swap(m_read, m_write);
		}
		// 判定序列号是否相差 1。
		if (serials[1] - serials[0] != 1)
			throw std::runtime_error(fmt::format("Invalid serials: {0}, {1}.", serials[0], serials[1]));
		// 记录文件序列号。
		m_serial = serials[1];
	}

	// 缓存队列文件。
	for (int i = 0; i < 2; ++i)
	{
		if (!files[i]) continue;

		// 读取数据的偏移。
		uint32 offset{};
		read(files[i], offset);
		if (i == 0)
		{
			// 检查偏移的有效性。
			if (offset < INITIAL_OFFSET || offset > files[i]->getFileSize())
				throw std::runtime_error(fmt::format("Invalid offset: {0}.", offset));
			// 定位。
			seek(files[i], offset, File::SeekType::Begin);
		}
		else
		{
			// 检查偏移的有效性。
			if (offset != INITIAL_OFFSET)
				throw std::runtime_error(fmt::format("Invalid initial offset: {0}.", offset));
			// 实时写入的位置。
			m_realtime_seek = static_cast<uint32>(files[i]->getFileSize());
		}

		// 读取所有数据块。
		while (files[i]->tell() != files[i]->getFileSize())
		{
			// 数据的大小。
			uint32 size{};
			read(files[i], size);
			// 读取数据。
			BufferData buffer(size);
			read(files[i], buffer.data(), size);
			// 读取 CRC。
			uint32 crc;
			read(files[i], crc);
			// 校验数据。
			if (crc != memcrc32(buffer.data(), size, m_crc))
				throw std::runtime_error("Validate data failed.");

			// 下个数据块的偏移值。
			offset += 4 + size + 4;

			// 解析数据。
			if (callback(buffer))
			{
				// 加入队列。
				fileInfo[i]->offset_queue.push(offset);
			}
		}
	}
}


// 完成一次数据同步后，清除一份数据。
void CacheQueue::pop(void)
{
	if (m_read.offset_queue.empty())	// 当前文件已经读取完毕。
	{
		// 认为执行此函数时，队列中一定有数据。
		if (m_write.offset_queue.empty())
			throw std::runtime_error("Pop cache failed.");
		// 读写互换。
		swap(m_read, m_write);
		// 重新创建文件。
		newFile(m_write.path, ++m_serial);
		m_realtime_seek = INITIAL_OFFSET;
	}

	// 写入新的偏移量。
    auto file = createFile(m_read.path, SubFileSystem::FS_EXTERNAL, File::AM_READ | File::AM_PLUS);
	seek(file, 4, File::SeekType::Begin);
	write(file, m_read.offset_queue.front());
	// 从队列中移除当前项。
	m_read.offset_queue.pop();
}


// 将数据块写入文件。
void CacheQueue::push(const void* p_buffer, uint32 size, bool formal)
{
	// 打开文件。
	auto file = createFile(m_write.path, SubFileSystem::FS_EXTERNAL, File::AM_READ | File::AM_PLUS);
	seek(file, m_realtime_seek, File::SeekType::Begin);
	// 写入数据大小。
	write(file, size);
	// 写入数据。
	write(file, p_buffer, size);
	// 写入 CRC。
	write(file, memcrc32(p_buffer, size, m_crc));

	auto newSize = static_cast<uint32>(file->tell());
	if (formal)
	{
		// 加入队列。
		m_write.offset_queue.push(newSize);
		// 设置新的写入点。
		m_realtime_seek = newSize;
	}

	// 设置文件大小。
	if (!file->setFileSize(newSize))
		throw std::runtime_error("Set file size failed.");
}


void CacheQueue::discardFiles(void)
{
	newFile(m_read .path, 0u);
	newFile(m_write.path, 1u);
	m_serial = 1u;
	std::queue<uint32> q0, q1;
	m_read .offset_queue.swap(q0);
	m_write.offset_queue.swap(q1);
}
