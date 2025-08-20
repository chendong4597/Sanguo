/*!
 * \file HttpLoader.h
 * \date 8-21-2013 15:01:06
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#ifndef __HTTPLOADER_H__
#define __HTTPLOADER_H__

#include "ILoaderBase.h"
#include "BaseUtil_Config.h"

NS_BEGIN

//typedef void (*FUNC_IEVENT)(Event& event);

typedef std::function< void(Event* pEvt)>  FUNC_IEVENT;

#define CAST_FUNC_IEVENT(x) reinterpret_cast<FUNC_IEVENT>(x)

class HttpLoaderEvent : public Event
{
	RTTI_DEF(HttpLoaderEvent);

public:
	static const tstring HTTP_LOADER_SUCCESS;
	static const tstring HTTP_LOADER_FAILED;

public:
	HttpLoaderEvent(){};
	HttpLoaderEvent(const tstring& strId);
	HttpLoaderEvent(const HttpLoaderEvent& copyObj){
		this->strUrl = copyObj.strUrl;
		this->strData = copyObj.strData;
		this->nDownloadSize = copyObj.nDownloadSize;
		this->nTotalSize = copyObj.nTotalSize;
		this->curlResultCode = copyObj.curlResultCode;
	};
	virtual ~HttpLoaderEvent();

	inline const tstring& getId(void) const noexcept { return m_strId; };
	inline void setId(const tstring& id) noexcept { m_strId = id; };

	bool isSuccess() const { return (m_strId == HTTP_LOADER_SUCCESS); };

	virtual Event* clone();

public:
	tstring strUrl{};
	tstring strData{};
	int nDownloadSize{};
	int nTotalSize{};
	int curlResultCode{};

protected:
	tstring m_strId{};
};

class HttpLoaderInternalEvent final : public HttpLoaderEvent
{
	RTTI_DEF(HttpLoaderInternalEvent);

public:
	static const tstring HTTP_LOADER_START;
	static const tstring HTTP_LOADER_PROGRESS;
public:
	
	HttpLoaderInternalEvent(){};
	HttpLoaderInternalEvent(const tstring& strId) :HttpLoaderEvent(strId){};
	HttpLoaderInternalEvent(const HttpLoaderEvent& base) :HttpLoaderEvent(base){};
	virtual ~HttpLoaderInternalEvent();
	bool isSuccess() const { return (m_strId == HTTP_LOADER_SUCCESS); };
	virtual Event* clone() override {
		HttpLoaderInternalEvent* pEvent = new HttpLoaderInternalEvent(m_strId);
		pEvent->strUrl = strUrl;
		pEvent->strData = strData;
		pEvent->nDownloadSize = nDownloadSize;
		pEvent->nTotalSize = nTotalSize;
		pEvent->curlResultCode = curlResultCode;
		pEvent->loaderId = loaderId;
		return pEvent;
	};
public:
	int loaderId{};
};

class HttpLoader : public ILoaderBase
{
public:
	typedef struct HTTP_INFO_tag
	{
		tstring strUrl;
		bool bPost;
		tstring strPostContent;			
		std::vector<FUNC_IEVENT>  callbackVec;

		HTTP_INFO_tag()
		{
			bPost = false;
		}
	} HTTP_INFO;

	typedef std::map<tstring, HTTP_INFO> TM_HTTP_INFO;

	DECLARE_EVENT_HOST();
public:

	HttpLoader(int id);
	virtual ~HttpLoader();

	virtual bool initialize() override;
	virtual void terminate() override;
	virtual bool reset() override;

	bool loadHttp(const tstring& strUrl, EventHandler* pHandler, FUNC_IEVENT pFunc, bool bPost, tstring postContent, int timeout = 0, int retryTimes = CURL_MAX_CONNECTION_RETRY);

protected:
	virtual bool beginDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo) override;
	virtual bool endDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo) override;

	virtual bool preDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo) override;
	virtual bool postDownloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo) override;
	virtual uint downloading(CURL_DOWNLOADING_INFO& curlDownloadingInfo, void* pData, uint size) override;
	virtual bool progress(CURL_DOWNLOADING_INFO& curlDownloadingInfo) override;

private:
	void onDownloadingMsg(HttpLoaderInternalEvent& pMsg);

private:
	TM_HTTP_INFO m_httpInfoMap;
	int m_nId;

};

class HttpLoaderMgr
{
public:
	enum HTTP_LOADER_TYPE
	{
		HLT_NORMAL = 0,
		HLT_LOW_PRIORITY,
		NUM_HTTP_LOADER_TYPES,
		HLT_SPECIAL_CLASS,
	};

public:
	HttpLoaderMgr();
	~HttpLoaderMgr();

	static HttpLoaderMgr& getInstance();

	bool initialize();
	void terminate();
	void reset();

	bool loadHttp(const tstring& strUrl, EventHandler* pHandler, FUNC_IEVENT pFunc, HTTP_LOADER_TYPE eType = HLT_NORMAL, bool bPost = false, tstring strPostContent = (""), int timeout = 0, int retryTimes = ILoaderBase::CURL_MAX_CONNECTION_RETRY);
	HttpLoader* getLoader(HTTP_LOADER_TYPE eLoaderType);

private:
	HttpLoader* m_pLoaders[NUM_HTTP_LOADER_TYPES];

};
NS_END

#endif // __HTTPLOADER_H__
