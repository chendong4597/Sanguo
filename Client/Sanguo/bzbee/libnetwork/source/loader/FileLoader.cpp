/*!
 * \file FileLoader.cpp
 * \date 5-27-2019
 * 
 * 
 * \author lzj
 */
#if 1

#include "FileLoader.h"

#include "BaseUtil_Config.h"
#include "LogUtil.h"

#include <SystemUtil.h>
#include <FileUtil.h>

#include <curl/curl.h>

NS_BEGIN

std::mutex FileLoader::m_mutex;
int FileLoader::m_nNumLoaders;
FileLoader::TM_FILE_INFO FileLoader::m_fileInfoMap;

FileLoader::FileLoader(int id)
	: m_nId(id)
{
	++m_nNumLoaders;
}

FileLoader::~FileLoader()
{
	--m_nNumLoaders;
}

bool FileLoader::initialize()
{
	REGISTER_EVENT(&FileLoaderMgr::getInstance(), FileLoaderEvent, &FileLoader::onFileLoaderEvent);

	return true;
}

void FileLoader::terminate()
{
	UNREGISTER_EVENT(&FileLoaderMgr::getInstance(), FileLoaderEvent);
}

bool FileLoader::reset()
{
	return true;
}

bool FileLoader::loadFile(const tstring& strLocalFile, const tstring& strRemoteFile, FileLoaderEventHandler handler)
{
	std::lock_guard<std::mutex> guard{ m_mutex };
	auto it = m_fileInfoMap.find(strRemoteFile);
	if (it != m_fileInfoMap.end())
    {
		auto& fileInfo = it->second;
		fileInfo.eventHandler.push_back(handler);
		return true;
	}
    else
    {
		FILE_INFO fileInfo{};
		fileInfo.strLocalFile = strLocalFile;
		fileInfo.strRemoteFile = strRemoteFile;
		fileInfo.eventHandler.push_back(handler);

		m_fileInfoMap.emplace(fileInfo.strRemoteFile, fileInfo);
		return startDownloading(strRemoteFile, false, bzbee::EMPTY_STRING, 0, 3);
	}
}

bool FileLoader::removeLoadingByRemoteFile(const tstring& strRemoteFile) {
	std::lock_guard<std::mutex> guard{ m_mutex };
	TM_FILE_INFO::iterator it = m_fileInfoMap.find(strRemoteFile);
	if (it != m_fileInfoMap.end()) {
		const FILE_INFO& fileInfo = it->second;

		FileLoaderEvent event{};
		event.m_eventType = FileLoaderEvent::Cancelled;
		event.nId = m_nId;
		event.strFileId = fileInfo.strLocalFile;
		event.strFullFilePath = EMPTY_STRING;
		event.strRemoteFile = fileInfo.strRemoteFile;
		event.nDownloadSize = 0;
		event.nTotalSize = 0;
		event.curlResultCode = 0;
		for (const auto& handler : fileInfo.eventHandler) {
			handler(event);
		}

		removeLoadingUrl(strRemoteFile);

		return true;
	}

	return false;
}

void FileLoader::onFileLoaderEvent(FileLoaderEvent& evt) {
	if (m_nId != evt.nId) {
		return;
	}

	TM_FILE_INFO::iterator itFileInfo = m_fileInfoMap.find(evt.strRemoteFile);
	if (itFileInfo != m_fileInfoMap.end()) {
		const FILE_INFO& fileInfo = itFileInfo->second;

		for (const auto& handler : fileInfo.eventHandler) {
			handler(evt);
		}
		if (evt.m_eventType == FileLoaderEvent::Succeeded
			|| evt.m_eventType == FileLoaderEvent::Failed) {
			m_fileInfoMap.erase(itFileInfo);
		}
	}
}

bool FileLoader::beginDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo) {
	// create local path
	tstring localPath;
	tstring fileId;
	{
		std::lock_guard<std::mutex> guard{ m_mutex };
		TM_FILE_INFO::iterator it = m_fileInfoMap.find(curlDownloadingInfo.strUrl);
		if (it != m_fileInfoMap.end()) {
			const FILE_INFO& fileInfo = it->second;
            localPath = fileInfo.strLocalFile;
			fileId = fileInfo.strLocalFile;
		}
	}
	if (localPath.empty()) {
		return false;
	}

	// Notify download started
	FileLoaderEvent evt{};
	evt.m_eventType = FileLoaderEvent::Start;
	evt.nId = m_nId;
	evt.strFileId = localPath;
	evt.strFullFilePath = fileId;
	evt.strRemoteFile = curlDownloadingInfo.strUrl;
	evt.nDownloadSize = curlDownloadingInfo.downloadSize;
	evt.nTotalSize = curlDownloadingInfo.totalSize;
	evt.curlResultCode = curlDownloadingInfo.curlResult;
	FileLoaderMgr::getInstance().queueEvent(evt);

	return true;
}

bool FileLoader::endDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo) {
	if (curlDownloadingInfo.downloadFile) {
		curlDownloadingInfo.downloadFile->close();
	}

	FileLoaderEvent event{};
	event.nId = m_nId;
	{
		std::lock_guard<std::mutex> guard{ m_mutex };
		TM_FILE_INFO::iterator it = m_fileInfoMap.find(curlDownloadingInfo.strUrl);
		if (it != m_fileInfoMap.end()) {
			const FILE_INFO& fileInfo = it->second;
			event.strFileId = fileInfo.strLocalFile;
			event.strFullFilePath = fileInfo.strLocalFile;
		}
	}

	// notify download complete
	event.nId = m_nId;

	if (curlDownloadingInfo.curlResult == CURLE_OK) {
		event.m_eventType = FileLoaderEvent::Succeeded;
	} else {
		event.m_eventType = FileLoaderEvent::Failed;
	}
	event.strRemoteFile = curlDownloadingInfo.strUrl;
	event.nDownloadSize = curlDownloadingInfo.downloadSize;
	event.nTotalSize = curlDownloadingInfo.totalSize;
	event.curlResultCode = curlDownloadingInfo.curlResult;
	FileLoaderMgr::getInstance().queueEvent(event);

	return true;
}

bool FileLoader::preDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo) {
	return true;
}

bool FileLoader::postDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo) {
	if (curlDownloadingInfo.downloadFile) {
		curlDownloadingInfo.downloadFile->close();
	}

	return true;
}

uint FileLoader::downloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo, void* pData, uint size) {
	if (size <= 0) {
		return 0;
	}

	// create file if nessary
	if (!curlDownloadingInfo.downloadFile)
	{
		tstring localPath;
		{
			std::lock_guard<std::mutex> guard{ m_mutex };
			TM_FILE_INFO::iterator it = m_fileInfoMap.find(curlDownloadingInfo.strUrl);
			if (it != m_fileInfoMap.end()) {
				const FILE_INFO& fileInfo = it->second;
                localPath = fileInfo.strLocalFile;
			}
		}
		if (localPath.empty()) {
			return 0;
		}
		curlDownloadingInfo.downloadFile = FileSystem::createFile(localPath, bzbee::SubFileSystem::FS_INTERNAL);
		if (!curlDownloadingInfo.downloadFile) {
			LOGE("Create download file [{0}] failed.", localPath);
			return 0;
		}
	}

	return static_cast<uint>(curlDownloadingInfo.downloadFile->write(pData, size));
}

bool FileLoader::progress(CURL_DOWNLOADING_INFO& curlDownloadingInfo)
{
	FileLoaderEvent event{};

	{
		std::lock_guard<std::mutex> guard{ m_mutex };
		TM_FILE_INFO::iterator it = m_fileInfoMap.find(curlDownloadingInfo.strUrl);
		if (it != m_fileInfoMap.end()) {
			const FILE_INFO& fileInfo = it->second;
			event.strFileId = fileInfo.strLocalFile;
            event.strFullFilePath = fileInfo.strLocalFile;
		}
	}

	// notify download complete
	event.nId = m_nId;
	event.strRemoteFile = curlDownloadingInfo.strUrl;
	event.m_eventType = FileLoaderEvent::Progress;
	event.nDownloadSize = curlDownloadingInfo.downloadSize;
	event.nTotalSize = curlDownloadingInfo.totalSize;
	event.curlResultCode = curlDownloadingInfo.curlResult;
	FileLoaderMgr::getInstance().queueEvent(event);

	return true;
}

/**
 * ***************************************************************
 */
static FileLoaderMgr* g_FileLoaderMgr = nullptr;
FileLoaderMgr& FileLoaderMgr::getInstance()
{
    assert(g_FileLoaderMgr);
    return *g_FileLoaderMgr;
}


FileLoaderMgr::FileLoaderMgr()
{
    assert(!g_FileLoaderMgr);
    g_FileLoaderMgr = this;
}

FileLoaderMgr::~FileLoaderMgr()
{
    g_FileLoaderMgr = nullptr;
}

bool FileLoaderMgr::initialize()
{
	for (std::size_t i = 0; i < m_pLoaders.size(); ++i)
	{
		m_pLoaders[i].reset(new FileLoader{static_cast<int>(i)});
		if (!m_pLoaders[i]->initialize())
		{ return false; }
	}
	return true;
}

void FileLoaderMgr::terminate()
{
	for (int i = 0; i < NUM_LOADER_PRIORITY; ++i)
	{
		if (m_pLoaders[i])
		{
			m_pLoaders[i]->terminate();
			m_pLoaders[i].reset();
		}
	}
}

bool FileLoaderMgr::reset() {
	return true;
}

void FileLoaderMgr::update(float dt) {
	{
		std::lock_guard<std::mutex> guard{ m_mutexEventQueue };
		m_fileLoaderEventQueueBg.swap(m_fileLoaderEventQueue);
	}
	while (!m_fileLoaderEventQueueBg.empty()) {
		postEvent(m_fileLoaderEventQueueBg.front());
		m_fileLoaderEventQueueBg.pop();
	}
}

bool FileLoaderMgr::loadFile(const tstring& strLocalFile, const tstring& strRemoteFile, FileLoader::FileLoaderEventHandler handler,
	LOADER_PRIORITY eLoaderPriority) {
	FileLoader* pFileLoader = getWorkingLoader(eLoaderPriority);
	if (!pFileLoader) {
		return false;
	}
	return pFileLoader->loadFile(strLocalFile, strRemoteFile, handler);
}

void FileLoaderMgr::queueEvent(const FileLoaderEvent& evt) {
	std::lock_guard<std::mutex> guard{ m_mutexEventQueue };
	m_fileLoaderEventQueue.push(evt);
}

FileLoader* FileLoaderMgr::getWorkingLoader(LOADER_PRIORITY ePriority) {
	if (ePriority == LOADER_PRIORITY_NORMAL) {
		if (!m_pLoaders[LOADER_PRIORITY_LOW]->isDownloading()) {
			return m_pLoaders[LOADER_PRIORITY_LOW].get();
		} else if (!m_pLoaders[LOADER_PRIORITY_NORMAL]->isDownloading()) {
			return m_pLoaders[LOADER_PRIORITY_NORMAL].get();
		} else if (m_pLoaders[LOADER_PRIORITY_LOW]->getNumOfTask() <= m_pLoaders[LOADER_PRIORITY_NORMAL]->getNumOfTask()) {
			return m_pLoaders[LOADER_PRIORITY_LOW].get();
		} else {
			return m_pLoaders[LOADER_PRIORITY_NORMAL].get();
		}
	} else {
		return m_pLoaders[LOADER_PRIORITY_LOW].get();
	}
}

bool FileLoaderMgr::isBusy() {
	for (int i = 0; i < NUM_LOADER_PRIORITY; ++i) {
		if (m_pLoaders[i]->isDownloading()) {
			return true;
		}
	}
	return false;
}

NS_END

#endif
