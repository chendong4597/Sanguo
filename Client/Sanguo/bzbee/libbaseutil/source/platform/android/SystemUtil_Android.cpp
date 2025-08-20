/*!
 * \file SystemUtil_Android.cpp
 * \date 12-10-2012 13:07:53
 *
 *
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#include "SystemUtil_Android.h"
#include "JniHelper.h"
#include "System_Info_Android.h"
#include "../../LogUtil.h"
#include "../../file/FileSystem.h"

using namespace NS;

static const std::string systemUtilClassName = "com/igg/bzbee/static_baseutil/SystemUtil";
static const std::string ObbDownladerCalssName = "com/igg/bzbee/app_sandbox/platform/ObbDownloader";
static const std::string GameEntryCalssName = "com/igg/bzbee/static_app/GameEntry";

SystemUtil& SystemUtil::getInstance()
{
	static SystemUtil_Android s_SystemUtil_Android;
	return s_SystemUtil_Android;
}

SystemUtil_Android::SystemUtil_Android()
{

}

SystemUtil_Android::~SystemUtil_Android()
{

}

const tstring& SystemUtil_Android::getDeviceId()
{
	return m_strDeviceId;
}

CapacityLevel SystemUtil_Android::getDeviceLevel() const
{
	if (getMemoryLevel() == CapacityLevel::CapacityLevelHigh
		&& getCpuCoreLevel() == CapacityLevel::CapacityLevelHigh
		&& getCpuFreqLevel() == CapacityLevel::CapacityLevelHigh)
	{
		return CapacityLevel::CapacityLevelHigh;
	}
	else if (getMemoryLevel() == CapacityLevel::CapacityLevelLow
		|| getCpuCoreLevel() == CapacityLevel::CapacityLevelLow
		|| getCpuFreqLevel() == CapacityLevel::CapacityLevelLow)
	{
		return CapacityLevel::CapacityLevelLow;
	}
	else
	{
		return CapacityLevel::CapacityLevelMedium;
	}
}

CapacityLevel SystemUtil_Android::getMemoryLevel() const
{
	if (m_memoryCapacity >= 4.f)
	{
		return CapacityLevel::CapacityLevelHigh;
	}
	else if (m_memoryCapacity > 2.f)
	{
		return CapacityLevel::CapacityLevelMedium;
	}
	else
	{
		return CapacityLevel::CapacityLevelLow;
	}
}

CapacityLevel SystemUtil_Android::getCpuCoreLevel() const
{
	//LOGD("CPU Core Num : {0}", m_cpuCoresNum);
	if (m_cpuCoresNum >= 8)
	{
		return CapacityLevel::CapacityLevelHigh;
	}
	else if (m_cpuCoresNum > 4)
	{
		return CapacityLevel::CapacityLevelMedium;
	}
	else
	{
		return CapacityLevel::CapacityLevelLow;
	}
}

CapacityLevel SystemUtil_Android::getCpuFreqLevel() const
{
	//LOGD("CPU Max Freq : {0}", m_cpuFreqMHz);
	if (m_cpuFreqMHz >= 2400)
	{
		return CapacityLevel::CapacityLevelHigh;
	}
	else if (m_cpuFreqMHz > 2000)
	{
		return CapacityLevel::CapacityLevelMedium;
	}
	else
	{
		return CapacityLevel::CapacityLevelLow;
	}
}

const tstring& SystemUtil_Android::getDeviceModel()
{
	return m_strDeviceModel;
}

DEVICE_PLATFORM SystemUtil_Android::getDevicePlatform() const
{
	return DP_ANDROID;
}
const tstring& SystemUtil_Android::getPackageName()
{
	return EMPTY_STRING;
}

LANGUAGES SystemUtil_Android::getSystemLanguage()
{
	std::string languageName = JniHelper::callStaticStringMethod(systemUtilClassName, "getCurrentLanguage");
	std::string countryName = JniHelper::callStaticStringMethod(systemUtilClassName, "getCurrentCountry");

    const char* pLanguageName = languageName.c_str();
	m_sysLanguage = LANGUAGES::LANG_EN_US;

    if (0 == strcmp("zh", pLanguageName))
    {
		// 中文分简体繁体(香港/台湾/澳门)
		if (0 == strcmp("TW", countryName.c_str()) || 0 == strcmp("HK", countryName.c_str()) || 0 == strcmp("MO", countryName.c_str()))
		{
			m_sysLanguage = LANGUAGES::LANG_ZH_TW;
		}
		else
		{
			m_sysLanguage = LANGUAGES::LANG_TRUNK;
		}
    }
    else if (0 == strcmp("en", pLanguageName))
    {
		m_sysLanguage = LANGUAGES::LANG_EN_US;
    }
	else if (0 == strcmp("fr", pLanguageName))
	{
		m_sysLanguage = LANGUAGES::LANG_FR;
	}
	else if (0 == strcmp("it", pLanguageName))
	{
		m_sysLanguage = LANGUAGES::LANG_IT;
	}
	else if (0 == strcmp("de", pLanguageName))
	{
		m_sysLanguage = LANGUAGES::LANG_DE;
	}
	else if (0 == strcmp("es", pLanguageName))
	{
		m_sysLanguage = LANGUAGES::LANG_ES;
	}
	else if (0 == strcmp("ru", pLanguageName) || 0 == strcmp("uk", pLanguageName))
	{
		m_sysLanguage = LANGUAGES::LANG_RU;
	}
	else if (0 == strcmp("ko", pLanguageName))
	{
		m_sysLanguage = LANGUAGES::LANG_KR;
	}
	else if (0 == strcmp("ja", pLanguageName))
	{
		m_sysLanguage = LANGUAGES::LANG_JP;
	}
	else if (0 == strcmp("th", pLanguageName))
	{
		m_sysLanguage = LANGUAGES::LANG_TH;
	}
	else if (0 == strcmp("in", pLanguageName))
	{
		m_sysLanguage = LANGUAGES::LANG_ID;
	}
	else if (0 == strcmp("tr", pLanguageName))
	{
		m_sysLanguage = LANGUAGES::LANG_TR;
	}
	else if (0 == strcmp("pt", pLanguageName))
	{
		m_sysLanguage = LANGUAGES::LANG_PT;
	}
	else if (0 == strcmp("vi", pLanguageName))
	{
		m_sysLanguage = LANGUAGES::LANG_VN;
	}

	LOGD(" ===================================== getSystemLanguage = " + languageName);
	LOGD(" ===================================== countryName = " + countryName);

    return m_sysLanguage;
}

const tstring& SystemUtil_Android::getVersionName()
{
	return m_strVersionName;
}

const tstring& SystemUtil_Android::getAndroidVersion() const
{
	return m_strAndroidVersion;
}

int SystemUtil_Android::getAPIVersion()
{
	tstring strApiLevel = JniHelper::callStaticStringMethod(systemUtilClassName, "getAPILevel");
	return atoi(strApiLevel.c_str());
}

const tstring& SystemUtil_Android::getCpuInfo() const
{
	return m_strCpuInfoName;
}

const tstring& SystemUtil_Android::getAppleGameCenterId() const
{
	return EMPTY_STRING;
}

UI64 SystemUtil_Android::getTotalPhysicsMemory()
{
	static const char* const sums[] = { "MemTotal:", NULL };
	static const size_t sumsLen[] = { strlen("MemTotal:"), 0 };
	return getFreeMemoryImpl(sums, sumsLen, 1) * 1024;
}

UI64 SystemUtil_Android::getFreePhysicsMemory()
{
	static const char* const sums[] = { "MemFree:", "Cached:", NULL };
	static const size_t sumsLen[] = { strlen("MemFree:"), strlen("Cached:"), 0 };
	return getFreeMemoryImpl(sums, sumsLen, 2) * 1024;
}

UI64 SystemUtil_Android::getProcessMemoryHWM()
{
	struct stats_t stats;
	memset(&stats, 0, sizeof(stats_t));
	load_maps(getpid(), &stats);

	return (stats.dalvikPss + stats.nativePss + stats.otherPss) * 1024;
}

UI64 SystemUtil_Android::getProcessAvaMemory()
{
	struct proc_info procinfo;
	memset(&procinfo, 0, sizeof(proc_info));
	load_status(getpid(), &procinfo);

	return procinfo.VmHWM * 1024;
}

int SystemUtil_Android::getNetworkType() const
{
	tstring str = JniHelper::callStaticStringMethod(GameEntryCalssName, "getNetworkType");
	return atoi(str.c_str());
}

const tstring& SystemUtil_Android::getAId() const
{
	static tstring aid = "123";
	return aid;
}

bool SystemUtil_Android::downloadObb(const OnGetObbUrlFunc& func)
{
	m_onGetObbUrlFunc = func;

	JniHelper::callStaticStringMethod(ObbDownladerCalssName, "downloadObb");

	return true;
}

tstring SystemUtil_Android::getObbDownloadProgress()
{
	std::string bytesAndStatus = JniHelper::callStaticStringMethod(ObbDownladerCalssName, "getBytesAndStatus");
	return bytesAndStatus;
}

I64 SystemUtil_Android::getObbFreeSpace()
{
	tstring str = JniHelper::callStaticStringMethod(ObbDownladerCalssName, "getObbFreeSpace");
	I64 ret{0};
	StringUtil::stringToType(ret, str);
	return ret;
}

tstring SystemUtil_Android::getGameId(LANGUAGES language)
{
	switch (language)
	{
	case LANGUAGES::LANG_EN_US:
		return "1077010202";
	case LANGUAGES::LANG_TRUNK:
		return "1077190202";
	case LANGUAGES::LANG_ZH_TW:
		return "1077020202";
	case LANGUAGES::LANG_RU:
		return "1077070202";
	case LANGUAGES::LANG_VN:
		return "1077150202";
	case LANGUAGES::LANG_TH:
		return "1077100202";
	case LANGUAGES::LANG_JP:
		return "1077080202";
	case LANGUAGES::LANG_KR:
		return "1077130202";
	case LANGUAGES::LANG_DE:
		return "1077050202";
	case LANGUAGES::LANG_FR:
		return "1077060202";
	case LANGUAGES::LANG_ID:
		return "1077110202";
	case LANGUAGES::LANG_PT:
		return "1077220202";
	case LANGUAGES::LANG_TR:
		return "1077160202";
	case LANGUAGES::LANG_IT:
		return "1077120202";
	case LANGUAGES::LANG_ES:
		return "1077090202";
	default:
		return "1077010202";
		break;
	}
}

tstring SystemUtil_Android::getGameKey(LANGUAGES language)
{
	switch (language)
	{
	case LANGUAGES::LANG_EN_US:
		return "3813e8c5b2e98b56a4de951384689d22";
	case LANGUAGES::LANG_TRUNK:
		return "4996be5f2ef80039a298ecc29d447fb5";
	case LANGUAGES::LANG_ZH_TW:
		return "bebb0e8299684d78fffb4c0889e72bff";
	case LANGUAGES::LANG_RU:
		return "55509836ce5b63bf34802a3773be0ed1";
	case LANGUAGES::LANG_VN:
		return "e3a76bdfec93432a107e9581ce071964";
	case LANGUAGES::LANG_TH:
		return "b4d5f4e2bb9227b435885c8b3b6668b1";
	case LANGUAGES::LANG_JP:
		return "8dbaa13575df2dd111b9ae617653f08a";
	case LANGUAGES::LANG_KR:
		return "081ab38f850b01d3a45f0781181a98cf";
	case LANGUAGES::LANG_DE:
		return "9223fce304fe71d950bf249154b926cd";
	case LANGUAGES::LANG_FR:
		return "c74665c1891ad693a0ed88067b586daa";
	case LANGUAGES::LANG_ID:
		return "732801f21d07995ff5f0cb4a0c2bce0a";
	case LANGUAGES::LANG_PT:
		return "e6eb1aa6794a0fd95633bdcf3426d9b8";
	case LANGUAGES::LANG_TR:
		return "7d6f06031d87d2f97e2cdfe5b06609d1";
	case LANGUAGES::LANG_IT:
		return "443501289aa97e9d3e7e770f80b9f1e5";
	case LANGUAGES::LANG_ES:
		return "93f154acd199e1deb843cee90e94b7d6";
	default:
		return "3813e8c5b2e98b56a4de951384689d22";
		break;
	}
}

tstring SystemUtil_Android::getPaymentKey()
{
	// PaymentKey offer by IGG
	return "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAiZ1Sc2GqQW3T5bnkIfR0e71z0mciehpYSCZ7B9tID50BeAEbcIvv3qv1YAa1k1s4JkRQ+9CRXVvyfvztCLKPqkJh0dw5abhHjpNpZP1yLb3lOndq72D5VU5TyU2wOlawA4iJBOEsX/q396idHCeruZNxFKT72shj6xQMJfDlGubnNmetPxOnaNlDG9TrIMbFhx8BF8AObxmgai+qr2TTrdkw9FejPW08Pa36IyWyzRQ4/ff8QsSwdXKGXpDCL+yQDRjakbn6rJYvFrTZZLQja+Vz99FH0ias2GhPXmjpsNGpN4CmV5y2WtarGGbz8debS8bbR2yk6VeKb/FE+afesQIDAQAB";
}

tstring SystemUtil_Android::getServiceEmail(LANGUAGES language)
{
	switch (language)
	{
	case LANGUAGES::LANG_EN_US:
		return "help.craftlegend.android@igg.com";
	case LANGUAGES::LANG_TRUNK:
		return "help.craftlegend.android.cn@igg.com";
	case LANGUAGES::LANG_ZH_TW:
		return "help.craftlegend.android.tw@igg.com";
	case LANGUAGES::LANG_RU:
		return "help.craftlegend.android.ru@igg.com";
	case LANGUAGES::LANG_VN:
		return "help.craftlegend.android.vn@igg.com";
	case LANGUAGES::LANG_TH:
		return "help.craftlegend.android.th@igg.com";
	case LANGUAGES::LANG_JP:
		return "help.craftlegend.android.jp@igg.com";
	case LANGUAGES::LANG_KR:
		return "help.craftlegend.android.kr@igg.com";
	case LANGUAGES::LANG_DE:
		return "help.craftlegend.android.de@igg.com";
	case LANGUAGES::LANG_FR:
		return "help.craftlegend.android.fr@igg.com";
	case LANGUAGES::LANG_ID:
		return "help.craftlegend.android.id@igg.com";
	case LANGUAGES::LANG_PT:
		return "help.craftlegend.android.pt@igg.com";
	case LANGUAGES::LANG_TR:
		return "help.craftlegend.android.tr@igg.com";
	case LANGUAGES::LANG_IT:
		return "help.craftlegend.android.it@igg.com";
	case LANGUAGES::LANG_ES:
		return "help.craftlegend.android.es@igg.com";
	default:
		return "help.craftlegend.android@igg.com";
		break;
	}
}

tstring SystemUtil_Android::getAppName()
{
	return m_strAppName;
}

void SystemUtil_Android::setAppName(const tstring& appName)
{
	m_strAppName = appName;
}

bool SystemUtil_Android::playVideo(const tstring& filePath , bool bLoop)
{
    JniMethodInfo methodInfo;
    if (!JniHelper::getStaticMethodInfo(methodInfo, "com/igg/bzbee/static_app/OpenGLESView", "playVideo", "(Ljava/lang/String;JJZ)Z"))
    {
		return false;
	}

	tstring absoluteFilePath{ filePath };
	long offset, length;
	if (!FileSystem::getFileLocation(absoluteFilePath, offset, length))
	{
		LOGE("playVideo: file {0} not found.", filePath);
		return false;
	}
    jstring jstrFilePath = methodInfo.env->NewStringUTF(absoluteFilePath.c_str());
	jlong javaOffset{offset}, javaLength{length};
    auto ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, jstrFilePath, javaOffset, javaLength, bLoop);
	methodInfo.env->DeleteLocalRef(jstrFilePath);

	LOGI("CallStaticVoidMethod playVideo");
	return ret;
}

void SystemUtil_Android::seekTo(int msec)
{
	JniMethodInfo methodInfo;
	if (!JniHelper::getStaticMethodInfo(methodInfo, "com/igg/bzbee/static_app/OpenGLESView", "seekTo", "(I)V")) return;

	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, msec);

	LOGI("CallStaticVoidMethod seekTo");
}

void SystemUtil_Android::stop()
{
	JniMethodInfo methodInfo;
	if (!JniHelper::getStaticMethodInfo(methodInfo, "com/igg/bzbee/static_app/OpenGLESView", "stop", "()V"))
	{
		LOGI("CallStaticVoidMethod pre stop error");
		return;
	}

	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);

	LOGI("CallStaticVoidMethod stop");
}
