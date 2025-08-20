/*!
 * \file LogFile.cpp
 * \date 9-24-2019
 *
 *
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#include "LogFile.h"
#include "../file/FileSystem.h"

NS_BEGIN

LogFile::LogFile()
{

}

LogFile::~LogFile()
{
    closeLogFile();
}

bool LogFile::isReady() const
{
    return (m_file != nullptr);
}

bool LogFile::openLogFile(const tstring& logFile)
{
    if (m_file) return false;

    m_file = FileSystem::createFile(logFile, SubFileSystem::FS_EXTERNAL);
    if (!m_file) return false;

    m_logFilePath = logFile;
    return true;
}

void LogFile::closeLogFile()
{
    flush();

    m_file.reset();
    m_logFilePath.clear();
}

void LogFile::flush()
{
    if (!m_file) return;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (int i = 0; i < m_currentCacheCount; ++i)
        {
            m_file->write((const int8*)m_logCaches[i].data(), m_logCaches[i].length());
        }
        m_currentCacheCount = 0;
    }

    m_file->flush();
}

const tstring& LogFile::getLogFilePath() const
{
    return m_logFilePath;
}

bool LogFile::logString(const tstring& strLog)
{
    if (m_currentCacheCount >= MAX_CACHE_COUNT)
    {
        flush();
    }

	if(m_currentCacheCount < MAX_CACHE_COUNT)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

		if (m_currentCacheCount < MAX_CACHE_COUNT)
		{
			m_logCaches[m_currentCacheCount++] = strLog;
		}
    }

    return true;
}

NS_END
