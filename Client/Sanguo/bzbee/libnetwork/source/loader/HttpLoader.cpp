/*!
 * \file HttpLoader.cpp
 * \date 8-21-2013 15:01:11
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#include "HttpLoader.h"
#include <curl/curl.h>
#include "BaseUtil_Config.h"
#include "LogUtil.h"

NS_BEGIN
const tstring HttpLoaderEvent::HTTP_LOADER_SUCCESS = ("HTTP_LOADER_SUCCESS");
const tstring HttpLoaderEvent::HTTP_LOADER_FAILED = ("HTTP_LOADER_FAILED");

const tstring HttpLoaderInternalEvent::HTTP_LOADER_PROGRESS = ("HTTP_LOADER_PROGRESS");
const tstring HttpLoaderInternalEvent::HTTP_LOADER_START = ("HTTP_LOADER_START");


HttpLoaderEvent::HttpLoaderEvent(const tstring& strId)
	: m_strId(strId)
{
	nDownloadSize = 0;
	nTotalSize = 0;
	curlResultCode = 0;
}

HttpLoaderEvent::~HttpLoaderEvent()
{
	// TODO: 
}

Event* HttpLoaderEvent::clone()
{
	HttpLoaderEvent* pEvent = new HttpLoaderEvent(m_strId);
	pEvent->strUrl = strUrl;
	pEvent->strData = strData;
	pEvent->nDownloadSize = nDownloadSize;
	pEvent->nTotalSize = nTotalSize;
	pEvent->curlResultCode = curlResultCode;
	return pEvent;
}

//////////////////////////////////////////////////////////////
HttpLoaderInternalEvent::~HttpLoaderInternalEvent() = default;

HttpLoader::HttpLoader(int id)
:m_nId(id)
{

}

HttpLoader::~HttpLoader()
{
	// TODO: 
}

bool HttpLoader::initialize()
{
	//REGISTER_EVENT(this, HttpLoaderInternalEvent, &HttpLoader::onDownloadingMsg);
	return true;
}

void HttpLoader::terminate()
{
	//UNREGISTER_EVENT(this, HttpLoaderInternalEvent);
	stop();
}

bool HttpLoader::reset()
{
	// TODO: 
	return true;
}


bool HttpLoader::loadHttp(const tstring& strUrl, EventHandler* pHandler, FUNC_IEVENT pFunc, bool bPost /*= false*/, tstring postContent /*= ("")*/, int timeout /*= 0*/, int retryTimes /*= CURL_MAX_CONNECTION_RETR*/)
{
	// find file if exist
// 	LOGD(("HttpLoader loadHttp:{0}"), strUrl.c_str());
	TM_HTTP_INFO::iterator it = m_httpInfoMap.find(strUrl);
	if (it != m_httpInfoMap.end() &&
		(it->second.bPost != bPost || it->second.strPostContent != postContent))
	{
		HTTP_INFO& httpInfo = it->second;

		if(pHandler && pFunc)
		{
			httpInfo.callbackVec.push_back(pFunc);
		}
	}
	else
	{
		HTTP_INFO httpInfo;
		httpInfo.strUrl = strUrl;
		httpInfo.bPost = bPost;
		httpInfo.strPostContent = postContent;

		if (pHandler && pFunc)
		{
			httpInfo.callbackVec.push_back(pFunc);
		}

		m_httpInfoMap.insert(std::make_pair(httpInfo.strUrl, httpInfo));
	}

	return startDownloading(strUrl, bPost, postContent, timeout, retryTimes);
}

bool HttpLoader::beginDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo)
{
	HttpLoaderInternalEvent msg(HttpLoaderInternalEvent::HTTP_LOADER_START);
	msg.loaderId = m_nId;
	msg.strUrl = curlDownloadingInfo.strUrl;
	msg.nTotalSize = curlDownloadingInfo.downloadSize;
	msg.curlResultCode = curlDownloadingInfo.curlResult;
	onDownloadingMsg(msg);
	return true;
}

bool HttpLoader::endDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo)
{
	HttpLoaderInternalEvent pMsg;

	tstring* pStrData = (tstring*)curlDownloadingInfo.pCustomData;
	if (pStrData)
	{
		pMsg.strData = *pStrData;
		//LOGI(pMsg.strData.c_str());
	}

	SAFE_DELETE(pStrData);
	curlDownloadingInfo.pCustomData = NULL;

	// send message notify end
    
	pMsg.loaderId = m_nId;
	pMsg.strUrl = curlDownloadingInfo.strUrl;
    
// 	LOGD(("HttpLoader url={0}, totalSize={1}, downloadSize={2}"), curlDownloadingInfo.strUrl.c_str(), curlDownloadingInfo.totalSize, curlDownloadingInfo.downloadSize);

	if (curlDownloadingInfo.curlResult == CURLE_OK)
	{
     
		pMsg.setId(HttpLoaderInternalEvent::HTTP_LOADER_SUCCESS);
//		LOGD(("HttpLoader loading success"));
	}
	else
	{
		pMsg.setId(HttpLoaderInternalEvent::HTTP_LOADER_FAILED);
		LOGE(("HttpLoader loading failed {0}"), curlDownloadingInfo.curlResult);
	}

	pMsg.nDownloadSize = curlDownloadingInfo.downloadSize;
	pMsg.nTotalSize = curlDownloadingInfo.totalSize;
	pMsg.curlResultCode = curlDownloadingInfo.curlResult;
	onDownloadingMsg(pMsg);
	return true;
}

bool HttpLoader::preDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo)
{
	if (curlDownloadingInfo.pCustomData)
	{
		tstring* pStrData = (tstring*)curlDownloadingInfo.pCustomData;
		SAFE_DELETE(pStrData);
		curlDownloadingInfo.pCustomData = NULL;
	}

	return true;
}

bool HttpLoader::postDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo)
{
	// TODO: 
	return true;
}

uint HttpLoader::downloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo, void* pData, uint size)
{
	if (size <= 0) return 0;

	// create string
	tstring* pStrData = (tstring*)curlDownloadingInfo.pCustomData;
	if (!pStrData)
	{
		pStrData = new tstring();
		curlDownloadingInfo.pCustomData = pStrData;
	}

	pStrData->append((const tchar*)pData, size);
	return size;
}

bool HttpLoader::progress(CURL_DOWNLOADING_INFO& curlDownloadingInfo)
{
	HttpLoaderInternalEvent msg;
	msg.loaderId = m_nId;
	msg.strUrl = curlDownloadingInfo.strUrl;
	msg.setId(HttpLoaderInternalEvent::HTTP_LOADER_PROGRESS);
	msg.nDownloadSize = curlDownloadingInfo.totalSize;
	msg.curlResultCode = curlDownloadingInfo.curlResult;
	onDownloadingMsg(msg);
	return true;
}

void HttpLoader::onDownloadingMsg(HttpLoaderInternalEvent& refMsg)
{
	if (m_nId != refMsg.loaderId) return;

	tstring strId = refMsg.getId();
	// dispatch event
	if (strId == HttpLoaderInternalEvent::HTTP_LOADER_START || strId == HttpLoaderInternalEvent::HTTP_LOADER_PROGRESS)
	{
		return;
	}

	TM_HTTP_INFO::iterator it = m_httpInfoMap.find(refMsg.strUrl);
	if (it == m_httpInfoMap.end()) return;

	HTTP_INFO& httpInfo = it->second;
	for (std::vector<FUNC_IEVENT>::const_iterator it = httpInfo.callbackVec.begin(); it != httpInfo.callbackVec.end(); ++it)
	{
		const FUNC_IEVENT& callback = (*it);
		if (callback)
		{
			callback(dynamic_cast<HttpLoaderInternalEvent*>(&refMsg));
		}
	}

	// remove from map
	if (strId == HttpLoaderInternalEvent::HTTP_LOADER_SUCCESS
		|| strId == HttpLoaderInternalEvent::HTTP_LOADER_FAILED)
	{
		m_httpInfoMap.erase(it);
	}
}

HttpLoaderMgr::HttpLoaderMgr()
{
	memset(m_pLoaders, 0, sizeof(m_pLoaders));
}

HttpLoaderMgr::~HttpLoaderMgr()
{
	// TODO: 
}

HttpLoaderMgr& HttpLoaderMgr::getInstance()
{
	static HttpLoaderMgr s_HttpLoaderMgr;
	return s_HttpLoaderMgr;
}

bool HttpLoaderMgr::initialize()
{
	for (int i = 0; i < NUM_HTTP_LOADER_TYPES; ++i)
	{
		m_pLoaders[i] = new HttpLoader(i);
		m_pLoaders[i]->initialize();
	}

	return true;
}

void HttpLoaderMgr::terminate()
{
	for (int i = 0; i < NUM_HTTP_LOADER_TYPES; ++i)
	{
		if (m_pLoaders[i]) m_pLoaders[i]->terminate();
		SAFE_DELETE(m_pLoaders[i]);
	}
}

void HttpLoaderMgr::reset()
{

}


bool HttpLoaderMgr::loadHttp(const tstring& strUrl, EventHandler* pHandler, FUNC_IEVENT pFunc, HTTP_LOADER_TYPE eType /*= HLT_NORMAL*/, bool bPost /*= false*/, tstring strPostContent /*= ("")*/, int timeout /*= 0*/, int retryTimes /*= ILoaderBase::CURL_MAX_CONNECTION_RETRY*/)
{
    LOGE(("HttpLoaderMgr loadHttp: {0}"), strUrl);
	HttpLoader* pLoader = getLoader(eType);
	if (pLoader)
	{
		return pLoader->loadHttp(strUrl, pHandler, pFunc,bPost, strPostContent, timeout, retryTimes);
	}
	else
	{
		return false;
	}
}

HttpLoader* HttpLoaderMgr::getLoader(HTTP_LOADER_TYPE eLoaderType)
{
	if(eLoaderType != HLT_SPECIAL_CLASS)
	{
		if (eLoaderType != HLT_LOW_PRIORITY)
		{
			if (m_pLoaders[HLT_LOW_PRIORITY]->isDownloading() || m_pLoaders[HLT_LOW_PRIORITY]->getNumOfTask() > 0)
			{
				return m_pLoaders[HLT_NORMAL];
			}
		}
		
		return m_pLoaders[HLT_LOW_PRIORITY];
	}

	return NULL;
}
NS_END
