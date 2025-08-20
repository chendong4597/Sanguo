//
//  LoginHelper.m
//  CraftLegend
//
//  Created by Sandbox Mac on 2018/12/28.
//  Copyright © 2018 igg. All rights reserved.
//

#import "LoginHelper.h"

#import <NetworkModule.h>
#import <DispatchMsg.h>
#import <IGGSDK/IGGSDK.h>
#import <IGGSDK/IGGAccountManagementGuideline.h>
#import <IGGSDK/IGGThirdPartyAccountLoginScene.h>
#import <IGGSDK/IGGAgreementSigning.h>
#import <IGGSDK/IGGAgreementSigningFile.h>
#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKLoginKit/FBSDKLoginKit.h>
#import <IGGSDK/IGGAppRating.h>
#import "AdvertisingRealisation.h"
#import "SystemUtil_Impl_Ios.h"
#import <IGGSDK/IGGIncident.h>
#import "IGGCRMMgr.h"
#import "Payment.h"
#import "AppConfig.h"
#import "FriendAdd.h"
#import <AppsFlyerLib/AppsFlyerTracker.h>
#import <IGGSDK/IGGAdTracker.h>
#import <FBSDKShareKit/FBSDKShareKit.h>

namespace {
    bool switchingAccount{};
}

@implementation LoginHelper

#pragma mark - Facebook binding & logging in

 using namespace com::igg::bzbee::app_sandbox::platform::network::MsgPB;

+ (nonnull instancetype)sharedInstance {
    static LoginHelper *loginHelp_ = nil;
    if (!loginHelp_) {
        loginHelp_ = [[self alloc] init];
    }
    
    return loginHelp_;
}

+ (void)loginWithFacebook:(BOOL)forSwitch
{
    FBSDKLoginManager *login = [[FBSDKLoginManager alloc] init];
    login.loginBehavior = FBSDKLoginBehaviorBrowser;
    [login logOut];
    [login logInWithPermissions:@[@"public_profile"]
                 fromViewController:[self topMostController]
                            handler:^(FBSDKLoginManagerLoginResult *loginResult, NSError *error) {
                                if (error) {
                                    IGGLog(@"Process error");
                                    NSLog(@"%@", error.localizedDescription);
                                    [LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_FAILED];
//                                    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Get some error while requesting Facebook permission"
//                                                                                                             message:error.localizedDescription preferredStyle:UIAlertControllerStyleAlert];
//                                    [alertController addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDestructive handler:nil]];
//                                    [[self topMostController] presentViewController:alertController animated:YES completion:nil];
                                } else if (loginResult.isCancelled) {
                                    // 用户取消授权
                                    IGGLog(@"Cancelled");
                                    [LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_USER_CANCEL];
                                } else {
                                    IGGLog(@"Logged in");
                                    [self checkFacebookCandidate:loginResult.token.tokenString forSwitch:forSwitch];
                                    //[LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_OK];
                                }
                            }];
}

+ (void)checkFacebookCandidate:(NSString *)token forSwitch:(BOOL)forSwitch
{
    IGGFacebookAuthorizationProfile *profile = [IGGFacebookAuthorizationProfile new];
    profile.platform = IGGLoginTypeFacebook;
    profile.token = token;
    [IGGAccountManagementGuideline.sharedGuideline.loginScene.thirdPartyAccount checkCandidate:profile onComplete:^(IGGError *error, BOOL hasBound, NSString *IGGId) {
        if (error) {
            NSLog(@"%@", error.description);
            
//            UIAlertController *alertController = [UIAlertController alertControllerWithTitle: @"Get some error while requesting Facebook permission"
//                                                                                     message: error.description
//                                                                              preferredStyle: UIAlertControllerStyleAlert];
//            [alertController addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDestructive handler:nil]];
//            [[self topMostController] presentViewController:alertController animated:YES completion:nil];
            [LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_FAILED];
            return;
        }
        
        auto sure = _LC("CLI_GameCenter-Sure");
        NSString *nssure = [NSString stringWithUTF8String:sure.c_str()];
        auto cancel = _LC("CLI_GameCenter-cancel");
        NSString *nscancel = [NSString stringWithUTF8String:cancel.c_str()];
        auto thirdNewCreate = _LC("CLI_LoginScene_ThirdPartyAccount_Prompt_NewCreate");
        NSString *nsthirdNewCreate = [NSString stringWithUTF8String:thirdNewCreate.c_str()];
        
        if (hasBound) {
            
            if (forSwitch == NO) {
                [self loginWithThirdPartyProfile:profile];
                if ([LoginHelper isSwitchingAccount])
                {
                    [[AppConfig sharedInstance] loadConfig];
                }
                //[LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_OK];
                return;
            }
            if ([LoginHelper isSwitchingAccount] == true && [IGGId isEqualToString:[IGGSessionManager currentSession].IGGId] && [IGGSessionManager.currentSession.loginType isEqualToString:IGGLoginTypeFacebook]) {
//                UIAlertView *alert = [[UIAlertView alloc] initWithTitle:[IGGAccountManagementGuideline localStringByKey:@"LoginScene_ThirdPartyAccount_Prompt_AlreadyLogin"] message:nil delegate:nil cancelButtonTitle:[IGGAccountManagementGuideline localStringByKey:@"Common_Prompt_Action_Confirm"] otherButtonTitles:nil, nil];
//                [alert show];
                [LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_ALREADY_LOGIN];
                return;
            }
            auto content = _LC("CLI_GameCenter-Sure use iggid{0} enter game?");
            if(IGGId && IGGId.length > 0)
            {
                content = fmt::format(content,[IGGId UTF8String]);
            }
            else
            {
                content = fmt::format(content,"");
            }
            NSString *nscontent = [NSString stringWithUTF8String:content.c_str()];
            UIAlertController *alert = [UIAlertController alertControllerWithTitle:nil message:[NSString stringWithFormat:nscontent, IGGId] preferredStyle:UIAlertControllerStyleAlert];
            [alert addAction:[UIAlertAction actionWithTitle:nssure style:UIAlertActionStyleDefault handler:^(UIAlertAction *_Nonnull action) {
                [self loginWithThirdPartyProfile:profile];
                if ([LoginHelper isSwitchingAccount])
                {
                    [[AppConfig sharedInstance] loadConfig];
                }
                //[LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_OK];
            }]];
            [alert addAction:[UIAlertAction actionWithTitle:nscancel style:UIAlertActionStyleCancel handler:^(UIAlertAction *_Nonnull action) {
                [LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_USER_CANCEL];
            }]];
            [[self topMostController] presentViewController:alert animated:YES completion:nil];
            
        } else {
            UIAlertController *alert = [UIAlertController alertControllerWithTitle:nil message:nsthirdNewCreate preferredStyle:UIAlertControllerStyleAlert];
            [alert addAction:[UIAlertAction actionWithTitle:nssure style:UIAlertActionStyleDefault handler:^(UIAlertAction *_Nonnull action) {
                [self loginAndCreateWithThirdPartyProfile:profile];
                if ([LoginHelper isSwitchingAccount])
                {
                    [[AppConfig sharedInstance] loadConfig];
                }
                //[LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_OK];
            }]];
            [alert addAction:[UIAlertAction actionWithTitle:nscancel style:UIAlertActionStyleCancel handler:^(UIAlertAction *_Nonnull action) {
                [LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_USER_CANCEL];
            }]];
            [[self topMostController] presentViewController:alert animated:YES completion:nil];
        }
    }];
}

+ (void)bindFacebook
{
    auto bindfail = _LC("CLI_BindFacebook-bind fail");
    NSString *nsbindfail = [NSString stringWithUTF8String:bindfail.c_str()];
    auto sure = _LC("CLI_GameCenter-Sure");
    NSString *nssure = [NSString stringWithUTF8String:sure.c_str()];
    auto bidFaceNoPermission = _LC("CLI_BindFaceBook-Get some error while requesting Facebook permission");
    NSString *nsbidFaceNoPermission = [NSString stringWithUTF8String:bidFaceNoPermission.c_str()];
    auto retry = _LC("CLI_GameCenter-Please retry thank you!");
    NSString *nsretry = [NSString stringWithUTF8String:retry.c_str()];
    FBSDKLoginManager *login = [[FBSDKLoginManager alloc] init];
    login.loginBehavior = FBSDKLoginBehaviorBrowser;
    [login logInWithPermissions:@[@"public_profile"]
                 fromViewController:[self topMostController]
                            handler:^(FBSDKLoginManagerLoginResult *loginResult, NSError *error) {
                                if (error) {
                                    NSLog(@"%@", error.localizedDescription);
                                    
                                    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:nsbidFaceNoPermission
                                                                                                             message:nsretry
                                                                                                      preferredStyle:UIAlertControllerStyleAlert];
                                    [alertController addAction:[UIAlertAction actionWithTitle:nssure style:UIAlertActionStyleDestructive handler:nil]];
                                    [[self topMostController] presentViewController:alertController animated:YES completion:nil];
                                    
                                    using namespace com::igg::bzbee::app_sandbox::platform::network::MsgPB;
                                    MsgLocPlatformReponseBind msgBind;
                                    msgBind.set_m_eplatform(PlatFormType::FACEBOOK);
                                    msgBind.set_m_estate(BoundState::BOUND_FAILED);
                                    ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msgBind.msgtype(), &msgBind);
                                    return;
                                } else if (loginResult.isCancelled) {
                                    // 用户取消授权
                                    using namespace com::igg::bzbee::app_sandbox::platform::network::MsgPB;
                                    MsgLocPlatformReponseBind msgBind;
                                    msgBind.set_m_eplatform(PlatFormType::FACEBOOK);
                                    msgBind.set_m_estate(BoundState::BOUND_FAILED);
                                    ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msgBind.msgtype(), &msgBind);
                                } else {
                                    IGGFacebookAuthorizationProfile *profile = [IGGFacebookAuthorizationProfile new];
                                    profile.platform = IGGLoginTypeFacebook;
                                    profile.token = loginResult.token.tokenString;
                                    [[IGGAccountManagementGuideline sharedGuideline].bindScene.thirdPartyAccount bind:profile onComplete:^(IGGError *error, NSString *IGGId) {
                                        if (error) {
                                            NSLog(@"%@", error.description);
                                            bzbee::tstring iggid2;
                                            if ([IGGId isKindOfClass:[NSNumber class]])
                                            {
                                                iggid2 = std::to_string([IGGId longLongValue]);
                                            }
                                            else if ([IGGId isKindOfClass:[NSString class]])
                                            {
                                                const auto cstrIGGid = [IGGId UTF8String];
                                                if (cstrIGGid)
                                                {
                                                    iggid2 = cstrIGGid;
                                                }
                                            }
                                            
                                            auto havebind = fmt::format(_LC("CLI_BindGameCenter-bind fail,the third account have bind IGGID:{0}"),iggid2);
                                            NSString *nsHaveBind = [NSString stringWithUTF8String:havebind.c_str()];

                                            UIAlertController *alertController = [UIAlertController alertControllerWithTitle:nsHaveBind
                                                                                                                message: error.description
                                                                                                              preferredStyle: UIAlertControllerStyleAlert];
                                            [alertController addAction:[UIAlertAction actionWithTitle:nssure style:UIAlertActionStyleDestructive handler:nil]];
                                            [[self topMostController] presentViewController:alertController animated:YES completion:nil];
                                        }
                                        else
                                        {
                                            using namespace com::igg::bzbee::app_sandbox::platform::network::MsgPB;
                                            MsgLocPlatformReponseBind msgBind;
                                            msgBind.set_m_eplatform(PlatFormType::FACEBOOK);
                                            msgBind.set_m_estate(BoundState::BOUND_SUCCESS);
                                            ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msgBind.msgtype(), &msgBind);
                                        }
                                    }];
                                }
                            }];
}

#pragma mark- game center

+ (void)loginWithGameCenter:(BOOL)forSwitch
{
    [IGGAccountManagementGuideline.sharedGuideline.loginScene.gameCenter checkCandidate:^(UIViewController *viewController) {
        [[self topMostController] presentViewController:viewController animated:YES completion:nil];
    } onComplete:^(IGGError *error, BOOL hasBound, NSString *IGGId) {
        if (error) {
             [LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_FAILED];
            NSString *nshavebind = nullptr;
            if (error.underlyingError.code == GKErrorCancelled || IGGId.length == 0)
            {
                auto havebind = _LC("CLI_LoginGameCenter-go to setting view set");
                nshavebind = [NSString stringWithUTF8String:havebind.c_str()];
            }
            else
            {
                auto havebind = _LC("CLI_LoginGameCenter-login gameCenter fail");
                nshavebind = [NSString stringWithUTF8String:havebind.c_str()];
            }
            
            
            auto ok = _LC("CLI_BindGameCenter-ok");
            NSString *nsok = [NSString stringWithUTF8String:ok.c_str()];
            UIAlertController *alertController = [UIAlertController alertControllerWithTitle: nshavebind
                                                                                     message: error.description
                                                                              preferredStyle: UIAlertControllerStyleAlert];
            [alertController addAction:[UIAlertAction actionWithTitle:nsok style:UIAlertActionStyleDestructive handler:nil]];
            [[self topMostController] presentViewController:alertController animated:YES completion:nil];
//            NSLog(@"%@", error.description);
//
//            UIAlertController *alertController = [UIAlertController alertControllerWithTitle: @"Get some error while logging in GameCenter"
//                                                                                     message: error.description
//                                                                              preferredStyle: UIAlertControllerStyleAlert];
//            [alertController addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDestructive handler:nil]];
//            [[self topMostController] presentViewController:alertController animated:YES completion:nil];
            return;
        }
        auto sure = _LC("CLI_GameCenter-Sure");
        NSString *nssure = [NSString stringWithUTF8String:sure.c_str()];
        auto cancel = _LC("CLI_GameCenter-cancel");
        NSString *nscancel = [NSString stringWithUTF8String:cancel.c_str()];
        auto thirdNewCreate = _LC("CLI_LoginScene_ThirdPartyAccount_Prompt_NewCreate");
        NSString *nsthirdNewCreate = [NSString stringWithUTF8String:thirdNewCreate.c_str()];
        
        if (hasBound) {
            if (forSwitch == NO) {
                [IGGAccountManagementGuideline.sharedGuideline.loginScene.gameCenter login];
                if ([LoginHelper isSwitchingAccount])
                {
                    [[AppConfig sharedInstance] loadConfig];
                }
                return;
            }
            
            if ([LoginHelper isSwitchingAccount] == true && [IGGId isEqualToString:[IGGSessionManager currentSession].IGGId] && [IGGSessionManager.currentSession.loginType isEqualToString:IGGLoginTypeGameCenter]) {
//                UIAlertView *alert = [[UIAlertView alloc] initWithTitle:[IGGAccountManagementGuideline localStringByKey:@"LoginScene_ThirdPartyAccount_Prompt_AlreadyLogin"] message:nil delegate:nil cancelButtonTitle:[IGGAccountManagementGuideline localStringByKey:@"Common_Prompt_Action_Confirm"] otherButtonTitles:nil, nil];
//                [alert show];
                [LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_ALREADY_LOGIN];
                return;
            }
            auto content = _LC("CLI_GameCenter-Sure use iggid{0} enter game?");
            if(IGGId && IGGId.length > 0)
            {
                content = fmt::format(content,[IGGId UTF8String]);
            }
            else
            {
                content = fmt::format(content,"");
            }
            NSString *nscontent = [NSString stringWithUTF8String:content.c_str()];
            UIAlertController *alert = [UIAlertController alertControllerWithTitle:nil message:[NSString stringWithFormat:@"%@", nscontent] preferredStyle:UIAlertControllerStyleAlert];
            [alert addAction:[UIAlertAction actionWithTitle:nssure style:UIAlertActionStyleDefault handler:^(UIAlertAction *_Nonnull action) {
                [IGGAccountManagementGuideline.sharedGuideline.loginScene.gameCenter login];
                if ([LoginHelper isSwitchingAccount])
                {
                    [[AppConfig sharedInstance] loadConfig];
                }
                //[LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_OK];
            }]];
            [alert addAction:[UIAlertAction actionWithTitle:nscancel style:UIAlertActionStyleCancel handler:^(UIAlertAction *_Nonnull action) {
                [LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_USER_CANCEL];
            }]];
            [[self topMostController] presentViewController:alert animated:YES completion:nil];
        } else {
            UIAlertController *alert = [UIAlertController alertControllerWithTitle:nil message:nsthirdNewCreate preferredStyle:UIAlertControllerStyleAlert];
            [alert addAction:[UIAlertAction actionWithTitle:nssure style:UIAlertActionStyleDefault handler:^(UIAlertAction *_Nonnull action) {
                [IGGAccountManagementGuideline.sharedGuideline.loginScene.gameCenter createAndLogin];
                //[LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_USER_CANCEL];
                if ([LoginHelper isSwitchingAccount])
                {
                    [[AppConfig sharedInstance] loadConfig];
                }
            }]];
            [alert addAction:[UIAlertAction actionWithTitle:nscancel style:UIAlertActionStyleCancel handler:^(UIAlertAction *_Nonnull action) {
                [LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_USER_CANCEL];
            }]];
            [[self topMostController] presentViewController:alert animated:YES completion:nil];
        }
    }];
}

+ (void)bindGameCenter
{
    [IGGAccountManagementGuideline.sharedGuideline.bindScene.gameCenter bind:^(UIViewController *viewController) {
        [[self topMostController] presentViewController:viewController animated:YES completion:nil];
    } onComplete:^(IGGError *error, NSString *IGGId) {
        bzbee::tstring iggid2;
        if ([IGGId isKindOfClass:[NSNumber class]])
        {
            iggid2 = std::to_string([IGGId longLongValue]);
        }
        else if ([IGGId isKindOfClass:[NSString class]])
        {
            const auto cstrIGGid = [IGGId UTF8String];
            if (cstrIGGid)
            {
                iggid2 = cstrIGGid;
            }
        }
        if (error) {
            //* 已经绑定其他 IGGID：111403
            //* 当前 IGGID 已经绑定其他同类型第三方账号： 111404
            NSString *nshavebind;
            auto ok = _LC("CLI_BindGameCenter-ok");
            NSString *nsok = [NSString stringWithUTF8String:ok.c_str()];
            if ([error.uniqueCode isEqualToString:@"111403"]) {
                auto havebind = fmt::format(_LC("CLI_BindGameCenter-bind fail,have bind other IGGID:{0}"),iggid2);
                 nshavebind = [NSString stringWithUTF8String:havebind.c_str()];
            }
            else if([error.uniqueCode isEqualToString:@"111404"])
            {
                 auto havebind = fmt::format(_LC("CLI_BindGameCenter-bind fail,the third account have bind IGGID:{0}"),iggid2);
                nshavebind = [NSString stringWithUTF8String:havebind.c_str()];
            }
            else
            {
                if (error.underlyingError.code == GKErrorCancelled || IGGId.length == 0)
                {
                    auto havebind = _LC("CLI_BindGameCenter-go to setting view set");
                    nshavebind = [NSString stringWithUTF8String:havebind.c_str()];
                }
                else
                {
                    auto havebind = _LC("CLI_BindGameCenter-bind fail");
                    nshavebind = [NSString stringWithUTF8String:havebind.c_str()];
                }
            }
            NSLog(@"%@", error.description);
            
            if(nshavebind && nshavebind.length > 0)
            {
                UIAlertController *alertController = [UIAlertController alertControllerWithTitle: nshavebind
                                                                                         message: error.description
                                                                                  preferredStyle: UIAlertControllerStyleAlert];
                [alertController addAction:[UIAlertAction actionWithTitle:nsok style:UIAlertActionStyleDestructive handler:nil]];
                [[self topMostController] presentViewController:alertController animated:YES completion:nil];
                
                using namespace com::igg::bzbee::app_sandbox::platform::network::MsgPB;
                MsgLocPlatformReponseBind msgBind;
                msgBind.set_m_eplatform(PlatFormType::GOOGLE_PLAY);
                msgBind.set_m_estate(BoundState::BOUND_FAILED);
                msgBind.set_m_iggid(iggid2);
                ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msgBind.msgtype(), &msgBind);
            }
            else
            {
                using namespace com::igg::bzbee::app_sandbox::platform::network::MsgPB;
                MsgLocPlatformReponseBind msgBind;
                msgBind.set_m_eplatform(PlatFormType::GOOGLE_PLAY);
                msgBind.set_m_estate(BoundState::HAS_BEEN_BOUND);
                msgBind.set_m_iggid(iggid2);
                ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msgBind.msgtype(), &msgBind);
            }
        }
        else
        {
            using namespace com::igg::bzbee::app_sandbox::platform::network::MsgPB;
            MsgLocPlatformReponseBind msgBind;
            msgBind.set_m_eplatform(PlatFormType::GOOGLE_PLAY);
            msgBind.set_m_estate(BoundState::BOUND_SUCCESS);
            msgBind.set_m_iggid(iggid2);
            ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msgBind.msgtype(), &msgBind);
        }
    }];
}

//评分系统请求；
+(void)openScoreReq
{
    LoginHelper.sharedInstance.appRating = [IGGAppRating new];
    [LoginHelper.sharedInstance.appRating getDefaultDistributorAppStore:^(IGGDistributorAppStore *distributor, IGGError *error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (distributor)
            {
                [distributor openStoreWithConfirmation:^{
                    MsgOpenScoreRes msg;
                    msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::sccu);
                    ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
                }];
            }else {
                UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"失败" message:[NSString stringWithFormat:@"%@(%li)",error.uniqueCode,(long)error.underlyingError.code] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
                [alert show];
                MsgOpenScoreRes msg;
                msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::error);
                msg.set_errorcode((int)error.underlyingError.code);
                ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
            }
        });
    }];
}

//翻译聊天内容请求；
+(void)chatTranReq:(NSString*)text lanType:(NSString*)lan index:(int)value
{
    IGGTranslator *_translator = [[IGGTranslator alloc] initWithSourceLanguage:nil targetLanguage:lan];
    [_translator translateText:[[IGGTranslationSource alloc] initWithText:text] completed:^(IGGTranslationSet *translationSet) {
        NSString *ms =  [translationSet getByIndex:0].text;
        MsgChatTranRes msg;
        msg.set_id(value);
        const char *str = [ms UTF8String];
        msg.set_text(str);
        msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgChatTranRes_ChatTranRes::MsgChatTranRes_ChatTranRes_Sccu);
        NSString *srcLan = [translationSet getByIndex:0].sourceLanguage;
        msg.set_srclan([srcLan UTF8String]);
        ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
    } failed:^(id texts, NSError *error) {
        NSError *underlying = [error.userInfo objectForKey:NSUnderlyingErrorKey];
        MsgChatTranRes msg;
        msg.set_id(value);
        msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgChatTranRes_ChatTranRes(underlying.code));
        ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
    }];
}

//使用密码和验证码绑定邮箱请求；
+(void)usePwdAndValityBindMail:(NSString *)mail pwd:(NSString *)pwsd vality:(NSString *)value
{
     [[IGGAccountManagementGuideline sharedGuideline].bindScene.email bind:pwsd verificationCode:value onComplete:^(IGGError *error, NSString *mosaicMail)
    {
        MsgUsePwdAndValityBindMailRes msg;
        msg.set_errorcode((int)error.underlyingError.code);
        if (error) {
          //  [SVProgressHUD showErrorWithStatus:[NSString stringWithFormat:@"%@（%@）",[IGGAccountManagementGuideline localStringByKey:@"Common_Prompt_Failed"],error.description]];
            msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::error);
        }
        else
        {
            msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::sccu);
        }
        ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
    }];
}

//使用密码绑定邮箱；
+(void)usePwdBindMail:(NSString *)pwd email:(NSString *)mail
{
    [[IGGAccountManagementGuideline sharedGuideline].bindScene.email bind:pwd onComplete:^(IGGError *error, NSString *emailMosaic)
     {
         MsgUsePwdAndValityBindMailRes msg;
         msg.set_errorcode((int)error.underlyingError.code);
         if ([error.uniqueCode isEqualToString:@"111603"]) {
            //[SVProgressHUD showErrorWithStatus:[NSString stringWithFormat:@"%@(%@)",[IGGAccountManagementGuideline localStringByKey:@"BindingScene_Email_Prompt_PasswordIncorrect"], error.description]];
        }
        if (error)
        {
            msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::error);
            //[SVProgressHUD showErrorWithStatus:[NSString stringWithFormat:@"%@(%@)", [IGGAccountManagementGuideline localStringByKey:@"Common_Prompt_Failed"],error.description]];
        }
         else
         {
             msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::sccu);
         }
         ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
    }];
}

//使用邮箱切换账号；
+(void)useMailSwitchAccount:(NSString *)mail
{
    //需要检查用户输入 Email 是否已经在用户系统绑定
    [IGGAccountManagementGuideline.sharedGuideline.loginScene.email checkCandidate:mail onComplete:^(IGGError *error, IGGEmailState state, NSString *IGGId)
    {
        if (error) {
            //[SVProgressHUD showErrorWithStatus:[NSString stringWithFormat:@"%@(%@)",[IGGAccountManagementGuideline localStringByKey:@"Common_Prompt_Failed"],error.description]];
            MsgSwitchMailRes msg;
            msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::bindMailRes::OTHER);
            ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
            return;
        }
        MsgSwitchMailRes msg;
        msg.set_m_mail([mail UTF8String]);
        if(IGGId != nullptr)
        {
            msg.set_m_iggid([IGGId UTF8String]);
        }
        msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::bindMailRes(state));
        ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
    } ];
}

//邮箱使用密码+验证码登录请求；
+(void)useMailPwdAndValityLoginReq:(NSString *)pwd email:(NSString *)mail Valitycode:(NSString *)value
{
    [IGGAccountManagementGuideline.sharedGuideline.loginScene.email createAndLogin:pwd verificationCode:value onComplete:^(IGGError *error, IGGSession *session)
     {
        if (error) {
            //[SVProgressHUD showErrorWithStatus:error.description];
            MsgMailUsePwdAndValityLoginRes msg;
            msg.set_errorcode((int)error.underlyingError.code);
            msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::error);
            ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
            return;
        }
    }];
}

//修改邮箱密码；
+(void)modifMailPwdReq:(NSString *)mail oldPwd:(NSString *)curPwd newPwd:(NSString *)pwd
{
    [[IGGAccountManagementGuideline sharedGuideline].emailPasswordResetScene modify:curPwd password:pwd onComplete:^(IGGError *error)
    {
        MsgModifMailPwdRes msg;
        msg.set_errorcode((int)error.underlyingError.code);
        if (error)
        {
            msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::error);
        }
        else
        {
            msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::sccu);
        }
        ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);

        if ([error.uniqueCode isEqualToString:@"111803"]) {
            //[SVProgressHUD showErrorWithStatus:[IGGAccountManagementGuideline localStringByKey:@"ResetPasswordScene_Modify_Prompt_PasswordIncorrect"]];
        } else if ([error.uniqueCode isEqualToString:@"111804"]) {
           // [SVProgressHUD showErrorWithStatus:[IGGAccountManagementGuideline localStringByKey:@"ResetPasswordScene_Modify_Rule"]];
        } else if ([error.uniqueCode isEqualToString:@"111805"]) {
          //  [SVProgressHUD showErrorWithStatus:[IGGAccountManagementGuideline localStringByKey:@"ResetPasswordScene_Modify_Prompt_OldNewPasswordSame"]];
        } else if (error) {
           // [SVProgressHUD showInfoWithStatus:[NSString stringWithFormat:@"%@(%@)", [IGGAccountManagementGuideline localStringByKey:@"Common_Prompt_Failed"],error.description]];
        } else {
           // [SVProgressHUD showInfoWithStatus:[IGGAccountManagementGuideline localStringByKey:@"ResetPasswordScene_Modify_Prompt_Success"]];
        }
    }];
}

//重置邮箱密码请求；
+(void)resetMailPwdReq:(NSString *)pwd valitycode:(NSString *)value email:(NSString *)mail
{
    [IGGAccountManagementGuideline.sharedGuideline.emailPasswordResetScene reset:pwd verificationCode:value onComplete:^(IGGError *error)
    {
        MsgResetMailPwdRes msg;
        msg.set_errorcode((int)error.underlyingError.code);
        if (error) {
            //[SVProgressHUD showErrorWithStatus:[NSString stringWithFormat:@"%@（%@）",[IGGAccountManagementGuideline localStringByKey:@"Common_Prompt_Failed"],error.description]];
            msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::error);
        }
        else
        {
            msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::sccu);
        }
        ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
    }];
}

//使用邮箱密码登录请求；
+(void)useMailPwdLoginReq:(NSString *)pwd email:(NSString *)mail haveBindIggid:(BOOL)haveBind
{
    if (haveBind) //邮箱已经绑定当前游戏；
    {
            [IGGAccountManagementGuideline.sharedGuideline.loginScene.email login:pwd onComplete:^(IGGError *error, IGGSession *session)
             {
                 if (error)
                 {
                     MsgUsePwdLoginMailRes msg;
                     msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::error);
                     msg.set_errorcode((int)error.underlyingError.code);
                     ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
                     return;
                 }
                 
                 if ([error.uniqueCode isEqualToString:@"110403"])
                {
                    //[SVProgressHUD showErrorWithStatus:[NSString stringWithFormat:@"%@（%@）",[IGGAccountManagementGuideline localStringByKey:@"Common_Prompt_Failed"],error.description]];
                    return;
                }
                if (error) {
                    //[SVProgressHUD showErrorWithStatus:[NSString stringWithFormat:@"%@（%@）",[IGGAccountManagementGuideline localStringByKey:@"Common_Prompt_Failed"],error.description]];
                    return;
                } else {
                    return;
                }
            }];
    }
    else
    {
        [IGGAccountManagementGuideline.sharedGuideline.loginScene.email createAndLogin:pwd onComplete:^(IGGError *error, IGGSession *session)
         {
             if (error)
             {
                 MsgUsePwdLoginMailRes msg;
                 msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::error);
                 msg.set_errorcode((int)error.underlyingError.code);
                 ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
                 return;
             }
         }];
    }
}

//绑定邮箱请求；
+(void)bindMail:(NSString*)mail
{
    if (mail.length <= 0) {
        return;
    }
    [[IGGAccountManagementGuideline sharedGuideline].bindScene.email checkCandidate:mail onComplete:^(IGGError *error, IGGEmailState state, NSString *IGGId)
    {
        if (error)
        {
           // [SVProgressHUD showErrorWithStatus:[NSString stringWithFormat:@"%@（%@）",[IGGAccountManagementGuideline localStringByKey:@"Common_Prompt_Failed"],error.description]];
            MsgBindMailRes msg;
            msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::bindMailRes::OTHER);
            ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
            return;
        }
        if (state == UNAVAILABLE)
        {
            //[SVProgressHUD showInfoWithStatus:[NSString stringWithFormat:[IGGAccountManagementGuideline localStringByKey:@"BindingScene_Email_Prompt_AlreadyBind"], IGGId]];
        }
        else if (state == AVAILABLE)
        {
            //IGGBindEmailWithVeviricationCodeVC *vc = [[UIStoryboard storyboardWithName:@"Main" bundle:nil] instantiateViewControllerWithIdentifier:@"IGGBindEmailWithVeviricationCodeVC"];
        //   [LoginHelper getBindMailVality:mail];
        }
        else
        {
           // IGGBindEmailWithPasswordVC *vc = [[UIStoryboard storyboardWithName:@"Main" bundle:nil] instantiateViewControllerWithIdentifier:@"IGGBindEmailWithPasswordVC"];
        }
        MsgBindMailRes msg;
        msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::bindMailRes(state));
        msg.set_m_email([mail UTF8String]);
        msg.set_m_iggid([IGGId UTF8String]);
        ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
    }];
}

//获取绑定邮箱验证码请求；
+(void)getBindMailVality:(NSString *)mail
{
    [[IGGAccountManagementGuideline sharedGuideline].bindScene.email sendVerificationCode:^(IGGError *error, NSInteger countDown) {
        MsgGetBindMailVerificationRes msg;
        msg.set_lefttime((int)countDown);
        msg.set_errorcode((int)error.underlyingError.code);
        if (error) {
            msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::error);
        }
        else
        {
            msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::sccu);
        }
        ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
        if (error.underlyingError.code == 81211) {
            //self->_countDown = countDown;
            return ;
        }
    }];
}

//获取重置邮箱的验证码请求；
+(void)getResetMailPwdValityReq:(NSString *)mail
{
    [IGGAccountManagementGuideline.sharedGuideline.emailPasswordResetScene sendVerificationCodeForReset:^(IGGError *error, NSInteger countDown) {
        MsgGetResetMailValityRes msg;
        msg.set_errorcode((int)error.underlyingError.code);
        msg.set_time((int)countDown);
        if (error) {
            //[SVProgressHUD showErrorWithStatus:error.description];
            msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::error);
        }
        else
        {
            msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::sccu);
        }
        ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
    }];
}

//获取切换邮箱账号验证码请求；
+(void)getSwitchMailAccountValityReq:(NSString *)mail
{
    [IGGAccountManagementGuideline.sharedGuideline.loginScene.email sendVerificationCode:^(IGGError *error, NSInteger countDown)
     {
         MsgGetSwitchMailValityRes msg;
         msg.set_errorcode((int)error.underlyingError.code);
         msg.set_time((int)countDown);
         if (error) {
             //[SVProgressHUD showErrorWithStatus:error.description];
             msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::error);
         }
         else
         {
             msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::sccu);
         }
         ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
    }];
}

//邮箱登录请求；
+(void)sendLoginMail:(NSString*)pwd
{
    [IGGAccountManagementGuideline.sharedGuideline.loginScene.email login:pwd onComplete:^(IGGError *error, IGGSession *session) {
        if ([error.uniqueCode isEqualToString:@"110403"]) {
            return;
        }
        if (error) {
            return;
        }
        else
        {
            return;
        }
    }];
}

+(void)popLoginFail:(MsgLocPlatformResponseSession_LOGIN_RESULT)loginResult
{
    MsgLocPlatformResponseSession msg;
    msg.set_m_eresult(loginResult);
    ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
}

//mail login req
+(void)loginWithEmail:(NSString*)email pwd:(NSString*) param
{
    if (email.length <= 0) {
        [LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_EMAIL_EMPTY];
        return;
    }
    
    //需要检查用户输入 Email 是否已经在用户系统绑定
    [IGGAccountManagementGuideline.sharedGuideline.loginScene.email checkCandidate:email onComplete:^(IGGError *error, IGGEmailState state, NSString *IGGId) {
        
        if (error) {
            [LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_FAILED];
            return;
        }
        
        //此邮箱有绑定过游戏内IGGID，只要输入登录密码切换账号
        if (state == UNAVAILABLE) {
            if ([IGGAccountManagementGuideline.sharedGuideline.user.IGGID isEqualToString:IGGId] && [IGGAccountManagementGuideline.sharedGuideline.user.loginType isEqualToString:IGGLoginTypeEmail]) {
                [LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_ALREADY_LOGIN];
                return;
            }
            [LoginHelper sendLoginMail:param];
            return;
        }
        else
        {
            [LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_BIND_INVALID];
        }
    }];
    //[[IGGLogin sharedInstance] loginWithEmail:account password:param];
}

#pragma mark - login or register

//2.1 已存在使用登录接口
+ (void)loginWithThirdPartyProfile:(id<IGGThirdPartyAuthorizationProtocol>)profile
{
    [[IGGAccountManagementGuideline sharedGuideline].loginScene.thirdPartyAccount login:profile];
}

//2.2 不存在使用注册登录接口
+ (void)loginAndCreateWithThirdPartyProfile:(id<IGGThirdPartyAuthorizationProtocol>)profile
{
    [[IGGAccountManagementGuideline sharedGuideline].loginScene.thirdPartyAccount createAndLogin:profile];
}

+ (UIViewController *)topMostController
{
    UIViewController *topController = [UIApplication sharedApplication].keyWindow.rootViewController;
    while (topController.presentedViewController) {
        topController = topController.presentedViewController;
    }
    return topController;
}

+ (void)setSwitchingAccount { 
    switchingAccount = true;
}

+(void)resetSwitchaccount{
    switchingAccount = false;
}

+ (bool)isSwitchingAccount { 
    return switchingAccount;
}

+(int) getRigthState:(int)curState
{
    IGGRealNameVerificationState currentState = (IGGRealNameVerificationState)curState;
    IGGRealNameVerificationState stateArray[]{
        IGGRealNameVerificationUnknow,
        IGGRealNameVerificationUnauthorized,
        IGGRealNameVerificationSumbitted,
        IGGRealNameVerificationDenied,
        IGGRealNameVerificationAuthorized };
    using namespace com::igg::bzbee::app_sandbox::platform::network::MsgPB;
    REAL_NAME_STATE stateResponseArray[]{
        STATE_UNKNOW,
        STATE_UNAUTHORIZED,
        STATE_SUBMITTED,
        STATE_DENIED,
        STATE_AUTHORIZED
    };
    static_assert(std::extent<decltype(stateArray)>::value == std::extent<decltype(stateResponseArray)>::value, "Array size mismatch.");
    REAL_NAME_STATE stateResponse;
    for (std::size_t i{}; i < std::extent<decltype(stateArray)>::value; ++i) {
        if (currentState == stateArray[i])
        {
            stateResponse = stateResponseArray[i];
            break;
        }
    }
    return (int)stateResponse;
}

+(void)loaduserInfo
{
    [[IGGAccountManagementGuideline sharedGuideline] loadUser:^(IGGError *error, IGGUserProfile *user)
     {
         if (error) {
             //[SVProgressHUD showErrorWithStatus:error.description];
         }
         else
         {
             MsgAccountInfo msg;
             msg.set_result(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::errorMailRes::sccu);
             msg.set_logintype([user.loginType UTF8String]);
             msg.set_ismainlanduser((bool)user.isMainlandUser);
             REAL_NAME_STATE state =(REAL_NAME_STATE) [LoginHelper getRigthState:user.state];
             msg.set_state(state);
             //is bind mail
             bool isHaveBindMail = false;
             for(int i=0;i<user.bindingProfiles.count;++i)
             {
                // MsgCommonBindPlaformData* info = msg.add_bindarry();
                 IGGUserBindingProfile *profile = user.bindingProfiles[i];
                 if ([profile.type isEqualToString:@"mail"]) {
//                     MsgCommonBindPlaformData* info = msg.add_bindarry();
//                     info->set_m_eplatform(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::PlatFormType::EMAIL);
//                     info->set_m_bindstate((int)profile.hasBound);
//                     if (profile.displayName.length > 0) {
//                         info->set_m_account([profile.displayName UTF8String]);
//                     }
//                     isHaveBindMail = true;
                }
                else if ([profile.type isEqualToString:@"facebook"]) {
                     MsgCommonBindPlaformData* info = msg.add_bindarry();
                     info->set_m_eplatform(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::PlatFormType::FACEBOOK);
                     info->set_m_bindstate((int)profile.hasBound);
                    if (profile.displayName.length > 0) {
                        info->set_m_account([profile.displayName UTF8String]);
                    }
                }
                else if ([profile.type isEqualToString:@"applegamecenter"]) {
                    MsgCommonBindPlaformData* info = msg.add_bindarry();
                    info->set_m_eplatform(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::PlatFormType::GOOGLE_PLAY);
                    info->set_m_bindstate((int)profile.hasBound);
                    if (profile.displayName.length > 0) {
                        info->set_m_account([profile.displayName UTF8String]);
                    }
                }
                else if ([profile.type isEqualToString:@"guest"]) {
                    MsgCommonBindPlaformData* info = msg.add_bindarry();
                    info->set_m_eplatform(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::PlatFormType::GUEST);
                    info->set_m_bindstate((int)profile.hasBound);
                    if (profile.displayName.length > 0) {
                        info->set_m_account([profile.displayName UTF8String]);
                    }
                }
             }
//             if (isHaveBindMail == false) {
//                 MsgCommonBindPlaformData* info = msg.add_bindarry();
//                 info->set_m_eplatform(::com::igg::bzbee::app_sandbox::platform::network::MsgPB::PlatFormType::EMAIL);
//                 info->set_m_bindstate(0);
//             }
             ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
         }
         
         if (error.underlyingError.code == 901) {
             return ;
         }
         if (error && ![error.uniqueCode isEqualToString:@"210201"]) {
            // [SVProgressHUD showErrorWithStatus:[NSString stringWithFormat:@"%@（%@）", [IGGAccountManagementGuideline localStringByKey:@"Common_Prompt_Failed"],error.description]];
             return ;
         }
     }];
}

IGGAgreementSigning * _agreement;
+(void)reqAgreementStatus
{
    if (!_agreement) {
        _agreement = [[IGGAgreementSigning alloc] init];
    }
    
    [_agreement requestStatus:^(IGGError *error, IGGAgreementSigningStatus *signingStatus) {
        
        MsgAgreementStatusRes msg;
        auto status = MsgAgreementStatusRes_AgreementStatus_Signed;
        if (error) {
            // 重要，该业务非重要场景，如果请求失败可不予提示用户错误信息
            status = MsgAgreementStatusRes_AgreementStatus_Error;
        }
        else{
            if (signingStatus.value == IGGAgreementSigningStatusShouldRenewButPostponed) {
                // 重要，该业务非重要场景，如果请求失败可不予提示用户错误信息
                status = MsgAgreementStatusRes_AgreementStatus_Error;
            }else if (signingStatus.value == IGGAgreementSigningStatusSigned) {
                // 重要，该业务非重要场景，如果请求失败可不予提示用户错误信息
                status = MsgAgreementStatusRes_AgreementStatus_Signed;
            }else if (signingStatus.value == IGGAgreementSigningStatusShouldRenew) {
                // 重要，该业务非重要场景，如果请求失败可不予提示用户错误信息
                IGGAgreementSigningFile *file = [signingStatus prepareFileToBeSigned:nil];
                
                if(file)
                {
                    status = MsgAgreementStatusRes_AgreementStatus_Renew;
                    msg.set_tile([file.localizedTitle UTF8String]);
                    msg.set_content([file.localizedCaption UTF8String]);
                    msg.set_btntile([file.localizedAction UTF8String]);
                    
                    for (IGGAgreement *agreement in file.agreements) {
                        auto msgAgreement = msg.add_agreelist();
                        switch (agreement.type) {
                            case IGGAgreementTermsOfService:
                            {
                                msg.set_jumptile([agreement.typeName UTF8String]);
                                msgAgreement->set_type(MsgAgreement_AgreementType::MsgAgreement_AgreementType_TermsOfSerivice);
                            }
                                break;
                            case IGGAgreementPrivacyPolicy:
                                msgAgreement->set_type(MsgAgreement_AgreementType_PrivacyPolicy);
                                break;
                            case IGGAgreementTermsOfSubscription:
                                msgAgreement->set_type(MsgAgreement_AgreementType_TermsOfSubscription);
                                break;
                            default:
                                break;
                        }
                       
                        msgAgreement->set_localizedname([agreement.typeName UTF8String]);
                        msgAgreement->set_url([agreement.url UTF8String]);
                    }
                }
            }
        }
        msg.set_result(status);
        ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
    }];
}

+(void)agreement
{
    if (!_agreement) {
        _agreement = [[IGGAgreementSigning alloc] init];
    }
    
    [_agreement requestStatus:^(IGGError *error, IGGAgreementSigningStatus *signingStatus) {
        if (error) {
            // 重要，该业务非重要场景，如果请求失败可不予提示用户错误信息
            return;
        }
        if (signingStatus.value == IGGAgreementSigningStatusShouldRenewButPostponed) {
            // 重要，该业务非重要场景，如果请求失败可不予提示用户错误信息
            return;
        }
        if (signingStatus.value == IGGAgreementSigningStatusSigned) {
            // 重要，该业务非重要场景，如果请求失败可不予提示用户错误信息
           return;
        }
        if (signingStatus.value == IGGAgreementSigningStatusShouldRenew) {
            // 重要，该业务非重要场景，如果请求失败可不予提示用户错误信息
            IGGAgreementSigningFile *file = [signingStatus prepareFileToBeSigned:nil];
            if(file)
            {
                [_agreement sign:file.agreements onComplete:^(IGGError *error) {
                    if (error) {
                        // 重要，该业务非重要场景，如果请求失败可不予提示用户错误信息
                        return;
                    }
                }];
            }
        }
    }];
}

+(NSString*)getNewGameId
{
    bzbee::tstring gameId =  bzbee::SystemUtil::getInstance().getGameId();
    NSString* newGameId =  [NSString stringWithUTF8String:gameId.c_str()];
    return newGameId;
}

+(NSString*)getNewGameKey
{
    bzbee::tstring gameKey =  bzbee::SystemUtil::getInstance().getGameKey();
    NSString* newGameKey =  [NSString stringWithUTF8String:gameKey.c_str()];
    return newGameKey;
}

+(void)initSdk
{
    [AppsFlyerTracker sharedTracker].appsFlyerDevKey = @"WEYqZmRBi6ZmFww2esj28Y";
    [AppsFlyerTracker sharedTracker].appleAppID = @"1457950994";
    NSString* str =  [NSString stringWithFormat:@"{\"g_id\":\"%@\"}",[self getNewGameId]];
    [[AppsFlyerTracker sharedTracker] setCustomerUserID:str];
    [[IGGAdTracker sharedInstance] registerAppsFlyerUID:[AppsFlyerTracker sharedTracker].getAppsFlyerUID];
    [[AppsFlyerTracker sharedTracker] trackAppLaunch];
    
    // Init IGGSDK
    IGGEnv *env = [IGGEnv sharedInstance];
    env.family = IGGFamilyIGG;
    env.gameId = [self getNewGameId];
    env.secretKey = [self getNewGameKey];
    env.dataCenter = IGGIDCSND;
    env.regionalCenter = IGGIDCSND;
    [[IGGSDK sharedInstance] initialize];
    [[IGGCRMMgr sharedInstance] initIGGCRM];
    //[[Payment sharedInstance] loadItems];
    [[AppConfig sharedInstance] loadConfig];
    _agreement = nil;
}

+(void)onUploadLogFile:(NSString*)filePath fileIndex:(int)index
{
    if(filePath.length <= 0)
    {
        [LoginHelper sendUplogFileRet:filePath fileIndex:index result:FALSE];
        return;
    }
    [LoginHelper upLogfileByIggsdk:filePath fileIndex:index];
}

+(void)upLogfileByIggsdk:(NSString*)filePath fileIndex:(int)index
{
    // Upload
    auto& systemUtil = static_cast<bzbee::SystemUtil_Impl_Ios&>(bzbee::SystemUtil::getInstance());
    NSString *gameId = [LoginHelper getNewGameId];
    IGGIncident *incident = [IGGIncident incidentWithGameId: gameId devideId: [NSString stringWithUTF8String: systemUtil.getDeviceId().c_str()] deviceMode: [NSString stringWithUTF8String: systemUtil.getDeviceModel().c_str()]];
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setDateFormat: @"yyyy-MM-dd HH:MM:ss"];
    incident.devicePlatform = @"4"; // Ios
    incident.IGGId = [[NSUserDefaults standardUserDefaults] objectForKey:@"igg_id"];
    incident.gameVersion = [NSString stringWithUTF8String: systemUtil.getVersionName().c_str()];
    NSString *uuidStr = [NSString stringWithFormat:@"%d",index];
    incident.extraFields1 = [[NSDictionary alloc] initWithObjectsAndKeys:uuidStr,@"extraFields1",nil];
    
    //NSFileWrapper *wrapper = [[NSFileWrapper alloc] initRegularFileWithContents: [filePath dataUsingEncoding: NSUTF8StringEncoding]];
    NSFileWrapper *wrapper = [[NSFileWrapper alloc] initWithURL:[NSURL fileURLWithPath:filePath] options:NSFileWrapperReadingImmediate error:nil];
    incident.file = wrapper;
    
    IGGIncidentCollector *collector = [[IGGIncidentCollector alloc] init];
    [collector saveIncident:incident withCompleteHandler:^(IGGError *error) {
        if (error) {
            NSLog(@"Warning: Upload crash report failed: %@", error);
            [LoginHelper sendUplogFileRet:filePath fileIndex:index result:FALSE];
        } else {
            NSLog(@"Upload crash report succeed.");
            [LoginHelper sendUplogFileRet:filePath fileIndex:index result:TRUE];
        }
    }];
}

+(void)sendUplogFileRet:(NSString*)filePath fileIndex:(int)index result:(BOOL)res
{
    MsgLocUploadLogFileRes msg;
    msg.set_logfileindex(index);
    msg.set_logfilefullpath([filePath UTF8String]);
    if (res == TRUE) {
        msg.set_result(MsgLocUploadLogFileRes_Result_success);
    }
    else
    {
        msg.set_result(MsgLocUploadLogFileRes_Result_fail);
    }
    ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
}

+(void)openMailReq:(NSString*)recvAddress mailtitle:(NSString*)title mailCont:(NSString*)content
{
    NSMutableString *mailUrl = [[NSMutableString alloc] init];
    //添加收件人
    NSArray *toRecipients = [NSArray arrayWithObject: recvAddress];
    [mailUrl appendFormat:@"mailto:%@",[toRecipients componentsJoinedByString:@","]];
    
    //添加主题
    [mailUrl appendFormat:@"?subject=%@", title];
    
    //添加邮件内容
    [mailUrl appendFormat:@"&body=%@", content];
    
    NSString* email = [mailUrl stringByAddingPercentEscapesUsingEncoding: NSUTF8StringEncoding];
    [[UIApplication sharedApplication] openURL: [NSURL URLWithString:email]];
    
    MsgLocOpenEmailResponse msg;
    msg.set_m_result(MsgLocOpenEmailResponse_RESULT_OPEN_EMAIL_EMAIL_OPEN_SUCCESS);
    ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
}
    
+(void)invalidAccount
{
    [[IGGSessionManager currentSession] setExpiredAt:[NSDate date]];
}

+(void)reqAgreementList
{
    if (!_agreement) {
        _agreement = [[IGGAgreementSigning alloc] init];
    }
    
    [_agreement requestAssignedAgreements:^(IGGError *error, IGGAssignedAgreements *agreements) {
        MsgAgreementListRes msg;
        if (error) {
            // 重要，该业务非重要场景，如果请求失败可不予提示用户错误信息
        }
        else
        {
            NSArray *temp = [agreements orderInType:@[@(IGGAgreementPrivacyPolicy),@(IGGAgreementTermsOfService),@(IGGAgreementTermsOfSubscription)]];
            if (temp.count == 0) {
                
            } else {
                for (IGGAgreement *agreement in temp) {
                    
                    auto msgAgreement = msg.add_agreelist();
                    switch (agreement.type) {
                        case IGGAgreementTermsOfService:
                            msgAgreement->set_type(MsgAgreement_AgreementType_TermsOfSerivice);
                            break;
                        case IGGAgreementPrivacyPolicy:
                            msgAgreement->set_type(MsgAgreement_AgreementType_PrivacyPolicy);
                            break;
                        case IGGAgreementTermsOfSubscription:
                            msgAgreement->set_type(MsgAgreement_AgreementType_TermsOfSubscription);
                            break;
                        default:
                            break;
                    }
                    
                    msgAgreement->set_localizedname([agreement.typeName UTF8String]);
                    msgAgreement->set_url([agreement.url UTF8String]);
                }
            }
        }
        ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
    }];
}

+(void)loginGuest
{
    IGGGuestLoginScene *guestLoginScene = [IGGAccountManagementGuideline sharedGuideline].loginScene.guest;
    [guestLoginScene checkCandidate:^(IGGError *error, BOOL hasBound, NSString *IGGId) {
        if (error) {
            //提示玩家登录失败.
            [LoginHelper popLoginFail: MsgLocPlatformResponseSession_LOGIN_RESULT_RESULT_FAILED];
            return;
        }
        
        auto sure = _LC("CLI_GameCenter-Sure");
        NSString *nssure = [NSString stringWithUTF8String:sure.c_str()];
        auto cancel = _LC("CLI_GameCenter-cancel");
        NSString *nscancel = [NSString stringWithUTF8String:cancel.c_str()];

        if (hasBound) {
            if ([LoginHelper isSwitchingAccount] == false)
            {
                [guestLoginScene login];
                return;
            }
            
            //提示玩家是否已 IGGID 进入游戏，玩家点击确认后调用
            auto content = _LC("CLI_GameCenter-Sure use iggid{0} enter game?");
            if(IGGId && IGGId.length > 0)
            {
                content = fmt::format(content,[IGGId UTF8String]);
            }
            else
            {
                content = fmt::format(content,"");
            }
            NSString *nscontent = [NSString stringWithUTF8String:content.c_str()];
            UIAlertController *alert = [UIAlertController alertControllerWithTitle:nil message:[NSString stringWithFormat:nscontent, IGGId] preferredStyle:UIAlertControllerStyleAlert];
            [alert addAction:[UIAlertAction actionWithTitle:nssure style:UIAlertActionStyleDefault handler:^(UIAlertAction *_Nonnull action) {
                [guestLoginScene login];
                if ([LoginHelper isSwitchingAccount])
                {
                    [[AppConfig sharedInstance] loadConfig];
                }
            }]];
            [alert addAction:[UIAlertAction actionWithTitle:nscancel style:UIAlertActionStyleCancel handler:^(UIAlertAction *_Nonnull action) {
                
            }]];
            [[self topMostController] presentViewController:alert animated:YES completion:nil];
            //[guestLoginScene login];
            
        } else {
            //提示玩家是否已全新IGGID 进入游戏，玩家点击确认后调用
            [guestLoginScene createAndLogin];
            if ([LoginHelper isSwitchingAccount])
            {
                [[AppConfig sharedInstance] loadConfig];
            }
        }
    }];
}

+(void)shareFacebookDialog:(NSString*)strPhotoPath caption : (NSString*)strCaption title : (NSString*)strTitle content : (NSString*)strContent
{
	//if [FBSDKShareDialog ] 
    NSData *dataImage = [[NSFileManager defaultManager] contentsAtPath:strPhotoPath];
    UIImage *uiImage = [UIImage imageWithData:dataImage];

    FBSDKSharePhoto *fbPhoto = [[FBSDKSharePhoto alloc] init];
    fbPhoto.image = uiImage;
    fbPhoto.userGenerated = YES;
    fbPhoto.caption = strCaption;

    FBSDKSharePhotoContent *fbContent = [[FBSDKSharePhotoContent alloc] init];
    fbContent.photos = @[fbPhoto];
    
    
    //AppController* appDelegate = (AppController*)[[UIApplication sharedApplication] delegate];
    [FBSDKShareDialog showFromViewController:[self topMostController] withContent:fbContent delegate:nil];
}

@end
