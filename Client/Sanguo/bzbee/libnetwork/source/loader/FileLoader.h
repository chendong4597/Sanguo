/*!
 * \file FileLoader.h
 * \date 5-27-2019
 * 
 * 
 * \author lzj
 */
#pragma once

#include "ILoaderBase.h"
#include "BaseUtil_Config.h"

#include <functional>
#include <vector>
#include <array>
#include <memory>
#include <mutex>
#include <queue>

NS_BEGIN

class FileLoaderEvent : public Event
{
public:
	enum EventType
	{
		Start,
		Progress,
		Succeeded,
		Failed,
		Cancelled
	};

public:
	bool isSucceeded() const { return (m_eventType == Succeeded); }
	bool isProgress() const { return (m_eventType == Progress); }
	bool isStart() const { return (m_eventType == Start); }
	bool isFailed() const { return (m_eventType == Failed); }
	bool isCancelled() const { return (m_eventType == Cancelled); }

public:
	EventType m_eventType;
	int nId{};
	tstring strFileId;
	tstring strFullFilePath;
	tstring strRemoteFile;
	int nDownloadSize{};
	int nTotalSize{ -1 };
	int curlResultCode{};
};

class FileLoader : public ILoaderBase
{
public:
	using FileLoaderEventHandler = std::function<void(FileLoaderEvent&)>;
	struct FILE_INFO
	{
		tstring strLocalFile;
		tstring strRemoteFile;
		std::vector<FileLoaderEventHandler> eventHandler;
	};

	typedef std::map<tstring, FILE_INFO> TM_FILE_INFO;

	DECLARE_EVENT_HOST();

public:
	RTTI_DEF(FileLoader);

	FileLoader(int id);
	~FileLoader() override;

	bool initialize() override;
	void terminate() override;
	bool reset() override;

	bool loadFile(const tstring& strLocalFile, const tstring& strRemoteFile, FileLoaderEventHandler handler);
	bool removeLoadingByRemoteFile(const tstring& strRemoteFile);

protected:
	/**
	 * Following functions will be invoked by loading threads
	 */
	bool beginDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo) override;
	bool endDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo) override;
	bool preDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo) override;
	bool postDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo) override;
	uint downloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo, void* pData, uint size) override;
	bool progress(CURL_DOWNLOADING_INFO& curlDownloadingInfo) override;
	/**
	 * **********************************************************
	 */

private:
	void onFileLoaderEvent(FileLoaderEvent& evt);

private:
	int m_nId;

private:
	static TM_FILE_INFO m_fileInfoMap;
	static int m_nNumLoaders;
	static std::mutex m_mutex;
};

/**
 * *********************************************************
 */
class FileLoaderMgr : public bzbee::MsgHandler
{
public:
	enum LOADER_PRIORITY
	{
		LOADER_PRIORITY_LOW = 0,
		LOADER_PRIORITY_NORMAL,
		NUM_LOADER_PRIORITY,
		LOADER_PRIORITY_MIN = LOADER_PRIORITY_LOW,
		LOADER_PRIORITY_MAX = LOADER_PRIORITY_NORMAL,
	};

public:
	static FileLoaderMgr& getInstance();
    FileLoaderMgr();
    ~FileLoaderMgr();

	bool initialize() override;
	void terminate() override;
	bool reset() override;
	void update(float dt) override;

	bool loadFile(const tstring& strLocalFile, const tstring& strRemoteFile, FileLoader::FileLoaderEventHandler handler = {},
		LOADER_PRIORITY eLoaderPriority = LOADER_PRIORITY_NORMAL);
	void queueEvent(const FileLoaderEvent& evt);

	bool isBusy();

private:
	FileLoader* getWorkingLoader(LOADER_PRIORITY ePriority);

private:
	std::array<std::unique_ptr<FileLoader>, NUM_LOADER_PRIORITY> m_pLoaders{};
	std::queue<FileLoaderEvent> m_fileLoaderEventQueue;
	std::queue<FileLoaderEvent> m_fileLoaderEventQueueBg;
	std::mutex m_mutexEventQueue;
};

NS_END
