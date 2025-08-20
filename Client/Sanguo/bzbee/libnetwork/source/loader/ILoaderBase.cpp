/*!
 * \file ILoaderBase.cpp
 * \date 8-21-2013 13:28:19
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#include "ILoaderBase.h"
#include <LogUtil.h>
#include <thread>
#include <curl/curl.h>


NS_BEGIN
bool ILoaderBase::ms_bUpdateProgress = true;

ILoaderBase::ILoaderBase()
{
    m_pTimerDevice = m_Timer.CreateDevice();
}

ILoaderBase::~ILoaderBase()
{
}

bool ILoaderBase::startDownloading(const tstring& strUrl, bool bPost, tstring strPostContent, int timeout, int retryTimes, const uint& ePriority)
{
	LoadingQueueInfo info;
	info.bPost = bPost;
	info.strUrl = strUrl;
	info.strPostContent = strPostContent;
	info.timeout = timeout;
	info.retryTimes = retryTimes;

	lock();
    if (ePriority == PRIORITY_HIGH){
        m_downloadingQueue.push_front(info);
    }
    else{
        m_downloadingQueue.push_back(info);
    }
	unlock();

	return startLoadingThread();
}

bool ILoaderBase::removeLoadingUrl(const tstring& strUrl)
{
	bool bIsInqueue = false;
	lock();
	for (auto it = m_downloadingQueue.begin(); it != m_downloadingQueue.end(); ++it)
	{
		if ((*it).strUrl == strUrl)
		{
			m_downloadingQueue.erase(it);
			bIsInqueue = true;
			break;
		}
	}
	unlock();

	return bIsInqueue;
}

bool ILoaderBase::startLoadingThread()
{
	if (isDownloading()) return true;

	m_bThreadRunning = true;
	
	std::thread t(std::bind(&ILoaderBase::loadingThreadFunc, std::placeholders::_1), this);
	if (!t.joinable())
	{
		m_bThreadRunning = false;
		LOGE("create loading thread failed");
		return false;
	}
	t.detach();	
	return true;
}

void ILoaderBase::lock()
{
	m_mutex.lock();
}

void ILoaderBase::unlock()
{
	m_mutex.unlock();
}

int ILoaderBase::getNumOfTask()
{
	int nNumOfTask = 0;
	lock();
	nNumOfTask = 1 + m_downloadingQueue.size();
	unlock();

	return nNumOfTask;
}

void* ILoaderBase::loadingThreadFunc(void* pData)
{
	ILoaderBase* pLoader = (ILoaderBase*)pData;

	CURL* pCurl = curl_easy_init();
	if (!pCurl)
	{
		pLoader->m_bThreadRunning = false;

// 		LOGD(("curl easy init failed"));
		return NULL;
	}

	while (!pLoader->m_bRequestStop)
	{
		CURL_DOWNLOADING_INFO curlDownloadingInfo;

		pLoader->lock();
		if (pLoader->m_downloadingQueue.size() > 0)
		{
			curlDownloadingInfo.strUrl = pLoader->m_downloadingQueue.front().strUrl;
			curlDownloadingInfo.post = pLoader->m_downloadingQueue.front().bPost;
			curlDownloadingInfo.postContent = pLoader->m_downloadingQueue.front().strPostContent;
			curlDownloadingInfo.timeout = pLoader->m_downloadingQueue.front().timeout;
			curlDownloadingInfo.retryTimes = pLoader->m_downloadingQueue.front().retryTimes;
			pLoader->m_downloadingQueue.pop_front();
		}
		pLoader->unlock();
		if (curlDownloadingInfo.strUrl.empty()) break;

		CURLcode curlResult = CURLE_OK;

		curlDownloadingInfo.totalSize = -1;
		curlDownloadingInfo.downloadSize = 0;
		curlDownloadingInfo.curlResult = curlResult;
		curlDownloadingInfo.pLoader = pLoader;
		curlDownloadingInfo.pCustomData = NULL;
		curlDownloadingInfo.downloadFile.reset();
		if (!pLoader->beginDownloading(curlDownloadingInfo)) break;

		for (int i = 0; i < curlDownloadingInfo.retryTimes; ++i)
		{
			curlResult = CURLE_OK;

			// keep the totalSize we got last time
//			curlDownloadingInfo.totalSize = -1;
			curlDownloadingInfo.downloadSize = 0;
			curlDownloadingInfo.curlResult = curlResult;
			curlDownloadingInfo.pLoader = pLoader;
			curlDownloadingInfo.pCustomData = NULL;
			curlDownloadingInfo.downloadFile.reset();

			// prepare for downloading
			curlDownloadingInfo.pLoader->preDownloading(curlDownloadingInfo);

			//LOGD(("curl downloading start url={0}"), curlDownloadingInfo.strUrl);

			tstring encodedUrl = UrlUtil::encode(curlDownloadingInfo.strUrl);
			curl_easy_setopt(pCurl, CURLOPT_URL, encodedUrl.c_str());

			if (curlDownloadingInfo.post)
			{
				curl_easy_setopt(pCurl, CURLOPT_POST, 1);
				curl_easy_setopt(pCurl, CURLOPT_COPYPOSTFIELDS, curlDownloadingInfo.postContent.c_str());
			}

			curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1L);
			curl_easy_setopt(pCurl, CURLOPT_FAILONERROR, 1L);
			curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1L);
 			curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, CURL_CONNECTION_TIMEOUT);
			if (curlDownloadingInfo.timeout > 0) { curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, curlDownloadingInfo.timeout); }

			curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, &ILoaderBase::writeDataFunc);
			curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &curlDownloadingInfo);

           
            curl_easy_setopt(pCurl, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(pCurl, CURLOPT_PROGRESSFUNCTION, &ILoaderBase::progressFunc);
            curl_easy_setopt(pCurl, CURLOPT_PROGRESSDATA, &curlDownloadingInfo);
            

			curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, 0L);

            pLoader->m_pTimerDevice->StartUp();
			curlResult = curl_easy_perform(pCurl);
			if (curlResult == CURLE_OK)
			{
// 				LOGD(("curl downloading success url={0}"), curlDownloadingInfo.strUrl);
			}
			else
			{
				LOGE(("curl downloading failed, result={0}, url={1}"), curlResult, curlDownloadingInfo.strUrl.c_str());
			}

			/* always cleanup */
			curl_easy_reset(pCurl);

			curlDownloadingInfo.curlResult = curlResult;

			// complete downloading
			curlDownloadingInfo.pLoader->postDownloading(curlDownloadingInfo);

			if (curlResult != CURLE_OPERATION_TIMEDOUT) break;
		
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

		}

		pLoader->endDownloading(curlDownloadingInfo);
	}

	curl_easy_cleanup(pCurl);
	pCurl = NULL;

	pLoader->m_bThreadRunning = false;

    
	return NULL;
}

size_t ILoaderBase::writeDataFunc(void* ptr, size_t size, size_t nmemb, void* pData)
{
	CURL_DOWNLOADING_INFO* pCurlDownloadingInfo = (CURL_DOWNLOADING_INFO*)pData;

	if (pCurlDownloadingInfo->pLoader->isRequestingStop()) return 0;

	// downloading
	return pCurlDownloadingInfo->pLoader->downloading(*pCurlDownloadingInfo, ptr, size*nmemb);
}


int ILoaderBase::progressFunc(void* pData, double t, double d, double ultotal, double ulnow)
{
    static const double PROGRESS_INTERVAL = 0.2;

    if (t <= 0.0) return 0;
   
    //if have actual download speed, just update the CURL_DOWNLOADING_INFO, skip invoke MSG_LOC_FILE_LOADER event
	CURL_DOWNLOADING_INFO* pCurlDownloadingInfo = (CURL_DOWNLOADING_INFO*)pData;
    pCurlDownloadingInfo->downloadSize = (int)d;
    pCurlDownloadingInfo->totalSize = (int)t;

    if (!ms_bUpdateProgress) return 0;

    ILoaderBase* pLoader = pCurlDownloadingInfo->pLoader;
    pLoader->m_dIntervals += pLoader->m_pTimerDevice->GetElapsed();
    if (pLoader->m_dIntervals < PROGRESS_INTERVAL) return 0;

	// progress
    pCurlDownloadingInfo->pLoader->progress(*pCurlDownloadingInfo);

    pLoader->m_dIntervals = 0.0;
    pLoader->m_pTimerDevice->StartUp();

    return 0;
}

void ILoaderBase::stop()
{
	m_bRequestStop = true;
	while (m_bThreadRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

tstring UrlUtil::char2hex(char dec)
{
	char dig1 = (dec & 0xF0) >> 4;
	char dig2 = (dec & 0x0F);
	if (0 <= dig1 && dig1 <= 9) dig1 += 48;    //0,48 in ascii
	if (10 <= dig1 && dig1 <= 15) dig1 += 65 - 10; //A,65 in ascii
	if (0 <= dig2 && dig2 <= 9) dig2 += 48;
	if (10 <= dig2 && dig2 <= 15) dig2 += 65 - 10;

	std::string r;
	r.append(&dig1, 1);
	r.append(&dig2, 1);
	return r;
}

tstring UrlUtil::encode(const tstring& url)
{
	tstring escaped;
	int max = url.length();
	for (int i = 0; i < max; i++)
	{
		if (url[i] == ' ')
		{
			escaped.append("%");
			escaped.append(char2hex(url[i]));	//converts char 255 to string "FF"
		}
		else
		{
			escaped.append(&url[i], 1);
		}
	}
	return escaped;
}

NS_END