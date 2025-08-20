//
//  LoginHelper.h
//  CraftLegend
//
//  Created by Sandbox Mac on 2018/12/28.
//  Copyright © 2018 igg. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <msg/clientMsgPb.pb.h>

NS_ASSUME_NONNULL_BEGIN
@class IGGAppRating;

@interface LoginHelper : NSObject

@property (retain, nonatomic) NSString* iggId;
@property (retain, nonatomic) IGGAppRating* appRating;
+(instancetype)sharedInstance;

+(void)loginWithFacebook:(BOOL)forSwitch;
+(void)loginWithEmail:(NSString*)email pwd:(NSString*) param;
+ (void)bindFacebook;
+ (void)bindGameCenter;
+ (void)loginWithGameCenter:(BOOL)forSwitch;

+(void)sendLoginMail:(NSString*)pwd;
+(void)chatTranReq:(NSString*)text lanType:(NSString*)lan index:(int)value;

+(void)openScoreReq;

+(void)bindMail:(NSString*)mail;
+(void)getBindMailVality:(NSString *)mail;
+(void)usePwdAndValityBindMail:(NSString *)mail pwd:(NSString *)pwsd vality:(NSString *)value;
+(void)usePwdBindMail:(NSString *)pwd email:(NSString *)mail;
+(void)useMailSwitchAccount:(NSString *)mail;
+(void)useMailPwdLoginReq:(NSString *)pwd email:(NSString *)mail haveBindIggid:(BOOL)haveBind;
+(void)useMailPwdAndValityLoginReq:(NSString *)pwd email:(NSString *)mail Valitycode:(NSString *)value;
+(void)modifMailPwdReq:(NSString *)mail oldPwd:(NSString *)curPwd newPwd:(NSString *)pwd;
+(void)resetMailPwdReq:(NSString *)pwd valitycode:(NSString *)value email:(NSString *)mail;
+(void)getResetMailPwdValityReq:(NSString *)mail;
+(void)getSwitchMailAccountValityReq:(NSString *)mail;

+(void)reqAgreementStatus;
+(void)agreement;
+(void)reqAgreementList;

+(bool)isSwitchingAccount;
+(void)setSwitchingAccount;
+(void)resetSwitchaccount;
+(void)loaduserInfo;
+(int) getRigthState:(int)curState;
+(void)popLoginFail:(com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgLocPlatformResponseSession_LOGIN_RESULT)loginResult;
+(void)initSdk;
+(void)invalidAccount;
+(void)onUploadLogFile:(NSString*)filePath fileIndex:(int)index;
+(void)upLogfileByIggsdk:(NSString*)filePath fileIndex:(int)index;
+(void)sendUplogFileRet:(NSString*)filePath fileIndex:(int)index result:(BOOL)res;
+(void)openMailReq:(NSString*)recvAddress mailtitle:(NSString*)title mailCont:(NSString*)content;
+(void)loginGuest;

+(void)shareFacebookDialog:(NSString*)strPhotoPath caption : (NSString*)strCaption title : (NSString*)strTitle content : (NSString*)strContent;

+(NSString *)getNewGameId;
@end

NS_ASSUME_NONNULL_END
