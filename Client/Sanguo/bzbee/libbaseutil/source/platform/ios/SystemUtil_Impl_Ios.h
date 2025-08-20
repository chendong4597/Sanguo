/*!
 * \file SystemUtil_Impl_Ios.h
 * \date 2018/09/10
 * 
 * 
 * \author lzj
 */
#pragma once

#import <SystemUtil.h>
#import <Foundation/Foundation.h>
#import <msg/clientMsgPb.pb.h>
#import <MsgDecoder.h>

NS_BEGIN

class SystemUtil_Impl_Ios : public SystemUtil
{
public:
    SystemUtil_Impl_Ios();

    const tstring& getDeviceId() override;
    const tstring& getDeviceModel() override;
    CapacityLevel getDeviceLevel() const override;
    CapacityLevel getMemoryLevel() const override;
    DEVICE_PLATFORM getDevicePlatform() const override;
    const tstring& getPackageName() override;
    const tstring& getLanguage() override;
    const tstring& getVersionName() override;
    const tstring& getAppleGameCenterId() const override;
    const tstring& getAId() const override;
    
    void shake() override;
    bool playVideo(const tstring &filePath , bool bLoop) override;
    void seekTo(int msec) override;
    void stop() override;

    uint64 getTotalPhysicsMemory() override;
    uint64 getFreePhysicsMemory() override;
    uint64 getProcessAvaMemory() override;
    uint64 getProcessMemoryHWM() override;

    int getNetworkType() const override;

    void requestProductData(const tstring& pcId) const override;
    tstring getProductName(const tstring& pcId) const override;
    bool isProductDataLoaded() const override;
    void showInterstitialAd() const override;
    void onHandlersInitialized() const override;
    tstring getBundleURLScheme() const override;
    const tstring& getSignedKey() const { return m_signed_key;}
    tstring getSystemVersion() const override;

    void scheduleLocalNotification(int64 remain_secs, const tstring& id, const tstring& message) const override;
    void unscheduleLocalNotification(const tstring& id) const override;

    void onFBShareSuccess() const;
    void onFBShareCancel() const;
    void onFBShareErr() const;

    void openDeepLinkReward() const;
    void setDeepLink(const tstring& deep_link) { m_deep_link = deep_link; }
    const tstring& getDeepLink() const { return m_deep_link; }
    bool hasEnterLobby() const { return m_has_enter_lobby; }
    NSString* getUUID();
    virtual tstring getGameId() override;
    virtual tstring getGameKey() override;
    virtual tstring getServiceEmail() override;
    virtual tstring getAppName() override;

    bool processLocalMsg(Msg* pProtoMsg, uint16 nMsgId);
    
private:
    bool isPackageInstalled(const tstring& packageName) const;
    tstring getIpAddress() const;

    bool initWeGamers(const tstring& accoundId, const tstring& name);
    void openWeGamers();
    void onLogFacebookEvent(com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgLocFaceBookLog* pProtoMsg);
    void LogStandardEvent(tstring strType);
    void AchievedLevelEvent(com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgLocFaceBookLog* pProtoMsg);
    void PurchasedEvent(com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgLocFaceBookLog* pProtoMsg);
    void SpentcreditsEvent(com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgLocFaceBookLog* pProtoMsg);
    void CustomizeEvent(com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgLocFaceBookLog* pProtoMsg);
private:
    bool m_weGamersInitialized{};
    bool m_bPushInitialized;
    tstring m_strDeviceId;
    tstring m_signed_key;
    tstring m_deep_link;
    bool m_has_enter_lobby;
};

NS_END
