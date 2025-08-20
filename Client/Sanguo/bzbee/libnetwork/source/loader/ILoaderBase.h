/*!
 * \file ILoaderBase.h
 * \date 8-21-2013 13:28:14
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#pragma once

#include <memory>
#include "../MsgHandler.h"
#include "BaseUtil_Config.h"
#include "TimeUtil.h"
#include <mutex>
#include <list>

NS_BEGIN
class File;
class ILoaderBase : public MsgHandler
{
public:
    enum DOWNLOAD_PRIORITY
    {
        PRIORITY_NORMAL = 0,
        PRIORITY_HIGH,
    };

	enum CONST_DEFINE
	{
		CURL_CONNECTION_TIMEOUT = 10,
		CURL_MAX_CONNECTION_RETRY = 6,
	};

	struct LoadingQueueInfo
	{
		LoadingQueueInfo()
		{
			bPost = false;
		}

		tstring strUrl;
		bool bPost;
		tstring strPostContent;
		int timeout;
		int retryTimes;
	};

	typedef std::list<LoadingQueueInfo> TL_LOADING_QUEUE_INFO;

	typedef struct CURL_DOWNLOADING_INFO_tag
	{
		tstring strUrl;
		int totalSize;
		int downloadSize;
		int curlResult;
		int timeout;
		int retryTimes;
		ILoaderBase* pLoader;
		void* pCustomData;
		std::shared_ptr<File> downloadFile;
		bool post;
		tstring postContent;
	} CURL_DOWNLOADING_INFO;

public:
	RTTI_DEF(ILoaderBase);

	ILoaderBase();
	virtual ~ILoaderBase();


	virtual bool isDownloading() const {	return m_bThreadRunning; };
	inline bool isRequestingStop() const { return m_bRequestStop; };

	virtual int getNumOfTask();
	void stop();
protected:
    bool startDownloading(const tstring& strUrl, bool bPost, tstring strPostContent, int timeout, int retryTimes, const uint& ePriority = PRIORITY_NORMAL);
	bool removeLoadingUrl(const tstring& strUrl);
	void lock();
	void unlock();

	virtual bool beginDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo) = 0;
	virtual bool endDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo) = 0;

	virtual bool preDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo) = 0;
	virtual uint downloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo, void* pData, uint size) = 0;
	virtual bool progress(CURL_DOWNLOADING_INFO& curlDownloadingInfo) = 0;
	virtual bool postDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo) = 0;

private:
	bool startLoadingThread();

	static void* loadingThreadFunc(void* pData);
	static size_t writeDataFunc(void* ptr, size_t size, size_t nmemb, void* pData);
	static int progressFunc(void* pData, double t, double d, double ultotal, double ulnow);

private:
    bool m_bRequestStop{};
    bool m_bThreadRunning{};
	std::mutex m_mutex;
	TL_LOADING_QUEUE_INFO m_downloadingQueue;
	CTimer m_Timer;
    CTimerDevice* m_pTimerDevice{};
    double m_dIntervals{};

public:
    static bool ms_bUpdateProgress;

};


class UrlUtil
{
public:
	static tstring char2hex(char dec);
	static tstring encode(const tstring& url);

};

NS_END
