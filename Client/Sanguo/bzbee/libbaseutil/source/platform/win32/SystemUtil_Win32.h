/*!
 * \file SystemUtil_Win32.h
 * \date 12-10-2012 13:07:49
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#pragma once
#include "../../SystemUtil.h"
#include <direct.h>
#include <io.h>

NS_BEGIN

class SystemUtil_Win32 : public SystemUtil
{
public:
	RTTI_DEF(SystemUtil_Win32);

	SystemUtil_Win32();
	virtual ~SystemUtil_Win32();

	const tstring& getDeviceId() override;
	CapacityLevel getDeviceLevel() const override;
	CapacityLevel getMemoryLevel() const override;
	const tstring& getDeviceModel() override;
	DEVICE_PLATFORM getDevicePlatform()const override;
	const tstring& getPackageName() override;
    LANGUAGES getSystemLanguage() override;
    const tstring& getVersionName() override;
	const tstring& getAppleGameCenterId() const override;

	void setDeviceLevel(CapacityLevel deviceLevel) override;

	UI64 getTotalPhysicsMemory() override;
	UI64 getFreePhysicsMemory() override;
	UI64 getProcessAvaMemory() override;
	UI64 getProcessMemoryHWM() override;

	int getNetworkType() const override;
	const tstring& getAId() const override;

	virtual tstring getGameId(LANGUAGES lang) override;

private:
	tstring m_strDeviceId;
	tstring m_strDeviceModel;
	tstring m_strVersionName;

};

NS_END
