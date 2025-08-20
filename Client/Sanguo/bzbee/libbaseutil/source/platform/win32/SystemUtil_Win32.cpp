/*!
 * \file SystemUtil_Win32.cpp
 * \date 12-10-2012 13:07:53
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#include "SystemUtil_Win32.h"
#include "../../Cache.h"

NS_BEGIN

SystemUtil& SystemUtil::getInstance()
{
	static SystemUtil_Win32 s_SystemUtil_Win32;
	return s_SystemUtil_Win32;
}

SystemUtil_Win32::SystemUtil_Win32()
:m_strDeviceId(EMPTY_STRING)
,m_strDeviceModel("Win32_PC")
,m_strVersionName("0.0.0")
{
	m_deviceLevel = CapacityLevel::CapacityLevelHigh;
}

SystemUtil_Win32::~SystemUtil_Win32()
{

}

const tstring& SystemUtil_Win32::getDeviceId()
{
	m_strDeviceId = Cache::getInstance().getGlobalString("DEVICE_ID", EMPTY_STRING);

	if (m_strDeviceId.empty())
	{
		I64 base = 350000000000000;
		I64 id = base + rand();
		StringUtil::stringToType<I64>(id, m_strDeviceId);
		Cache::getInstance().setGlobalString("DEVICE_ID", m_strDeviceId);
		Cache::getInstance().flushGlobal();
	}

	return m_strDeviceId;
}

CapacityLevel SystemUtil_Win32::getDeviceLevel() const
{
	//return CapacityLevel::CapacityLevelHigh;
	return m_deviceLevel;
}

CapacityLevel SystemUtil_Win32::getMemoryLevel() const
{
	return CapacityLevel::CapacityLevelHigh;
}

const tstring& SystemUtil_Win32::getDeviceModel()
{
	return m_strDeviceModel;
}

DEVICE_PLATFORM SystemUtil_Win32::getDevicePlatform() const
{
	return DP_WIN32;
}

const tstring& SystemUtil_Win32::getPackageName()
{
	return EMPTY_STRING;
}

LANGUAGES SystemUtil_Win32::getSystemLanguage()
{
    return LANG_TRUNK;
}

const tstring& SystemUtil_Win32::getVersionName()
{
	return m_strVersionName;
}

const tstring& SystemUtil_Win32::getAppleGameCenterId() const
{
	return EMPTY_STRING;
}

void SystemUtil_Win32::setDeviceLevel(CapacityLevel deviceLevel)
{
	m_deviceLevel = deviceLevel;
}

tstring SystemUtil_Win32::getGameId(LANGUAGES lang)
{
	return "1";
}

UI64 SystemUtil_Win32::getFreePhysicsMemory()
{
	return 0;
}

UI64 SystemUtil_Win32::getTotalPhysicsMemory()
{
	return 0;
}

UI64 SystemUtil_Win32::getProcessAvaMemory()
{
	return 0;
}

UI64 SystemUtil_Win32::getProcessMemoryHWM()
{
	return 0;
}

int SystemUtil_Win32::getNetworkType() const
{
	return SystemUtil::TYPE_WIFI;
}

const tstring& SystemUtil_Win32::getAId() const
{
	static tstring aid = "123";
	return aid;
}

NS_END
