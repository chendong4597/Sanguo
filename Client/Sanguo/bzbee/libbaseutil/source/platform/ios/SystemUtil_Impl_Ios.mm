/*!
 * \file SystemUtil_Impl_Ios.cpp
 * \date 2018/09/10
 * 
 * 
 * \author lzj
 */

#import "SystemUtil_Impl_Ios.h"

#import "LoginHelper.h"
#import "Payment.h"
#import "RealNameVerification.h"
#import "ViewController.h"
#import "EAGLView.h"

#import "IGGCRMMgr.h"
#import <Crashlytics/Crashlytics.h>

#import "AppDelegate.h"
#import <msg/clientMsgPb.pb.h>
#import <DispatchMsg.h>
#import <IGGSDK/IGGLogin.h>
#import <IGGSDK/IGGCRM.h>
#import "FriendAdd.h"
#import "AdvertisingRealisation.h"
#import "FriendAdd.h"
#import "AppConfig.h"

#define IFT_ETHER 0x6

#include <Config.h>
#include <Cache.h>
#include <StringUtil.h>
#import <UIKit/UIKit.h>
#import <AdSupport/ASIdentifierManager.h>
#import <GameCommunitySDK/GameCommunitySDK.h>
#import <GameKit/GKLocalPlayer.h>
#import <QuartzCore/QuartzCore.h>
#import <SystemConfiguration/SCNetworkReachability.h>

//#include <value.h>
//#include <reader.h>
#include <sys/stat.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <fstream>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <net/if_dl.h>
#include <ifaddrs.h>
#include <sys/sysctl.h>
#include <pthread.h>
#import <AppsFlyerLib/AppsFlyerTracker.h>
#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKCoreKit/FBSDKAppEvents.h>

#import "iosWebView.h"
#include "EAGLView.h"
namespace
{
//    const bzbee::tstring MAT_SIGNUP = "MAT_SIGNUP";
}

char* getMacAddress(char* macAddress, const char* ifName)
{
    int  success;
    struct ifaddrs * addrs;
    struct ifaddrs * cursor;
    const struct sockaddr_dl * dlAddr;
    const unsigned char* base;
    int i;
    
    success = getifaddrs(&addrs) == 0;
    if (success) {
        cursor = addrs;
        while (cursor != 0) {
            if ( (cursor->ifa_addr->sa_family == AF_LINK)
                && (((const struct sockaddr_dl *) cursor->ifa_addr)->sdl_type == IFT_ETHER) && strcmp(ifName,  cursor->ifa_name)==0 ) {
                dlAddr = (const struct sockaddr_dl *) cursor->ifa_addr;
                base = (const unsigned char*) &dlAddr->sdl_data[dlAddr->sdl_nlen];
                strcpy(macAddress, "");
                for (i = 0; i < dlAddr->sdl_alen; i++) {
                    if (i != 0) {
                        strcat(macAddress, ":");
                    }
                    char partialAddr[3];
                    sprintf(partialAddr, "%02X", base[i]);
                    strcat(macAddress, partialAddr);
                    
                }
            }
            cursor = cursor->ifa_next;
        }
        
        freeifaddrs(addrs);
    }    
    return macAddress;
}

NS_BEGIN

NSString* SystemUtil_Impl_Ios::getUUID()
{
    CFUUIDRef UUID = CFUUIDCreate(nil);
    CFStringRef UUIDString = CFUUIDCreateString(nil, UUID);
    
    NSString * result = (NSString *)CFBridgingRelease(CFStringCreateCopy( NULL, UUIDString));
    
    CFRelease(UUID);
    CFRelease(UUIDString);
    
    return result;
}

SystemUtil& SystemUtil::getInstance()
{
    static SystemUtil_Impl_Ios s_SystemUtil_Impl_Ios;
    return s_SystemUtil_Impl_Ios;
}

SystemUtil_Impl_Ios::SystemUtil_Impl_Ios()
{
    m_bPushInitialized = false;
    m_has_enter_lobby = false;
}

const tstring& SystemUtil_Impl_Ios::getDeviceId()
{
    m_strDeviceId = [[[[IGGEnv sharedInstance] deviceRegisterId] value] UTF8String];
    const tstring LAST_LOGIN_IGG_UUID = "LAST_LOGIN_IGG_UUID";
    Cache::getInstance().setString(LAST_LOGIN_IGG_UUID, m_strDeviceId);
    return m_strDeviceId;
    
//    auto uuid = getUUID();
//    const tstring LOGIN_IGG_UUID_ZONE = "00000000-0000-0000-0000-000000000000";
//    if (m_strDeviceId.empty() || m_strDeviceId == LOGIN_IGG_UUID_ZONE)
//    {
//        const tstring LAST_LOGIN_IGG_UUID = "LAST_LOGIN_IGG_UUID";
//        tstring gameUUID = Cache::getInstance().getString(LAST_LOGIN_IGG_UUID, EMPTY_STRING);
//        NSString *adId = [[NSString alloc]initWithCString:gameUUID.c_str() encoding:NSUTF8StringEncoding];
//        BOOL result = [adId hasPrefix:@"00000000"];//[adId isEqualToString: strZone];
//        if ([adId length] > 0 && result != YES)
//        {
//            m_strDeviceId = [adId UTF8String];
//        }
//        else
//        {
//            NSString* strVersion = [[UIDevice currentDevice] systemVersion];
//            float version = [strVersion floatValue];
//            if (version < 6.0) { // ios6以下读取mac
//                static tchar s_MacAddr[20] = {0};
//                m_strDeviceId = ::getMacAddress(s_MacAddr, "en0");
//            }
//            else
//            { // ios6及以上读取adid
//                BOOL isAdfiIDEnabled = [ASIdentifierManager sharedManager].isAdvertisingTrackingEnabled;
//                if (version >= 10.0)
//                {
//                    if(isAdfiIDEnabled == YES)
//                    {
//                        adId = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
//                        result = [adId hasPrefix:@"00000000"];
//                        if([adId length] == 0 || result == YES)
//                        {
//                            adId = getUUID();
//                        }
//                        
//                    }
//                    else
//                    {
//                        adId = getUUID();
//                    }
//                }
//                else
//                {
//                    adId = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
//                    result = [adId hasPrefix:@"00000000"];
//                    if([adId length] == 0 || result == YES)
//                    {
//                        adId = getUUID();
//                    }
//                }
//                m_strDeviceId = [adId UTF8String];
//            }
//             Cache::getInstance().setString(LAST_LOGIN_IGG_UUID, m_strDeviceId);
//        }
//        //
//    }
//
//    return m_strDeviceId;
}

CapacityLevel SystemUtil_Impl_Ios::getDeviceLevel() const
{
    return getMemoryLevel();
}

CapacityLevel SystemUtil_Impl_Ios::getMemoryLevel() const
{
    if (m_memoryCapacity >= 2.f)
    {
        return CapacityLevel::CapacityLevelHigh;
    }
    else if (m_memoryCapacity >= 1.f)
    {
        return CapacityLevel::CapacityLevelMedium;
    }
    else
    {
        return CapacityLevel::CapacityLevelLow;
    }
}

const tstring& SystemUtil_Impl_Ios::getDeviceModel()
{
    static const tstring strDeviceModel = tstring([[UIDevice currentDevice].model UTF8String])+" "
        +[[UIDevice currentDevice].systemName UTF8String]+[[UIDevice currentDevice].systemVersion UTF8String];
    return strDeviceModel;
}

DEVICE_PLATFORM SystemUtil_Impl_Ios::getDevicePlatform() const
{
    return DP_IOS;
}

const tstring& SystemUtil_Impl_Ios::getAppleGameCenterId() const
{
#if 0
    GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
    
    static tstring strPlayerId;
    if (localPlayer.isAuthenticated) {
        strPlayerId =[localPlayer.playerID UTF8String];
        return strPlayerId;
    }
    else
#endif
    {
        return EMPTY_STRING;
    }
}

const tstring& SystemUtil_Impl_Ios::getAId() const
{
    NSString *adId = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
    static const tstring strAId =[adId UTF8String];
    return strAId;
}

const tstring& SystemUtil_Impl_Ios::getVersionName()
{
    static const tstring strVerson =[[[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"] UTF8String];
    return strVerson;
}

const tstring& SystemUtil_Impl_Ios::getPackageName()
{
    static const tstring strBundleIdentifier = [[[NSBundle mainBundle] bundleIdentifier] UTF8String];
    return strBundleIdentifier;
}

const tstring& SystemUtil_Impl_Ios::getLanguage()
{
    static tstring strLanguage;
    NSString* language = [[NSLocale preferredLanguages] objectAtIndex:0];
    strLanguage = [language UTF8String];
    if(strLanguage == "zh-Hant-CN" || strLanguage == "zh-Hant-HK" || strLanguage == "zh-Hant-TW"
       || strLanguage == "zh-Hant-MO")
        strLanguage = "zh_tw";
    else if(strLanguage.find("vi") == 0)
        strLanguage = "vn";
    else if(strLanguage.find("ru") == 0)
        strLanguage = "ru";
    else if(strLanguage.find("fr") == 0)
        strLanguage = "fr";
    else if(strLanguage.find("de") == 0)
        strLanguage = "de";
    else if(strLanguage.find("en") == 0)
        strLanguage = "en";
    else if(strLanguage.find("ja") == 0)
        strLanguage = "jp";
    else if(strLanguage.find("th") == 0)
        strLanguage = "th";
    else if(strLanguage.find("ko") == 0)
        strLanguage = "kr";
    else if(strLanguage.find("id") == 0)
        strLanguage = "id";
    else if(strLanguage.find("pt") == 0)
        strLanguage = "pt";
    else if(strLanguage.find("tr") == 0)
        strLanguage = "tr";
    else if(strLanguage.find("it") == 0)
        strLanguage = "it";
    else if(strLanguage.find("es") == 0)
        strLanguage = "es";
    else if(strLanguage.find("uk") == 0)
        strLanguage = "ru";
    return strLanguage;
}

tstring SystemUtil_Impl_Ios::getServiceEmail()
{
    switch (Config::getInstance().getCurrLanguage())
    {
        case Config::LANG_TRUNK: //cn
            return "ihelp.craftlegend.cn@igg.com";
            break;
        case Config::LANG_EN_US: //en
            return "ihelp.craftlegend@igg.com";
            break;
        case Config::LANG_ZH_TW: //tw
            return "ihelp.craftlegend.tw@igg.com";
            break;
        case Config::LANG_RU: //ru
            return "ihelp.craftlegend.ru@igg.com";
            break;
        case Config::LANG_VN: //vn
            return "ihelp.craftlegend.vn@igg.com";
            break;
        case Config::LANG_TH: //th
            return "ihelp.craftlegend.th@igg.com";
            break;
        case Config::LANG_JP: //jp
            return "ihelp.craftlegend.jp@igg.com";
            break;
        case Config::LANG_KR: //kr
            return "ihelp.craftlegend.kr@igg.com";
            break;
        case Config::LANG_DE: //de
            return "ihelp.craftlegend.de@igg.com";
            break;
        case Config::LANG_FR: //fr
            return "ihelp.craftlegend.fr@igg.com";
            break;
        case Config::LAN_ID: //id
            return "ihelp.craftlegend.id@igg.com";
            break;
        case Config::LANG_PT: //PT
            return "ihelp.craftlegend.pt@igg.com";
            break;
        case Config::LANG_TR: //TR
            return "ihelp.craftlegend.tr@igg.com";
            break;
        case Config::LANG_IT: //IT
            return "ihelp.craftlegend.it@igg.com";
            break;
        case Config::LANG_ES: //ES
            return "ihelp.craftlegend.es@igg.com";
            break;
        default:
            return "ihelp.craftlegend@igg.com";
    }
}

tstring SystemUtil_Impl_Ios::getAppName()
{
    NSString *appName = [[NSBundle mainBundle].infoDictionary valueForKey:@"CFBundleDisplayName"];
    return [appName UTF8String];
}

bool SystemUtil_Impl_Ios::processLocalMsg(Msg* pProtoMsg, uint16 nMsgId)
{
    if (!pProtoMsg) { return false; }
    
    AppDelegate* appDelegate = (AppDelegate*)[[UIApplication sharedApplication] delegate];
    using namespace com::igg::bzbee::app_sandbox::platform::network::MsgPB;
    switch (nMsgId)
    {
        case MSG_LOC_OPEN_EMAIL_REQUEST:
        {
            auto msg = static_cast<MsgLocOpenEmailRequest*>(pProtoMsg);
            NSString *recvAddress = [NSString stringWithUTF8String :msg->m_recvaddress().c_str()];
            NSString *title = [NSString stringWithUTF8String :msg->m_title().c_str()];
            NSString *content = [NSString stringWithUTF8String :msg->m_content().c_str()];
            [LoginHelper openMailReq:recvAddress mailtitle:title mailCont:content];
        }
        break;
        case MSG_LOC_RELOAD_SERVER_CONFIG:
        {
            [[AppConfig sharedInstance] loadConfig];
        }
        break;
        case MSG_LOC_GET_ACCOUNT_INFO_REQ:
        {
            [LoginHelper loaduserInfo];
        }
        break;
        case MSG_LOC_UPLOAD_LOG_FILE_REQ:
        {
            auto msg = static_cast<MsgLocUploadLogFileReq*>(pProtoMsg);
            NSString *file = [NSString stringWithUTF8String :msg->logfilefullpath().c_str()];
            [LoginHelper onUploadLogFile:file fileIndex:msg->logfileindex()];
        }
            break;
        case MSG_LOC_REGISTER_FRIENDS_REQ:
        {
            [[FriendAdd sharedInstance] addfriends];
        }
            break;
        case MSG_LOC_SUGGEST_FRIENDS_REQ:
        {
            [[FriendAdd sharedInstance] suggestion];
        }
            break;
        case MSG_LOC_WATCH_ADMOB_REQ:
        {
            auto msg = static_cast<MsgWatchAdmobReq*>(pProtoMsg);
            if (msg->optype() == MsgWatchAdmobReq_admobType_watch)
            {
                [[AdvertisingRealisation getInstance] showMobAdsVideo];
            }
            else
            {
                //                [FBAdSettings setLogLevel :FBAdLogLevelLog];
                //                [FBAdSettings addTestDevice :@"HASHED_ID"];
                // [FBAdSettings clearTestDevice :[ FBAdSettings testDeviceHash ]];
                [[AdvertisingRealisation getInstance] initMobAdsVideo];
            }
        }
            break;
        case MSG_INIT_SDK_REQ:
        {
            [LoginHelper initSdk];
        }
            break;
        case MSG_LOC_GAME_INVALID_ACCOUNT:
        {
            [[IGGSessionManager currentSession] setExpiredAt:[NSDate date]];
        }
        break;
        case MSG_BIND_MAIL_REQ:
        {
            auto msg = static_cast<MsgBindMailReq*>(pProtoMsg);
            NSString *text = [NSString stringWithUTF8String :msg->mail().c_str()];
            [LoginHelper bindMail:text];
        }
            break;
        case MSG_GET_BIND_MAIL_VERFICATION_REQ:
        {
            auto msg = static_cast<MsgGetBindMailVerificationReq*>(pProtoMsg);
            NSString *text = [NSString stringWithUTF8String :msg->mail().c_str()];
            [LoginHelper getBindMailVality:text];
        }
            break;
        case MSG_USE_PWD_VALITY_BIND_MAIL_REQ:
        {
            auto msg = static_cast<MsgUsePwdAndValityBindMailReq*>(pProtoMsg);
            NSString *mail = [NSString stringWithUTF8String :msg->mail().c_str()];
            NSString *pwsd = [NSString stringWithUTF8String :msg->pwd().c_str()];
            NSString *value = [NSString stringWithUTF8String :msg->vality().c_str()];
            if(value.length <= 0)
            {
                [LoginHelper usePwdBindMail:pwsd email:mail];
            }
            else
            {
                [LoginHelper usePwdAndValityBindMail:mail pwd:pwsd vality:value];
            }
        }
            break;
        case MSG_MAIL_SWITCH_REQ:
        {
            auto msg = static_cast<MsgSwitchMailReq*>(pProtoMsg);
            NSString *mail = [NSString stringWithUTF8String :msg->mail().c_str()];
            [LoginHelper useMailSwitchAccount:mail];
        }
            break;
        case MSG_USE_MAIL_PWD_LOGIN_REQ:
        {
            auto msg = static_cast<MsgUsePwdLoginMailReq*>(pProtoMsg);
            NSString *mail = [NSString stringWithUTF8String :msg->mail().c_str()];
            NSString *pwd = [NSString stringWithUTF8String :msg->pwd().c_str()];
            int haveBind = msg->havebind();
            [LoginHelper useMailPwdLoginReq:pwd email:mail haveBindIggid:haveBind];
        }
            break;
        case MSG_USE_PWD_VALITY_LOGIN_MAIL_REQ:
        {
            auto msg = static_cast<MsgMailUsePwdAndValityLoginReq*>(pProtoMsg);
            NSString *mail = [NSString stringWithUTF8String :msg->mail().c_str()];
            NSString *pwd = [NSString stringWithUTF8String :msg->pwd().c_str()];
            NSString *code = [NSString stringWithUTF8String :msg->vality().c_str()];
            [LoginHelper useMailPwdAndValityLoginReq:pwd email:mail Valitycode:code];
        }
            break;
        case MSG_MODIFE_MAIL_PWD_REQ:
        {
            auto msg = static_cast<MsgModifMailPwdReq*>(pProtoMsg);
            NSString *mail = [NSString stringWithUTF8String :msg->mail().c_str()];
            NSString *pwd = [NSString stringWithUTF8String :msg->curpwd().c_str()];
            NSString *newPwd = [NSString stringWithUTF8String :msg->newpwd().c_str()];
            [LoginHelper modifMailPwdReq:mail oldPwd:pwd newPwd:newPwd];
        }
            break;
        case MSG_RESET_MAIL_PWD_REQ:
        {
            auto msg = static_cast<MsgResetMailPwdReq*>(pProtoMsg);
            NSString *pwd = [NSString stringWithUTF8String :msg->pwd().c_str()];
            NSString *code = [NSString stringWithUTF8String :msg->vality().c_str()];
            NSString *mail = [NSString stringWithUTF8String :msg->m_email().c_str()];
           [LoginHelper resetMailPwdReq:pwd valitycode:code email:mail];
        }
            break;
        case MSG_GET_RESET_MAIL_PWD_VALITY_REQ:
        {
            auto msg = static_cast<MsgGetResetMailValityReq*>(pProtoMsg);
            NSString *mail = [NSString stringWithUTF8String :msg->mail().c_str()];
            [LoginHelper getResetMailPwdValityReq:mail];
        }
            break;
        case MSG_GET_SWITCH_MAIL_VALITY_REQ:
        {
            auto msg = static_cast<MsgGetSwitchMailValityReq*>(pProtoMsg);
            NSString *mail = [NSString stringWithUTF8String :msg->mail().c_str()];
            [LoginHelper getSwitchMailAccountValityReq:mail];
        }
            break;
        case MSG_LOC_CHAT_TRAN_REQ:
        {
            auto msg = static_cast<MsgChatTranReq*>(pProtoMsg);
            NSString *text = [NSString stringWithUTF8String :msg->text().c_str()];
            NSString *lan = [NSString stringWithUTF8String :msg->targetlan().c_str()];
            [LoginHelper chatTranReq:text lanType:lan index:msg->id()];
        }
            break;
        case MSG_OPEN_SCORE_REQ:
        {
            [LoginHelper openScoreReq];
        }
            break;
        case MSG_LOCAL_PUSH_REQ:
        {
            auto msg = static_cast<MsgLocalPushReq*>(pProtoMsg);
            std::to_string(msg->id());
            scheduleLocalNotification(msg->remain_secs(), std::to_string(msg->id()), msg->text());
        }
            break;
    case MSG_LOC_PLATFORM_AUTO_LOGIN:
        {
            [[IGGLogin sharedInstance] start];
        }
        break;
    case MSG_LOC_PLATFORM_REQUEST_LOGIN:
        {
            //[LoginHelper setSwitchingAccount];
            auto msg = static_cast<MsgLocPlatformRequestLogin*>(pProtoMsg);
            if(msg->m_switchaccount() == TRUE)
            {
                [LoginHelper setSwitchingAccount];
            }
            else
            {
                [LoginHelper resetSwitchaccount];
            }
            switch (msg->eplatform()) {
                case PlatFormType::FACEBOOK:
                {
                    [LoginHelper loginWithFacebook: YES];
                }
                break;
                case PlatFormType::GOOGLE_PLAY:
                {
                    [LoginHelper loginWithGameCenter: YES];
                }
                break;
                case PlatFormType::EMAIL:
                {
                    [LoginHelper resetSwitchaccount];
                    NSString *mail = [NSString stringWithUTF8String :msg->m_email().c_str()];
                    NSString *passWord = [NSString stringWithUTF8String :msg->m_password().c_str()];
                    [LoginHelper loginWithEmail:mail pwd:passWord];
                }
                    break;
                case PlatFormType::GUEST:
                {
                    [LoginHelper loginGuest];
                }
                    break;
                default:
                    break;
            }
        }
        break;
        case MSG_LOC_BROWSER_OPEN_WITH_URL:
        {
            auto pMsgShowView = static_cast<MsgLocOpenUrl*>(pProtoMsg);
            if(pMsgShowView->has_m_webview() && pMsgShowView->m_webview())
            {
                iosWebView *pWebView= [[iosWebView alloc] initWithUrl:[NSString stringWithUTF8String:pMsgShowView->m_url().c_str()] andTitleImg:@"ic_title_bar.png" andBackImg:@"ic_btn_back.png"];
                [[EAGLView sharedEGLView] addSubview:pWebView];
            }
            else
            {
                NSString* url = [NSString stringWithUTF8String:pMsgShowView->m_url().c_str()];
                NSString *encodedString = [url stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
                NSURL *safariURL = [NSURL URLWithString:encodedString];
                [[UIApplication sharedApplication] openURL:safariURL];
            }
        }
        break;
    case MSG_LOC_PLATFORM_REQUEST_BIND:
        {
            auto msg = static_cast<MsgLocPlatformRequestBind*>(pProtoMsg);
            switch (msg->eplatform()) {
                case PlatFormType::FACEBOOK:
                {
                    [LoginHelper bindFacebook];
                }
                    break;
                case PlatFormType::GOOGLE_PLAY:
                {
                    [LoginHelper bindGameCenter];
                }
                break;
                case PlatFormType::EMAIL:
                {
                }
                    break;
                default:
                    break;
            }
        }
        break;
            
    case MSG_LOC_IGGSDK_SHOW_VIEW_EVENT:
    {
        auto msg = static_cast<MsgLocIggsdkShowViewEvent*>(pProtoMsg);
        switch (msg->m_viewtype()) {
            case ViewType::IGGCRM_CONTACT_US:
            {
               // [[IGGCRM sharedInstance] contactUsInScene:IGGCRMSceneNone parentViewController:appDelegate.window.rootViewController];
                NSString* url = [[IGGURLBundle bundle] serviceURL];
                if (url) {
                    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]];
                }
            }
                break;
                
            default:
                break;
        }
    }
        break;

    case MSG_LOC_WEGAMERS_SETPLAYERINFO:
    {
        auto msg = static_cast<MsgLocWegamersSetPlayerInfo*>(pProtoMsg);
        initWeGamers(msg->accountid(), msg->name());
        NSString *iggId = [NSString stringWithUTF8String:msg->accountid().c_str()];
        [[Crashlytics sharedInstance] setUserIdentifier: iggId];
        [[NSUserDefaults standardUserDefaults] setObject:iggId forKey:@"igg_id"];
    }
        break;
            
    case MSG_LOC_WEGAMERS_CHECKNEWNOTICEMESSAGE:
    {
        [GameCommunityEntry checkGameCommunityNotice:appDelegate.window completionBlock:^(NSError * _Nullable error){}];
    }
        break;
            
    case MSG_LOC_WEGAMERS_SETINCOMBAT:
    {
        auto msg = static_cast<MsgLocWegamersSetInComBat*>(pProtoMsg);
        [GameCommunityEntry setInComBat:msg->incombat()];
    }
        break;
            
    case MSG_LOC_WEGAMERS_STARTBROWSER:
    {
        openWeGamers();
    }
        break;
            
    case MSG_LOC_PAYMENT_REQUEST:
    {
        auto msg = static_cast<MsgLocPaymentRequest*>(pProtoMsg);
        int ret = [[Payment sharedInstance] purchaseItem:msg->m_itemid()];
        
//        using namespace com::igg::bzbee::app_sandbox::platform::network::MsgPB;
//        MsgLocPaymentResponse msgPaymentResponse;
//        msgPaymentResponse.set_m_paymentstate(static_cast<PAYMENT_STATE>(ret));
//        ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msgPaymentResponse.msgtype(), &msgPaymentResponse);
    }
        break;
    case MSG_LOC_REALNAME_STATE_QUERY:
    {
        auto msg = static_cast<MsgLocRealNameStateQuery*>(pProtoMsg);
        if (msg->m_openprocess()) {
            [[RealNameVerification sharedInstance] authenticate];
        }
        else
        {
            [[RealNameVerification sharedInstance] queryRealNameState];
        }
    }
        break;
    case MSG_LIVE_CHAT_REQ:
    {
        auto msg = static_cast<MsgLocLiveChatReq*>(pProtoMsg);
        if(msg->has_type())
        {
            NSString *url = nil;
            switch (msg->type()) {
                case MsgLocLiveChatReq_LiveChatType::MsgLocLiveChatReq_LiveChatType_LiveChat_Login:
                    url = [[IGGURLBundle bundle] livechatURL];
                    break;
                case MsgLocLiveChatReq_LiveChatType::MsgLocLiveChatReq_LiveChatType_LiveChat_Payment:
                    url = [[IGGURLBundle bundle] paymentLivechatURL];
                    break;
                case MsgLocLiveChatReq_LiveChatType::MsgLocLiveChatReq_LiveChatType_LiveChat_Forum:
                    url = [[IGGURLBundle bundle] forumURL];
                    break;
                case MsgLocLiveChatReq_LiveChatType::MsgLocLiveChatReq_LiveChatType_LiveChat_Service:
                    url = [[IGGURLBundle bundle] serviceURL];
                    break;
                default:
                    break;
            }
            if (url) {
                [[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]];
            }
            
        }
    }
            break;
    case MSG_LOC_AF_EVENT_REQ:
    {
        auto msg = static_cast<MsgLocAppsFlyerLog*>(pProtoMsg);
        auto eventid = msg->m_eventid();
        auto iggid = [LoginHelper sharedInstance].iggId;
        if (iggid == nil || iggid.length == 0)
        {
            [[AppsFlyerTracker sharedTracker] trackEvent:[NSString stringWithUTF8String:eventid.c_str()] withValues:@{@"userid" : @""}]; // userid 为 IGGID
        }
        else
        {
            auto eventstr = [NSString stringWithUTF8String:eventid.c_str()] ;
            
            [[AppsFlyerTracker sharedTracker] trackEvent:eventstr withValues:@{
                                                                                        @"g_id": [NSString stringWithUTF8String:Config::getInstance().getGameId().c_str()],
                                                                                        @"userid" :iggid}];
            //[[AppsFlyerTracker sharedTracker] trackEvent:[NSString stringWithUTF8String:eventid.c_str()] withValues:@{@"userid" : iggid}]; // userid 为 IGGID
        }
        
        //[FBSDKAppEvents logEvent:[NSString stringWithUTF8String:eventid.c_str()] parameters:{}];
    }
            break;
    case MSG_LOC_FB_EVENT_REQ:
    {
        auto msg = static_cast<MsgLocFaceBookLog*>(pProtoMsg);
        onLogFacebookEvent(msg);
    }
        break;
    case MSG_LOC_AGREEMENT_STATUS_REQ:
    {
        [LoginHelper reqAgreementStatus];
    }
        break;
    case MSG_LOC_AGREEMENT_LIST_REQ:
    {
         [LoginHelper reqAgreementList];
    }
        break;
    case MSG_LOC_AGREE_AGREEMENT:
    {
        [LoginHelper agreement];
    }
        break;
	case MSG_LOC_SHARE_REQ:
    {
		auto msg = static_cast<MsgLocShareReq*>(pProtoMsg);
		auto platform = msg->plattpy();
		if(platform == MsgLocShareReq_Platform::MsgLocShareReq_Platform_Platform_Facebook)
		{
			NSString* strPath = [NSString stringWithUTF8String: msg->strurl().c_str()];
			NSString* caption = [NSString stringWithUTF8String: msg->caption().c_str()];
			NSString* title = [NSString stringWithUTF8String: msg->title().c_str()];
			NSString* content = [NSString stringWithUTF8String: msg->content().c_str()];
			[LoginHelper shareFacebookDialog:strPath caption:caption title:title content:content];
		}
        //[LoginHelper agreement];
    }
        break;
    case MSG_LOC_PAYMENT_INIT:
    {
        [[Payment sharedInstance] loadItems];
    }
        break;
    case MSG_LOC_SET_IOS_LIMIT_FPS:
    {
        auto msg = static_cast<MsgLocSetIOSLimitFPS*>(pProtoMsg);
        if(msg)
        {
            ViewController *viewController = (ViewController*)appDelegate.window.rootViewController;
            if (viewController && viewController.preferredFramesPerSecond != msg->fps()) {
                viewController.preferredFramesPerSecond = msg->fps();
            }
        }
    }
        break;
	case MSG_LOC_RESUME_SUB_REQ:
    {
        [[Payment sharedInstance] resumeSubscription];
    }
        break;
    default:
        break;
    }
    
    return true;
}

void SystemUtil_Impl_Ios::shake()
{
    if (@available(iOS 10.0, *)) {
        UIImpactFeedbackGenerator* generator = [[UIImpactFeedbackGenerator alloc] initWithStyle:UIImpactFeedbackStyleMedium];
        [generator prepare];
        [generator impactOccurred];
    }
}

bool SystemUtil_Impl_Ios::playVideo(const tstring &filePath , bool bLoop)
{
    AppDelegate* appDelegate = (AppDelegate*)[[UIApplication sharedApplication] delegate];
    ViewController *viewController = (ViewController*)appDelegate.window.rootViewController;
    return [viewController playVideo: [NSString stringWithUTF8String: filePath.c_str()] loop:bLoop];
}

void SystemUtil_Impl_Ios::seekTo(int msec) {
    AppDelegate* appDelegate = (AppDelegate*)[[UIApplication sharedApplication] delegate];
    ViewController *viewController = (ViewController*)appDelegate.window.rootViewController;
    if (viewController.mediaPlayer) {
        [viewController.mediaPlayer seekTo:msec];
    }
}

void SystemUtil_Impl_Ios::stop() {
    AppDelegate* appDelegate = (AppDelegate*)[[UIApplication sharedApplication] delegate];
    ViewController *viewController = (ViewController*)appDelegate.window.rootViewController;
    if (viewController.mediaPlayer) {
        [viewController.mediaPlayer stop];
    }
}

#if 0
bool SystemUtil_Impl_Ios::sendLocalMessage(IMsg* pMsg)
{
    if (!pMsg) { return false; }

    ISystemUtilEvent event(ISystemUtilEvent::SEND_MESSAGE, pMsg->getMsgId());
    invokeEvent(&event);

    AppController* appDelegate = (AppController*)[[UIApplication sharedApplication] delegate];

    switch (pMsg->getMsgId()) {
        case MSG_LOC_BILLING_REQUEST:
        {
            MsgLocChargeRequest* pMsgChargeRequest = static_cast<MsgLocChargeRequest*>(pMsg);
            if ([SKPaymentQueue canMakePayments]) {
                // Display a store to the user.
                appDelegate.pay.m_priceEn = pMsgChargeRequest->priceEn;
                appDelegate.pay.m_itemName = [NSString stringWithUTF8String:pMsgChargeRequest->strItemName.c_str()];
                appDelegate.pay.m_item_id = [NSString stringWithUTF8String:pMsgChargeRequest->strItemId.c_str()];
                [appDelegate.pay requestCharge: [NSString stringWithUTF8String:pMsgChargeRequest->strItemId.c_str()]];
            } else {
                // Warn the user that purchases are disabled.
                MsgLocChargeResponse* pMsgChargeResponse = new MsgLocChargeResponse;
                pMsgChargeResponse->eResult = MsgLocChargeResponse::RESULT_UNSUPPORT;
                pMsgChargeResponse->strError = "payments can not be processed.";

                IMsgMgr::getInstance().addDispatchMessage(pMsgChargeResponse, EMPTY_STRING, IMsgMgr::MSG_SOURCE_CLIENT);
            }
        }
            break;
            
        case MSG_LOC_ROLE_INFO:
        {
            MsgLocRoleInfo* pMsgRoleInfo = static_cast<MsgLocRoleInfo*>(pMsg);
            [appDelegate.pay setIdUser:[NSString stringWithUTF8String:pMsgRoleInfo->m_strIggId.c_str()]];
if (!m_bPushInitialized)
            {
                m_bPushInitialized = true;

                if ( [[UIDevice currentDevice].systemVersion floatValue] >= 8.0)
                {
                    if ([[UIApplication sharedApplication] respondsToSelector:@selector(registerUserNotificationSettings:)]) {
                        UIUserNotificationSettings *settings = [UIUserNotificationSettings settingsForTypes:UIUserNotificationTypeBadge|UIUserNotificationTypeAlert|UIUserNotificationTypeSound
                                                                                                 categories:nil];
                        [[UIApplication sharedApplication] registerUserNotificationSettings:settings];
                        [[UIApplication sharedApplication] registerForRemoteNotifications];
                    } else {
                        [[UIApplication sharedApplication] registerForRemoteNotificationTypes:
                         UIRemoteNotificationTypeBadge |
                         UIRemoteNotificationTypeAlert |
                         UIRemoteNotificationTypeSound];
                        
                    }
                }
                else
                {
                    [[UIApplication sharedApplication] registerForRemoteNotificationTypes:UIRemoteNotificationTypeBadge|UIRemoteNotificationTypeSound|UIRemoteNotificationTypeAlert];
                }
            }

			NSString* iggId =  [NSString stringWithUTF8String:pMsgRoleInfo->m_strIggId.c_str()];
            [Tapjoy setUserID:iggId];
            
			if (Config::getInstance().getCurrLanguage() == Config::LANG_JP)
			{
				bool hasMatSignup = Cache::getInstance().getBool(MAT_SIGNUP, false);
				if (!hasMatSignup)
				{
					[MobileAppTracker setUserId:iggId];
					[MobileAppTracker measureEventName:@"registration"];
					Cache::getInstance().setBool(MAT_SIGNUP, true);
				}
			}

			if (nil == [[NSUserDefaults standardUserDefaults] objectForKey:@"SIGN_UP_EVENTS_SENT"]) {
				//[[AppsFlyerTracker sharedTracker] trackEvent:@"SIGN_UP" withValues:@{@"userid" : iggId }];

				[[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithBool:YES] forKey:@"SIGN_UP_EVENTS_SENT"];
			}
            NSString *gameId = [LoginHelper getNewGameId];
			IGGCRMUserData *userdate = [IGGCRMUserData userDataWithIGGId:iggId gameId session:[NSString stringWithUTF8String:m_signed_key.c_str()]];
		        //IGGCRM相关配置
		        IGGCRMOptions *options = [IGGCRMOptions optionWithProfileType:IGGCRMProfileVIP plan:IGGCRMPlan2];
		        options.languageType = getIGGLang();
		        //IGGCRM初始化
		       [[IGGCRM sharedInstance] initWithOptions:options userData:userdate];
		}
		break;
            
        case MSG_LOC_SHOW_VIEW:
        {
            MsgLocShowView *pMsgShowView = static_cast<MsgLocShowView*>(pMsg);

            if (pMsgShowView->strUrl == HandlerMisc::FREECASH_TAPJOY_URL)
			{
				UIViewController* pController = appDelegate.viewController;
                [[TapjoyView alloc]showOfferwall:pController];
			}
            else if (pMsgShowView->strUrl == HandlerMisc::LOBI_URL)
			{
                NSString *const officialCommunityUrlString       = @"https://web.lobi.co/special/community-lp/deckheroes?backScheme=com.vsg.ios.deckheroesjp";
                NSString *const officialCommunityUrlSchemeString = @"lobi://game_community?gameId=deckheroes&backScheme=com.vsg.ios.deckheroesjp";
                
                // Lobi初回起動時使用粘贴板跳到游戏页
                UIPasteboard.generalPasteboard.string = @"{\"co.lobi.pasteboard.urlscheme\":\"lobi://game_community?gameId=deckheroes\"}";
                
                NSURL *officialCommunityUrl       = [NSURL URLWithString:officialCommunityUrlString];
                NSURL *officialCommunityUrlScheme = [NSURL URLWithString:officialCommunityUrlSchemeString];
                
                if ([[UIApplication sharedApplication] canOpenURL:officialCommunityUrlScheme]) {
                    [[UIApplication sharedApplication] openURL:officialCommunityUrlScheme];
                }
                else {
                    [[UIApplication sharedApplication] openURL:officialCommunityUrl];
                 }
            }
	    else if (pMsgShowView->strUrl == HandlerMisc::IGGCRM_LOGIN_URL)
            {
                
                tstring iggId = std::to_string(HandlerRole::getInstance().getInstanceId());
                NSString* iggIdNstr =  [NSString stringWithUTF8String:iggId.c_str()];

                NSString* GameId = [LoginHelper getNewGameId];
                IGGCRMUserData *userdate = [IGGCRMUserData userDataWithIGGId:iggIdNstr gameId:GameId session:[NSString stringWithUTF8String:m_signed_key.c_str()]];
                //IGGCRM相关配置
                IGGCRMOptions *options = [IGGCRMOptions optionWithProfileType:IGGCRMProfileVIP plan:IGGCRMPlan2];
                options.languageType = static_cast<SystemUtil_Impl_Ios&>(ISystemUtil::getInstance()).getIGGLang();
                //IGGCRM初始化
                [[IGGCRM sharedInstance] initWithOptions:options userData:userdate];
                [IGGCRM sharedInstance].proxy =  [[IGGCRMCompatDHProxy alloc] init];
                
                [[IGGCRM sharedInstance] contactUsInScene:IGGCRMSceneLoading parentViewController:appDelegate.viewController];
            }
	    else if (pMsgShowView->strUrl == HandlerMisc::IGGCRM_PURCHASE_URL)
            {
                
                tstring iggId = std::to_string(HandlerRole::getInstance().getInstanceId());
                NSString* iggIdNstr =  [NSString stringWithUTF8String:iggId.c_str()];

                NSString* GameId = [LoginHelper getNewGameId];
                IGGCRMUserData *userdate = [IGGCRMUserData userDataWithIGGId:iggIdNstr gameId:GameId session:[NSString stringWithUTF8String:m_signed_key.c_str()]];
                //IGGCRM相关配置
                IGGCRMOptions *options = [IGGCRMOptions optionWithProfileType:IGGCRMProfileVIP plan:IGGCRMPlan2];
                options.languageType = static_cast<SystemUtil_Impl_Ios&>(ISystemUtil::getInstance()).getIGGLang();
                //IGGCRM初始化
                [[IGGCRM sharedInstance] initWithOptions:options userData:userdate];
                [IGGCRM sharedInstance].proxy =  [[IGGCRMCompatDHProxy alloc] init];
                
                [[IGGCRM sharedInstance] contactUsInScene:IGGCRMScenePurchase parentViewController:appDelegate.viewController];
            }
            else if (pMsgShowView->strUrl == HandlerMisc::IGGCRM_URL)
            {
                
                tstring iggId = std::to_string(HandlerRole::getInstance().getInstanceId());
                NSString* iggIdNstr =  [NSString stringWithUTF8String:iggId.c_str()];

                NSString* GameId = [LoginHelper getNewGameId];
                IGGCRMUserData *userdate = [IGGCRMUserData userDataWithIGGId:iggIdNstr gameId:GameId session:[NSString stringWithUTF8String:m_signed_key.c_str()]];
                //IGGCRM相关配置
                IGGCRMOptions *options = [IGGCRMOptions optionWithProfileType:IGGCRMProfileVIP plan:IGGCRMPlan2];
                options.languageType = static_cast<SystemUtil_Impl_Ios&>(ISystemUtil::getInstance()).getIGGLang();
                //IGGCRM初始化
                [[IGGCRM sharedInstance] initWithOptions:options userData:userdate];
                [IGGCRM sharedInstance].proxy =  [[IGGCRMCompatDHProxy alloc] init];
                
                [[IGGCRM sharedInstance] presentsFAQsInViewController:appDelegate.viewController];
            }
			else
			{
                if (pMsgShowView->bOpenWithExternalApp)
                {
                    NSString* url = [NSString stringWithUTF8String:pMsgShowView->strUrl.c_str()];
                    NSString *encodedString = [url stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
                    NSURL *safariURL = [NSURL URLWithString:encodedString];
                    [[UIApplication sharedApplication] openURL:safariURL];
                }
                else
                {
                    iosWebView *pWebView= [[iosWebView alloc] initWithUrl:[NSString stringWithUTF8String:pMsgShowView->strUrl.c_str()] andTitleImg:@"ic_title_bar.png" andBackImg:@"ic_btn_back.png"];
                    [[EAGLView sharedEGLView] addSubview:pWebView];
                    [pWebView release];
                }
			}
        }
            break;

		case MSG_LOC_UPDATE_REQUEST:
		{
            MsgLocUpdateRequest *pMsgUpdateReq = static_cast<MsgLocUpdateRequest*>(pMsg);
            
            [appDelegate.viewController openView: [NSString stringWithUTF8String:pMsgUpdateReq->strUrl.c_str()]];
		}

			break;

		case MSG_LOC_SEND_EMAIL:
		{
            MsgSendEmail *pMsgSendEmail = static_cast<MsgSendEmail*>(pMsg);

            tstring strMailUrl = StringBuilder::format("#0?subject=#1&body=#2").add(pMsgSendEmail->strUrl.c_str()).add(pMsgSendEmail->strTitle.c_str()).add(pMsgSendEmail->strContent.c_str()).build();

            iosWebView *pWebView= [[iosWebView alloc] initWithUrl:[NSString stringWithUTF8String:strMailUrl.c_str()] andTitleImg:@"ic_title_bar.png" andBackImg:@"ic_btn_back.png"];
            [[EAGLView sharedEGLView] addSubview:pWebView];
            [pWebView release];
		}
			break;

		case MSG_LOC_ACTIVITY_VIEW:
		{
            MsgLocActivityView *pMsgActivityView = static_cast<MsgLocActivityView*>(pMsg);
            activityView *pWebView= [[activityView alloc] initWithUrl:[NSString stringWithUTF8String:pMsgActivityView->strUrl.c_str()]];

            [[EAGLView sharedEGLView] addSubview:pWebView];
            [pWebView release];
		}
			break;

        case MSG_LOC_NOTICE_VIEW:
        {
            MsgLocNoticeView *pMsgNoticeView = static_cast<MsgLocNoticeView*>(pMsg);

            noticeView* view = [noticeView initWithUrl:[NSString stringWithUTF8String:pMsgNoticeView->strContent.c_str()] andStrTitle:[NSString stringWithUTF8String:pMsgNoticeView->strTitle.c_str()]];
            [[EAGLView sharedEGLView] addSubview:view];
            [[EAGLView sharedEGLView] sendSubviewToBack:view];
        }
            break;

        case MSG_LOC_PLATFORM_SHARE_REQUEST:
        {
            auto pMsgShareRequest = static_cast<MsgLocPlatformShareRequest*>(pMsg);
            tstring strShare = pMsgShareRequest->m_name+"\n"+pMsgShareRequest->m_description+"\n"+pMsgShareRequest->m_caption;

            switch (pMsgShareRequest->m_ePlatform)
            {
                case LOGIN_PLATFORM_FACEBOOK:
                {
                    FBSDKShareLinkContent* content = [[FBSDKShareLinkContent alloc] init];
                    content.contentURL = [NSURL URLWithString:[NSString stringWithUTF8String:pMsgShareRequest->m_link.c_str()]];
                    //content.imageURL = [NSURL URLWithString:[NSString stringWithUTF8String:pMsgShareRequest->m_picture.c_str()]];
                    //content.contentDescription = [NSString stringWithUTF8String:pMsgShareRequest->m_description.c_str()];
                    //content.contentTitle = [NSString stringWithUTF8String:(pMsgShareRequest->m_name+" \n"+pMsgShareRequest->m_caption).c_str()];
                    [FBSDKShareDialog showFromViewController:appDelegate.viewController withContent:content delegate:appDelegate];
                }
                    break;

                case LOGIN_PLATFORM_TWITTER:
                {
                    [DCSocial postToTwitter: appDelegate.viewController text:[NSString stringWithUTF8String:strShare.c_str()] imageName: [NSString stringWithUTF8String: pMsgShareRequest->m_picture.c_str()] url: [NSString stringWithUTF8String:pMsgShareRequest->m_link.c_str()]];

                    MsgLocPlatformShareResponse* pMsgResponse = new MsgLocPlatformShareResponse;
                    pMsgResponse->m_ret = MsgLocPlatformShareResponse::RESULT_UNKNOWN;
                    IMsgMgr::getInstance().addDispatchMessage(pMsgResponse, EMPTY_STRING, IMsgMgr::MSG_SOURCE_CLIENT);
                }
                    break;

                case LOGIN_PLATFORM_LINE:
                {
                    [DCSocial postTextToLine: [NSString stringWithUTF8String:strShare.c_str()]];

                    MsgLocPlatformShareResponse* pMsgResponse = new MsgLocPlatformShareResponse;
                    pMsgResponse->m_ret = MsgLocPlatformShareResponse::RESULT_UNKNOWN;
                    IMsgMgr::getInstance().addDispatchMessage(pMsgResponse, EMPTY_STRING, IMsgMgr::MSG_SOURCE_CLIENT);
                }
                    break;

                default:
                    break;
            }

        }
            break;

		case MSG_LOC_PLATFORM_COPY:
		{
            auto pMsgCopy = static_cast<MsgLocPlatformCopy*>(pMsg);
			UIPasteboard* pasteboard = [UIPasteboard generalPasteboard];
			pasteboard.string = [NSString stringWithUTF8String:pMsgCopy->m_content.c_str()];
		}
			break;

		case MSG_LOC_EXIT_GAME:
		{
			exit(0);
		}
		break;

        case MSG_LOC_LINK_LIVE_SUPPORT:
        {
            auto pMsgLink = static_cast<MsgLocLinkLiveSupport*>(pMsg);

            //调用SDK接口
            [GameCustomSer openLiveChatService:[NSString stringWithUTF8String:pMsgLink->m_strParamJson.c_str()] CallBack:^(GameCustomStatus status) {
                //接口返回结果处理
                MsgLocLinkLiveSupportRet::ERROR_TYPE err =  MsgLocLinkLiveSupportRet::LINK_ERROR_NONE;

                switch (status) {
                    case GameCustomStatus_NotInstall:
                        err =  MsgLocLinkLiveSupportRet::LINK_NOT_INSTALL;
                        break;
                    case GameCustomStatus_NotLastVersion:
                        err =  MsgLocLinkLiveSupportRet::LINK_VERSION_LOWER;
                        break;
                    case GameCustomStatus_ErrorParameter:
                        err =  MsgLocLinkLiveSupportRet::PARAMS_ERROR_JSON;
                        break;

                    default:
                        break;
                }

                MsgLocLinkLiveSupportRet* pMsgRet = new MsgLocLinkLiveSupportRet;
                pMsgRet->m_nErrorCode = err;
                IMsgMgr::getInstance().addDispatchMessage(pMsgRet, EMPTY_STRING, IMsgMgr::MSG_SOURCE_CLIENT);
            }];
        }
            break;

        case MSG_LOC_ACTION:
        {
            auto pMsgAction = static_cast<MsgLocAction*>(pMsg);
            
            switch (pMsgAction->m_usAction) {
                case MsgLocAction::ACTION_LEVEL:
                {
                    [FBSDKAppEvents logEvent:FBSDKAppEventNameAchievedLevel parameters:@{FBSDKAppEventParameterNameLevel : [NSString stringWithUTF8String:std::to_string(pMsgAction->m_data1).c_str()]}];
                    if (Config::getInstance().getCurrLanguage() == Config::LANG_JP)
                    {
                        tstring iggId = std::to_string(HandlerRole::getInstance().getInstanceId());
                        [MobileAppTracker setUserId:[NSString stringWithUTF8String: iggId.c_str()]];
                        MATEvent *event = [MATEvent eventWithName:@"level_achieved"];
                        event.level = pMsgAction->m_data1;
                        [MobileAppTracker measureEvent:event];
                    }
                }
                    break;

                case MsgLocAction::ACTION_TUTORIAL_COMPLETE:
                {
					if (Config::getInstance().getCurrLanguage() == Config::LANG_JP)
					{
						tstring iggId = std::to_string(HandlerRole::getInstance().getInstanceId());
						[MobileAppTracker setUserId:[NSString stringWithUTF8String: iggId.c_str()]];
						[MobileAppTracker measureEventName:@"tutorial_complete"];
					}
                }
                    break;

                case MsgLocAction::ACTION_DAY2_RETENTION:
                {
					[[AppsFlyerTracker sharedTracker] trackEvent:@"DAY2_RETENTION" withValues:@{
						@"g_id": [NSString stringWithUTF8String:Config::getInstance().getGameId().c_str()],
							@"userid" : [NSString stringWithUTF8String:std::to_string(HandlerRole::getInstance().getInstanceId()).c_str()] }];
                }
                    break;
                    
                case MsgLocAction::ACTION_CASH_CONSUMPTION:
                {
                    [FBSDKAppEvents logEvent:FBSDKAppEventNameSpentCredits valueToSum:pMsgAction->m_data1];
                }
                    break;
                case MsgLocAction::ACTION_SIGNED_KEY:
		{
			m_signed_key = pMsgAction->m_data2;
		}
		break;
                case MsgLocAction::ACTION_ENTER_LOBBY:
                {
                    m_signed_key = pMsgAction->m_data2;
                    m_has_enter_lobby = true;
                    if (m_deep_link .find(getPackageName()+"://rewards")!=tstring::npos)
                    {
                        openDeepLinkReward();
                    }
		    [appDelegate initCRMNotifaction];
                }
                    break;
                case MsgLocAction::ACTION_GUILD_MAKE:
                    sendGuildData(pMsgAction,"guildmake", MsgLocAction::ACTION_GUILD_MAKE);
                    break;
                case MsgLocAction::ACTION_GUILD_IN:
                    sendGuildData(pMsgAction,"guildin", MsgLocAction::ACTION_GUILD_IN);
                    break;
                case MsgLocAction::ACTION_GUILD_QUIT:
                    sendGuildData(pMsgAction,"guildout", MsgLocAction::ACTION_GUILD_QUIT);
                    break;
                case MsgLocAction::ACTION_GUILD_FIRE:
                    sendGuildData(pMsgAction,"guildout", MsgLocAction::ACTION_GUILD_FIRE);
                    break;
            }
        }
            break;

        case MSG_LOC_CHECK_PACKAGE_INSTALLED_REQUEST:
        {
            auto pMsgRequest = static_cast<MsgLocCheckPackageInstalledRequest*>(pMsg);

            auto pMsgRet = new MsgLocCheckPackageInstalledResponse;
            pMsgRet->m_packageName = pMsgRequest->m_packageName;
            pMsgRet->m_ret = isPackageInstalled(pMsgRequest->m_packageName);

            IMsgMgr::getInstance().addDispatchMessage(pMsgRet, EMPTY_STRING, IMsgMgr::MSG_SOURCE_CLIENT);
        }
            break;

		case MSG_LOC_SENDING_IO_REQUEST:
		{
            auto pMsgRequest = static_cast<MsgLocSendingIoRequest*>(pMsg);
			onMsgLocSendingIoRequest(pMsgRequest);
		}
			break;
        case MSG_LOC_GAME_INVALID_ACCOUNT:
        {
            [LoginHelper invalidAccount];
        }
        break;

        default:
            break;
    }

    return true;
}

void SystemUtil_Impl_Ios::sendGuildData(const MsgLocAction* pMsgAction,tstring strEventName, MsgLocAction::ACTION_TYPE nType)
{
    if (Config::getInstance().getCurrLanguage() == Config::LANG_JP)
    {
        tstring iggId = std::to_string(pMsgAction->m_data1);;
        [MobileAppTracker setUserId:[NSString stringWithUTF8String: iggId.c_str()]];
        tstring guildName = pMsgAction->m_data2 ;
        tstring guildId = std::to_string(pMsgAction->m_data3);
        tstring strQuitType = "1";
        if (nType == MsgLocAction::ACTION_GUILD_FIRE)
        {
            strQuitType = "2";
        }
        
        MATEventItem *itemEvent = NULL;
        if (nType == MsgLocAction::ACTION_GUILD_FIRE || nType == MsgLocAction::ACTION_GUILD_QUIT)
        {
            itemEvent = [MATEventItem eventItemWithName:[NSString stringWithUTF8String: guildName.c_str()] attribute1:[NSString stringWithUTF8String:guildId.c_str()] attribute2:[NSString stringWithUTF8String:strQuitType.c_str()] attribute3:nil attribute4:nil attribute5:nil];
        }
        else
        {
            
            itemEvent = [MATEventItem eventItemWithName:[NSString stringWithUTF8String: guildName.c_str()] attribute1:[NSString stringWithUTF8String:guildId.c_str()] attribute2:nil attribute3:nil attribute4:nil attribute5:nil];
        }
        NSArray * eventItem = @[itemEvent];
        MATEvent *event =[MATEvent eventWithName:[NSString stringWithUTF8String:strEventName.c_str()]];
        event.eventItems = eventItem;
        [MobileAppTracker measureEvent:event];
    }
}
#endif

uint64 SystemUtil_Impl_Ios::getTotalPhysicsMemory()
{
    return 0;
}

uint64 SystemUtil_Impl_Ios::getFreePhysicsMemory()
{
    return 0;
}

uint64 SystemUtil_Impl_Ios::getProcessAvaMemory()
{
    return 0;
}

uint64 SystemUtil_Impl_Ios::getProcessMemoryHWM()
{
    return 0;
}

int SystemUtil_Impl_Ios::getNetworkType() const
{
	struct sockaddr_in zeroAddress;
	bzero(&zeroAddress, sizeof(zeroAddress));
	zeroAddress.sin_len = sizeof(zeroAddress);
	zeroAddress.sin_family = AF_INET;
	SCNetworkReachabilityRef defaultRouteReachability = SCNetworkReachabilityCreateWithAddress(NULL, (struct sockaddr *)&zeroAddress); //创建测试连接的引用：
	SCNetworkReachabilityFlags flags;
	SCNetworkReachabilityGetFlags(defaultRouteReachability, &flags);
	if ((flags & kSCNetworkReachabilityFlagsReachable) == 0) 
	{
		return 0;  
	}
 
	int retVal = 0;
 
	if ((flags & kSCNetworkReachabilityFlagsConnectionRequired) == 0)
	{
		retVal = ((int)SystemUtil::TYPE_WIFI);
	}
 
	if ((((flags & kSCNetworkReachabilityFlagsConnectionOnDemand ) != 0) ||   
       (flags & kSCNetworkReachabilityFlagsConnectionOnTraffic) != 0))
	{
		if ((flags & kSCNetworkReachabilityFlagsInterventionRequired) == 0) 
		{
			retVal = ((int)SystemUtil::TYPE_WIFI);
		}
	}
 
	if ((flags & kSCNetworkReachabilityFlagsIsWWAN) == kSCNetworkReachabilityFlagsIsWWAN)
	{
		if((flags & kSCNetworkReachabilityFlagsReachable) == kSCNetworkReachabilityFlagsReachable) {
			if ((flags & kSCNetworkReachabilityFlagsTransientConnection) == kSCNetworkReachabilityFlagsTransientConnection) {
				retVal = ((int)SystemUtil::TYPE_CELLULAR);
				if((flags & kSCNetworkReachabilityFlagsConnectionRequired) == kSCNetworkReachabilityFlagsConnectionRequired) {
					retVal = ((int)SystemUtil::TYPE_CELLULAR);
				}
			}
		}
	}
 
	return retVal;
 



   /* int iResult = -1;
    SCNetworkReachabilityFlags scFlags;
    SCNetworkReachabilityRef scAddr = SCNetworkReachabilityCreateWithName(NULL, "www.igg.com");
    bool bSuccess = SCNetworkReachabilityGetFlags(scAddr, &scFlags);
    
    if(scAddr)
    {
        CFRelease(scAddr);
    }
    if ( !bSuccess ) {
        return iResult;
    }
    bool bReachable = ((scFlags & kSCNetworkReachabilityFlagsReachable) != 0);
    bool bConnRequired = ((scFlags & kSCNetworkReachabilityFlagsConnectionRequired) != 0);
    bool bAvailable = (bReachable && !bConnRequired);
    if ( !bAvailable ) {
        iResult = -1;
    } else {
        if ((scFlags & kSCNetworkReachabilityFlagsIsWWAN) != 0) {
            iResult = ((int)ISystemUtil::TYPE_CELLULAR);
        } else {
            iResult = ((int)ISystemUtil::TYPE_WIFI);
        }
    }
    return iResult;*/
}

void SystemUtil_Impl_Ios::requestProductData(const tstring& pcId) const
{
#if 0
    AppController* appDelegate = (AppController*)[[UIApplication sharedApplication] delegate];

    if ([SKPaymentQueue canMakePayments]) {
        // Display a store to the user.
        [appDelegate.pay requestProductData: [NSString stringWithUTF8String:pcId.c_str()]];
    }
#endif
}

tstring SystemUtil_Impl_Ios::getProductName(const tstring& pcId) const
{
#if 0
    AppController* appDelegate = (AppController*)[[UIApplication sharedApplication] delegate];

    return [[appDelegate.pay getProductName: [NSString stringWithUTF8String:pcId.c_str()]] UTF8String];
#endif
    return {};
}

bool SystemUtil_Impl_Ios::isProductDataLoaded() const
{
#if 0
    AppController* appDelegate = (AppController*)[[UIApplication sharedApplication] delegate];

    return appDelegate.pay.m_pendingRequest <= 0;
#endif
    return false;
}

void SystemUtil_Impl_Ios::showInterstitialAd() const
{
#if 0
    AppController* appDelegate = (AppController*)[[UIApplication sharedApplication] delegate];
    [appDelegate showInterstitialAd];
#endif
}

void SystemUtil_Impl_Ios::onHandlersInitialized() const
{
#if 0
    AppController* appDelegate = (AppController*)[[UIApplication sharedApplication] delegate];
    [appDelegate loginGameCenter];
#endif
}

tstring SystemUtil_Impl_Ios::getSystemVersion() const
{
    return [[UIDevice currentDevice].systemVersion UTF8String];
}

bool SystemUtil_Impl_Ios::isPackageInstalled(const tstring& packageName) const
{
    return ([[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:[NSString stringWithUTF8String:packageName.c_str()]]]);
}

tstring SystemUtil_Impl_Ios::getBundleURLScheme() const
{
    NSArray* cfBundleURLTypes = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleURLTypes"];
    if (!cfBundleURLTypes) { return EMPTY_STRING; }
    NSDictionary* cfBundleURLTypes0 = [cfBundleURLTypes objectAtIndex:0];
    if (!cfBundleURLTypes0) { return EMPTY_STRING; }
    NSArray* cfBundleURLSchemes = [cfBundleURLTypes0 objectForKey:@"CFBundleURLSchemes"];
    if (!cfBundleURLSchemes) { return EMPTY_STRING; }
    return [[cfBundleURLSchemes objectAtIndex:0] UTF8String];
}

void SystemUtil_Impl_Ios::onFBShareSuccess() const
{
#if 0
    MsgLocPlatformShareResponse* pMsgResponse = new MsgLocPlatformShareResponse;
    pMsgResponse->m_ret = MsgLocPlatformShareResponse::RESULT_OK;
    IMsgMgr::getInstance().addDispatchMessage(pMsgResponse, EMPTY_STRING, IMsgMgr::MSG_SOURCE_CLIENT);
#endif
}

void SystemUtil_Impl_Ios::onFBShareCancel() const
{
#if 0
    MsgLocPlatformShareResponse* pMsgResponse = new MsgLocPlatformShareResponse;
    pMsgResponse->m_ret = MsgLocPlatformShareResponse::RESULT_CANCELLED;
    IMsgMgr::getInstance().addDispatchMessage(pMsgResponse, EMPTY_STRING, IMsgMgr::MSG_SOURCE_CLIENT);
#endif
}

void SystemUtil_Impl_Ios::onFBShareErr() const
{
#if 0
    MsgLocPlatformShareResponse* pMsgResponse = new MsgLocPlatformShareResponse;
    pMsgResponse->m_ret = MsgLocPlatformShareResponse::RESULT_FAILED;
    IMsgMgr::getInstance().addDispatchMessage(pMsgResponse, EMPTY_STRING, IMsgMgr::MSG_SOURCE_CLIENT);
#endif
}

#if 0
void SystemUtil_Impl_Ios::onMsgLocSendingIoRequest(const MsgLocSendingIoRequest* pMsg) const
{
    if (!pMsg) { return; }

    Json::Value root;
    root["offer_id"]          = Json::Value("2c763a59acdd41249a49a0b3eab2635b");
    root["publisher_id"]      = Json::Value("ccf06d7b699847a59043a30b4c3872dd");
    root["publisher_user_id"] = Json::Value(pMsg->m_iUserId);
    root["name"]              = Json::Value(pMsg->m_strName);
    root["gender"]            = Json::Value(pMsg->m_strGender);
    root["email"]             = Json::Value(pMsg->m_strEmail);
    root["ip"]                = Json::Value(getIpAddress());
    root["idfa"]              = Json::Value(getAId());

    const tstring SENDING_TEST_URL = "http://test.sending.io/users";
    const tstring SENDING_API_URL = "https://api.sending.io/users";

    NSString* URLString;
    if (pMsg->m_bTestMode)
    {
	URLString = [NSString stringWithUTF8String:SENDING_TEST_URL.c_str()];
    }
    else
    {
	URLString = [NSString stringWithUTF8String:SENDING_API_URL.c_str()];
    }

    NSURL *finalURL = [NSURL URLWithString:[URLString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
    NSMutableURLRequest *URLRequest = [NSMutableURLRequest requestWithURL:finalURL cachePolicy:NSURLRequestReloadIgnoringLocalCacheData timeoutInterval:10];
    NSString* post = [NSString stringWithUTF8String:root.toStyledString().c_str()];
    NSData* bodyData = [post dataUsingEncoding:NSUTF8StringEncoding allowLossyConversion:YES];
    [URLRequest setHTTPMethod:@"POST"];
    [URLRequest setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];
    [URLRequest setValue:[NSString stringWithFormat:@"%lu", (unsigned long)[bodyData length]] forHTTPHeaderField:@"Content-Length"];
    [URLRequest setHTTPBody:bodyData];
    NSOperationQueue *queue = [[NSOperationQueue alloc]init];  
    [NSURLConnection sendAsynchronousRequest:URLRequest
	queue:queue  
	    completionHandler:^(NSURLResponse *response, NSData *data, NSError *error){  
		bool isSuccess = true;
		tstring strResponse;
		if (error) {
		    isSuccess = false;
		    strResponse = [error.localizedDescription UTF8String];
		    NSLog(@"Httperror:%@%ld", error.localizedDescription,(long)error.code);  
		}else{
		    NSInteger responseCode = [(NSHTTPURLResponse *)response statusCode];  
		    NSString *responseString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];  

		    isSuccess = (responseCode == 200);
		    strResponse = [responseString UTF8String];

		    NSLog(@"HttpResponseCode:%ld", (long)responseCode);  
		    NSLog(@"HttpResponseBody %@",responseString);  
		}

		auto pMsgResponse = new MsgLocSendingIoResponse;
		pMsgResponse->m_eResult = (isSuccess ? MsgLocSendingIoResponse::RESULT_SUCCESS : MsgLocSendingIoResponse::RESULT_FAILED);
		pMsgResponse->m_strError = strResponse;
		IMsgMgr::getInstance().addDispatchMessage(pMsgResponse, EMPTY_STRING, IMsgMgr::MSG_SOURCE_CLIENT);
	    }];
}
#endif

tstring SystemUtil_Impl_Ios::getIpAddress() const
{
    NSString *address          = @"error";
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr  = NULL;
    // retrieve the current interfaces - returns 0 on success
    int success = getifaddrs(&interfaces);
    if (success == 0) {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != NULL) {
            if(temp_addr->ifa_addr->sa_family == AF_INET) {
                // Check if interface is en0 which is the wifi connection on the iPhone
                if([[NSString stringWithUTF8String:temp_addr->ifa_name] isEqualToString:@"en0"]) {
                    // Get NSString from C String
                    address = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr)];
                }
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    // Free memory
    freeifaddrs(interfaces);
    return [address UTF8String];
}

bool SystemUtil_Impl_Ios::initWeGamers(const tstring& accoundId, const tstring& name)
{
    WeGamersSDKParams *gcParam = [[WeGamersSDKParams alloc] init];
    gcParam.sdkId = @"1001000250";
    gcParam.sessionKey = @"Bp5pmDxkOg2qOy1p";
    gcParam.gameAccountId = [NSString stringWithUTF8String:accoundId.c_str()];
    gcParam.nickName = [NSString stringWithUTF8String:name.c_str()];
    if ([gcParam.nickName length] == 0) {
        gcParam.nickName = @"Guest";
    }
    gcParam.skinType = GameCommunityThemDark;
    [GameCommunityEntry initGameCommunity:gcParam showCommunityRed:^(BOOL bShow) {
		dispatch_async(dispatch_get_main_queue(), ^{
			using namespace com::igg::bzbee::app_sandbox::platform::network::MsgPB;
			MsgLocWegamersShowRedPoint msgShowRedPoint;
			msgShowRedPoint.set_ishas(bShow);
			ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msgShowRedPoint.msgtype(), &msgShowRedPoint);
		});
    } supportGameCommunity:^(BOOL bSupport) {
        dispatch_async(dispatch_get_main_queue(), ^{
            using namespace com::igg::bzbee::app_sandbox::platform::network::MsgPB;
            MsgLocWegamersShowEntrance msgShowEntrance;
            msgShowEntrance.set_isshow(bSupport);
            ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msgShowEntrance.msgtype(), &msgShowEntrance);
        });
    }];
    return true;
}

void SystemUtil_Impl_Ios::openWeGamers()
{
    AppDelegate* appDelegate = (AppDelegate*)[[UIApplication sharedApplication] delegate];
    GameCommunityEntryResult* result = [GameCommunityEntry openGameCommunityHomePageAndwillExitLive:^{
        appDelegate.WeGamersOpened = FALSE;
    }];
    if (result.error) {
        NSLog(@"Open WeGamers failed, %@.", [result.error localizedDescription]);
    }
    else
    {
        appDelegate.WeGamersOpened = TRUE;
    }
}

void SystemUtil_Impl_Ios::onLogFacebookEvent(com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgLocFaceBookLog* pProtoMsg)
{
    if (pProtoMsg == nil)
    {
        return;
    }
    if(pProtoMsg->m_eventid().compare("app_launch") == 0)
    {
        LogStandardEvent("fb_mobile_launch_source");
        return;
    }
    if(pProtoMsg->m_eventid().compare("completed_registration")== 0)
    {
        LogStandardEvent([FBSDKAppEventNameCompletedRegistration UTF8String]);
        return;
    }
    if(pProtoMsg->m_eventid().compare("completed_tutorial")== 0)
    {
        LogStandardEvent([FBSDKAppEventNameCompletedTutorial UTF8String]);
        return;
    }
    if(pProtoMsg->m_eventid().compare("rated")== 0)
    {
        LogStandardEvent([FBSDKAppEventNameRated UTF8String]);
        return;
    }
    if(pProtoMsg->m_eventid().compare("unlocked_acheivement")== 0)
    {
        LogStandardEvent([FBSDKAppEventNameUnlockedAchievement UTF8String]);
        return;
    }
    if(pProtoMsg->m_eventid().compare("unlocked_acheivement")== 0)
    {
        LogStandardEvent([FBSDKAppEventNameUnlockedAchievement UTF8String]);
        return;
    }
    if(pProtoMsg->m_eventid().compare("ad_click")== 0)
    {
        LogStandardEvent([FBSDKAppEventNameAdClick UTF8String]);
        return;
    }
    if(pProtoMsg->m_eventid().compare("ad_impression")== 0)
    {
        LogStandardEvent([FBSDKAppEventNameAdImpression UTF8String]);
        return;
    }
    if(pProtoMsg->m_eventid().compare("achieved_level")== 0)
    {
        AchievedLevelEvent(pProtoMsg);
        return;
    }
    if(pProtoMsg->m_eventid().compare("purchased")== 0)
    {
        PurchasedEvent(pProtoMsg);
        return;
    }
    if(pProtoMsg->m_eventid().compare("spent_credits")== 0)
    {
        SpentcreditsEvent(pProtoMsg);
        return;
    }
    CustomizeEvent(pProtoMsg);
}

void SystemUtil_Impl_Ios::LogStandardEvent(tstring strType)
{
    NSString* iggid = [LoginHelper sharedInstance].iggId;
    if (iggid != nil && iggid.length > 0)
    {
        NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:iggid,@"IggId",nil];
        auto nsType = [NSString stringWithUTF8String:strType.c_str()];
        [FBSDKAppEvents logEvent:nsType parameters:dic];
    }
}

void SystemUtil_Impl_Ios::AchievedLevelEvent(com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgLocFaceBookLog* pProtoMsg)
{
    auto eventid = pProtoMsg->m_eventid();
    if (pProtoMsg->has_parameter1() && pProtoMsg->parameter1().length() > 0) {
        auto parm1 = pProtoMsg->parameter1();
        NSString* iggid = [LoginHelper sharedInstance].iggId;
        if (iggid != nil && iggid.length > 0)
        {
            NSString* pam1 = [NSString stringWithUTF8String :parm1.c_str()];
            NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:iggid,@"IggId",pam1,@"parameter1", nil];
            [FBSDKAppEvents logEvent:FBSDKAppEventNameAchievedLevel parameters:dic];
        }
    }
}

void SystemUtil_Impl_Ios::PurchasedEvent(com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgLocFaceBookLog* pProtoMsg)
{
    auto eventid = pProtoMsg->m_eventid();
    auto parm1 = pProtoMsg->parameter1();
    auto parm2 = pProtoMsg->parameter2();
    NSString* pam1 = [NSString stringWithUTF8String :parm1.c_str()];
    NSString* pam2 = [NSString stringWithUTF8String :parm2.c_str()];
    NSString* iggid = [LoginHelper sharedInstance].iggId;
    if (iggid != nil && iggid.length > 0)
    {
        NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:iggid,@"IggId",nil];
        [FBSDKAppEvents logPurchase:[pam1 doubleValue] currency:pam2 parameters:dic];
    }
}

void SystemUtil_Impl_Ios::SpentcreditsEvent(com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgLocFaceBookLog* pProtoMsg)
{
    auto eventid = pProtoMsg->m_eventid();
    auto parm1 = pProtoMsg->parameter1();
    auto parm2 = pProtoMsg->parameter2();
    NSString* pam1 = [NSString stringWithUTF8String :parm1.c_str()];
    NSString* pam2 = [NSString stringWithUTF8String :parm2.c_str()];
    NSString* iggid = [LoginHelper sharedInstance].iggId;
    if (iggid != nil && iggid.length > 0)
    {
        NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:iggid,@"IggId",pam2,FBSDKAppEventParameterNameContentType, nil];
        [FBSDKAppEvents logEvent:FBSDKAppEventNameSpentCredits valueToSum:[pam1 doubleValue] parameters:dic];
    }
}

void SystemUtil_Impl_Ios::CustomizeEvent(com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgLocFaceBookLog* pProtoMsg)
{
    auto eventid = pProtoMsg->m_eventid();
    auto parm1 = pProtoMsg->parameter1();
    auto parm2 = pProtoMsg->parameter2();
    NSString* pam1 = [NSString stringWithUTF8String :parm1.c_str()];
    NSString* pam2 = [NSString stringWithUTF8String :parm2.c_str()];
    NSString* iggid = [LoginHelper sharedInstance].iggId;
    if (iggid != nil && iggid.length > 0)
    {
        NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:iggid,@"IggId",pam1,@"parameter1", pam2,@"parameter2",nil];
        // NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:iggid,@"IggId"];
        [FBSDKAppEvents logEvent:[NSString stringWithUTF8String:eventid.c_str()] parameters:dic];
    }
}

void SystemUtil_Impl_Ios::scheduleLocalNotification(int64 remain_secs, const tstring& id, const tstring& message) const
{
    UILocalNotification* noti = [[UILocalNotification alloc] init];
    noti.fireDate = [NSDate dateWithTimeIntervalSinceNow:remain_secs];
    noti.timeZone = [NSTimeZone defaultTimeZone];
    noti.alertBody = [NSString stringWithUTF8String:message.c_str()];
    noti.soundName = UILocalNotificationDefaultSoundName;
    NSDictionary* infoDic = [NSDictionary dictionaryWithObject:[NSString stringWithUTF8String:id.c_str()] forKey:@"id"];
    noti.userInfo = infoDic;
    [[UIApplication sharedApplication] scheduleLocalNotification: noti];
}

void SystemUtil_Impl_Ios::unscheduleLocalNotification(const tstring& id) const
{
    NSArray* notifications = [[UIApplication sharedApplication] scheduledLocalNotifications];
    if (!notifications || notifications.count <= 0) { return; }
    for (UILocalNotification* notify in notifications)
    {
        if ([[notify.userInfo objectForKey:@"id"] isEqualToString:[NSString stringWithUTF8String:id.c_str()]])
        {
            [[UIApplication sharedApplication] cancelLocalNotification:notify];
            break;
        }
    }
}

void SystemUtil_Impl_Ios::openDeepLinkReward() const
{
#if 0
    tstring host = "snd";
    auto lang = Config::getInstance().getCurrLanguage();
    if (lang==Config::LANG_DE || lang==Config::LANG_FR || lang==Config::LANG_RU) {
        host = "nl";
    }
    const tstring DEEP_LINK_REWARDS = "http://event.#0.dh.igg.com/event/deep-linking/?gid=#1&signed_key=#2";
    auto url = StringBuilder::format(DEEP_LINK_REWARDS.c_str()).add(host).add(Config::getInstance().getGameId()).add(m_signed_key).build();

    iosWebView *pWebView= [[iosWebView alloc] initWithUrl:[NSString stringWithUTF8String:url.c_str()] andTitleImg:@"ic_title_bar.png" andBackImg:@"ic_btn_back.png"];
    [[EAGLView sharedEGLView] addSubview:pWebView];
    [pWebView release];
#endif
}

#if 0
IGGIDC SystemUtil_Impl_Ios::getCRMIDC()
{
    switch (Config::getInstance().getCurrLanguage())
    {
	case Config::LANG_EN_US:
	    return IGGIDCSND;

	case Config::LANG_FR:
	case Config::LANG_DE:
	case Config::LANG_RU:
	    return IGGIDCEU;

	case Config::LANG_KR:
	case Config::LANG_JP:
	case Config::LANG_FP_TW:
	    return IGGIDCTW;

	default:
	    return IGGIDCTW;
    }
}
#endif

tstring SystemUtil_Impl_Ios::getGameKey()
{
    switch (Config::getInstance().getCurrLanguage())
    {
        case Config::LANG_TRUNK: //cn
            return "446b08acf7110064ed8893b8f5a9e6ad";
            break;
        case Config::LANG_EN_US: //en
            return "9c4b6fa7f9897b5140221f04e279cbd1";
            break;
        case Config::LANG_ZH_TW: //tw
            return "9023db9ffc8ce0ce555ed44f62896806";
            break;
        case Config::LANG_RU: //ru
            return "6dcb5c7f550288f6af7ac97bbcfb37f9";
            break;
        case Config::LANG_VN: //vn
            return "2753097d8f1d4199d77c7575bd4676aa";
            break;
        case Config::LANG_TH: //th
            return "f3fbd77c736ab6d9caec0673945146ad";
            break;
        case Config::LANG_JP: //jp
            return "7c407d7904b87e662bb8dabfd4526fee";
            break;
        case Config::LANG_KR: //kr
            return "e333437615bc054de2bdaf66ce1eadde";
            break;
        case Config::LANG_DE: //de
            return "fadba7a4d440815abe0efee490b7935e";
            break;
        case Config::LANG_FR: //fr
            return "4ad9538a2ad45429ea09ea392273a2c8";
            break;
        case Config::LAN_ID: //id
            return "06ce88dfeae1495040e84e23be83bbda";
            break;
        case Config::LANG_PT: //PT
            return "4401add5869f905a1481094dbde1facd";
            break;
        case Config::LANG_TR: //TR
            return "b516f98f87ad421b5ff18fa24ba41095";
            break;
        case Config::LANG_IT: //IT
            return "7ac3cd63577531939ae148944b59e9c1";
            break;
        case Config::LANG_ES: //ES
            return "8c6b2d52db7472afb655852c747cce05";
            break;
        default:
            return "9c4b6fa7f9897b5140221f04e279cbd1";
    }
    return "9c4b6fa7f9897b5140221f04e279cbd1";
}

tstring SystemUtil_Impl_Ios::getGameId()
{
    switch (Config::getInstance().getCurrLanguage())
    {
        case Config::LANG_TRUNK: //cn
            return "1077190303";
            break;
        case Config::LANG_EN_US: //en
            return "1077010303";
            break;
        case Config::LANG_ZH_TW: //tw
            return "1077020303";
            break;
        case Config::LANG_RU: //ru
            return "1077070303";
            break;
        case Config::LANG_VN: //vn
            return "1077150303";
            break;
        case Config::LANG_TH: //th
            return "1077100303";
            break;
        case Config::LANG_JP: //jp
            return "1077080303";
            break;
        case Config::LANG_KR: //kr
            return "1077130303";
            break;
        case Config::LANG_DE: //de
            return "1077050303";
            break;
        case Config::LANG_FR: //fr
            return "1077060303";
            break;
        case Config::LAN_ID: //id
            return "1077110303";
            break;
        case Config::LANG_PT: //PT
            return "1077220303";
            break;
        case Config::LANG_TR: //TR
            return "1077160303";
            break;
        case Config::LANG_IT: //IT
            return "1077120303";
            break;
        case Config::LANG_ES: //ES
            return "1077090303";
            break;
        default:
            return "1077010303";
    }
    return "1077010303";
   // return @"1077010303";
}

#if 0
IGGFamily SystemUtil_Impl_Ios::getIGGFamily()
{
    switch (Config::getInstance().getCurrLanguage())
    {
//        case Config::LANG_EN_US:
//            return IGGFamilyIGG;
//        case Config::LANG_FR:
//        case Config::LANG_DE:
//        case Config::LANG_RU:
//            return IGGIDCEU;
            
//        case Config::LANG_KR:
 //       case Config::LANG_JP:
        case Config::LANG_FP_TW:
            return IGGFamilyFP;
        default:
            return IGGFamilyIGG;
    }
}

IGGCRMLanguageType SystemUtil_Impl_Ios::getIGGLang()
{
    switch (Config::getInstance().getCurrLanguage())
    {
        case Config::LANG_EN_US:
            return IGGCRMLanguageEN;
        case Config::LANG_FR:
            return IGGCRMLanguageFR;
        case Config::LANG_DE:
            return IGGCRMLanguageDE;
        case Config::LANG_RU:
            return IGGCRMLanguageRU;
        case Config::LANG_KR:
            return IGGCRMLanguageKR;
        case Config::LANG_JP:
            return IGGCRMLanguageJP;
        case Config::LANG_FP_TW:
            return IGGCRMLanguageTW;
        default:
            return IGGCRMLanguageEN;
    }
}
#endif

NS_END

