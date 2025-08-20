/*!
 * \file SystemUtil_Android.h
 * \date 12-10-2012 13:07:49
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#pragma once
#include "../../SystemUtil.h"
#include "../../Cache.h"

NS_BEGIN

class SystemUtil_Android : public SystemUtil
{
public:
	RTTI_DEF(SystemUtil_Android);

	SystemUtil_Android();
	virtual ~SystemUtil_Android();

	const tstring& getDeviceId() override;
	CapacityLevel getDeviceLevel() const override;
	CapacityLevel getMemoryLevel() const override;
	CapacityLevel getCpuCoreLevel() const;
	CapacityLevel getCpuFreqLevel() const;
	const tstring& getDeviceModel() override;
	DEVICE_PLATFORM getDevicePlatform() const override;
	const tstring& getPackageName() override;
	LANGUAGES getSystemLanguage() override;
	const tstring& getVersionName() override;
	const tstring& getAppleGameCenterId() const override;
	virtual const tstring& getAndroidVersion() const override;
	virtual const tstring& getCpuInfo() const override;
	virtual int getAPIVersion() override;

	virtual void setDeviceModel(tstring devModelName) override {m_strDeviceModel = devModelName;}
	virtual void setDeviceId(tstring deviceId) override { m_strDeviceId = deviceId; }
	virtual void setVersionName(tstring versionName)  override { m_strVersionName = versionName; }
	virtual void setAndroidVersion(tstring versionName)  override { m_strAndroidVersion = versionName; }
	virtual void setCpuInfo(tstring versionName)  override { m_strCpuInfoName = versionName; }

	bool playVideo(const tstring& filePath , bool bLoop) override;
	void seekTo(int msec) override;
	void stop() override;

	UI64 getTotalPhysicsMemory() override;
	UI64 getFreePhysicsMemory() override;
	UI64 getProcessAvaMemory() override;
	UI64 getProcessMemoryHWM() override;

	int getNetworkType() const override;
	const tstring& getAId() const override;
	
	virtual bool downloadObb(const OnGetObbUrlFunc& func) override;

	virtual tstring getObbDownloadProgress() override;

	virtual I64 getObbFreeSpace() override;

	virtual tstring getServiceEmail(LANGUAGES language) override;
	virtual tstring getAppName() override;
	virtual void setAppName(const tstring& appName) override;

	virtual tstring getGameId(LANGUAGES language) override;
	virtual tstring getGameKey(LANGUAGES language) override;
	virtual tstring getPaymentKey() override;

private:
	tstring m_strDeviceId;
	tstring m_strDeviceModel;
	tstring m_strVersionName;
	LANGUAGES m_sysLanguage;
	tstring m_strExternalDir;
	tstring m_strAndroidVersion;
	tstring m_strCpuInfoName;
	tstring m_strAppName;
};

NS_END
