/*!
 * \file SystemUtil.h
 * \date 12-10-2012 13:07:42
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#pragma once
#include "BaseType.h"
#include "Rtti.h"
#include "StringUtil.h"
#include <functional>

NS_BEGIN

enum DEVICE_PLATFORM
{
    DP_UNKNOWN = 0,
    DP_WIN32,
    DP_ANDROID,
    DP_IOS,
    DP_FLASH,
    DP_AMAZON,
    DP_WP,
    NUM_DP,
};

enum LANGUAGES
{
    LANG_UNKNOWN = 0,
    LANG_TRUNK,
    LANG_EN_US,
    LANG_ZH_TW,
    LANG_FP_TW,
    LANG_DE,
    LANG_FR,
    LANG_TH,
    LANG_KR,
    LANG_JP,
    LANG_RU,
    LANG_IT,
    LANG_BA,
    LANG_I18N,
    LANG_ES,
    LANG_ARB,
    LANG_PT,
    LANG_TR,
    LANG_VN,
    LANG_ID,
    NUM_LANGUAGES,
};

enum class CapacityLevel : unsigned char
{
	CapacityLevelLow,
	CapacityLevelMedium,
	CapacityLevelHigh,
};

class SystemUtil
{
public:
	enum CONST_DEFINE
	{
		// max texture size 80 Mb
		MAX_TEXTURE_MEMORY_SIZE = 80 * 1024 * 1024,
	};

	enum NETWORK_TYPE
	{
		TYPE_NULL = 0,
		TYPE_WIFI = 1,
		TYPE_CELLULAR = 2,
	};

public:
	RTTI_ROOT(SystemUtil);

	static SystemUtil& getInstance();

	/*!
	 * \brief get the device id of the phone
	 * \return 
	 */
	virtual const tstring& getDeviceId() = 0;
	virtual const tstring& getAndroidId()           const { return EMPTY_STRING; }
	virtual const tstring& getGoogleAdvertisingId() const { return EMPTY_STRING; }
	virtual const tstring& getAndroidVersion()  const { return EMPTY_STRING; }
	virtual const tstring& getCpuInfo()  const { return EMPTY_STRING; }

	virtual void setAndroidVersion(tstring versionName) {  }
	virtual void setCpuInfo(tstring versionName) {  }

	bool getBackGround() const { return m_bBackGround; }
	void setBackGround(bool b) { m_bBackGround = b; }
	int getLoadingProcess()  const { return m_process; }
	void setLoadingProcess(int process) { m_process = process; }

	virtual CapacityLevel getDeviceLevel() const = 0;
	virtual CapacityLevel getMemoryLevel() const = 0;

	virtual void setDeviceLevel(CapacityLevel deviceLevel);

	virtual void setDeviceId(tstring deviceId) {};
	/*!
	 * \brief get the device model of the phone
	 * \return 
	 */
	virtual const tstring& getDeviceModel() = 0;
	virtual void setDeviceModel(tstring devModelName) {};
	/*!
	* \brief get the device platform of the phone
	* \return
	*/
	virtual DEVICE_PLATFORM getDevicePlatform() const = 0;

	/*!
	 * \brief set the apple game center id, only used for ios
	 * \return 
	 */
	virtual const tstring& getAppleGameCenterId() const = 0;

	virtual const tstring& getAId() const        { return EMPTY_STRING; }
	virtual const tstring& getMacAddress() const { return EMPTY_STRING; }
	virtual const tstring& getIMEI() const       { return EMPTY_STRING; }
	/*!
	 * \brief get package name
	 * \return 
	 */
	virtual const tstring& getPackageName() = 0;
	/*!
	 * \brief get system default language
	 * \return 
	 */
	virtual LANGUAGES getSystemLanguage() = 0;

	void setUserLanguage(LANGUAGES lang) { m_userLanguage = lang; };
	LANGUAGES getUserLanguage() const { return m_userLanguage; };

	/*!
	 * \brief get the version name of app
	 * \return 
	 */
	virtual const tstring& getVersionName() = 0;
	virtual void setVersionName(tstring versionName) {};

	virtual UI64 getTotalPhysicsMemory() = 0;
	virtual UI64 getFreePhysicsMemory() = 0;
	virtual UI64 getProcessAvaMemory() = 0;
	virtual UI64 getProcessMemoryHWM() = 0;

	virtual int getNetworkType() const = 0;
	virtual void requestProductData(const tstring& pcId) const {};
	virtual tstring getProductName(const tstring& pcId) const { return EMPTY_STRING; };
	virtual bool isProductDataLoaded() const { return true; };
	virtual void showInterstitialAd() const {};
	virtual void onHandlersInitialized() const {};
    virtual tstring getBundleURLScheme() const { return EMPTY_STRING; }
	virtual tstring getSystemVersion() const   { return EMPTY_STRING; }
    virtual void scheduleLocalNotification(I64 remain_secs, const tstring& id, const tstring& message) const {}
    virtual void unscheduleLocalNotification(const tstring& id) const {}
    virtual void shake() {}
	virtual bool playVideo(const tstring& filePath, bool bLoop) { return {}; }
	virtual void seekTo(int msec) {}
	virtual void stop() {}

	void setMemoryCapacity(float capacity) { m_memoryCapacity = capacity; }
	void setCpuCoreNum(int num) { m_cpuCoresNum = num; };
	void setCpuMaxFreq(int freq) { m_cpuFreqMHz = freq; };

	float getMemoryCapacity() { return m_memoryCapacity; }
	int   getCpuCoreNum() { return m_cpuCoresNum; };
	int   getCpuMaxFreq() { return m_cpuFreqMHz; };
	
	using OnGetObbUrlFunc = std::function<void(const tstring& url, int step, int code)>;

	virtual bool downloadObb(const OnGetObbUrlFunc& func)
	{
		/*m_onGetObbUrlFunc = func;

		callOnGetObbUrlFunc("http://113.196.94.68/sandbox_cdn/main.0.com.igg.android.demo26.obb", 1, 0);

		return true; */

		return false;
	}

	void callOnGetObbUrlFunc(const tstring& url, int step, int code)
	{
		if (m_onGetObbUrlFunc)
		{
			m_onGetObbUrlFunc(url, step, code);
		}
	}

	virtual tstring getObbDownloadProgress()
	{
		return{};
	}

	virtual I64 getObbFreeSpace()
	{
		return 0;
	}

	I64 getCurrentGameId();
	virtual tstring getGameId(LANGUAGES language) { return EMPTY_STRING; }
	virtual tstring getGameKey(LANGUAGES language) { return EMPTY_STRING; }
	virtual tstring getPaymentKey() { return EMPTY_STRING; }

	virtual tstring getServiceEmail(LANGUAGES language) { return EMPTY_STRING; }
	virtual tstring getAppName() { return EMPTY_STRING; }
	virtual void setAppName(const tstring& appName) {}
	virtual int getAPIVersion() { return 0; }

	/*!
	 * \brief get the version name of app
	 * \return
	 */
	virtual const tstring& getFileName() { return m_strFile; }
	virtual void setFileName(const tstring& fileName) {m_strFile = fileName;}

protected:
	float m_memoryCapacity{};	// in gigabyte units
	int m_cpuFreqMHz{ 0 };
	int m_cpuCoresNum{ 0 };
	CapacityLevel m_deviceLevel;

	OnGetObbUrlFunc m_onGetObbUrlFunc{};

	bool m_bBackGround{ false };
    int m_process{ 0 };
	tstring m_strFile{};

	LANGUAGES m_userLanguage{ LANG_UNKNOWN };

};

NS_END
