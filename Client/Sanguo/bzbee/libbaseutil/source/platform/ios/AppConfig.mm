//
//  AppConfig.m
//  CraftLegend
//
//  Created by Sandbox Mac on 2019/4/3.
//  Copyright © 2019 igg. All rights reserved.
//

#import "AppConfig.h"
#import <IGGSDK/IGGAppConfigLoader.h>
#import <DispatchMsg.h>
#import <msg/clientMsgPb.pb.h>

@interface AppConfig() <IGGAppConfigLoaderDelegate>
@end

@implementation AppConfig

using namespace com::igg::bzbee::app_sandbox::platform::network::MsgPB;

+ (nonnull instancetype)sharedInstance { 
    static AppConfig *appConfig_ = nil;
    if (!appConfig_) {
        appConfig_ = [[self alloc] init];
    }
    
    return appConfig_;
}

- (void)loadConfig {
    IGGAppConfigLoader *loader = [[IGGAppConfigLoader alloc] initWithConfigName:@"server_config"];
    loader.delegate = self;
    [loader load];
}

- (NSDictionary*) getJsonObject: (NSString*)str
{
    if (!str) { return nil; }
    
    NSError *error = nil;
    id jsonObject = [NSJSONSerialization JSONObjectWithData:[str dataUsingEncoding:NSUTF8StringEncoding] options:NSJSONReadingAllowFragments error:&error];
    if (jsonObject && !error) {
        if ([jsonObject isKindOfClass:[NSDictionary class]]) {
            NSDictionary *deserializedDictionary = (NSDictionary*)jsonObject;
            return deserializedDictionary;
        }
    }

    return nil;
}

#pragma mark - IGGAppConfigLoaderDelegate
#import "SystemUtil_Impl_Ios.h"
- (void)onAppConfigLoader:(IGGAppConfigLoader *)loader loadingFinished:(IGGAppConfig *)config
{
    NSDictionary *jsonObject = [self getJsonObject:config.rawString];
    if (!jsonObject) { return; }
    NSDictionary *jsonObjectLoginBox = jsonObject[@"LoginBox"];
    if (!jsonObjectLoginBox) { return; }

    self.verified_ = [jsonObjectLoginBox[@"verified"] boolValue];
    self.addictedTips_ = [jsonObjectLoginBox[@"addictedTips"] boolValue];
    self.purchaseLimit_ = (NSNumber*)jsonObjectLoginBox[@"purchaseLimit"];
    self.purchaseLimitNormal_ = (NSNumber*)jsonObjectLoginBox[@"purchaseLimitNormal"];
    
    MsgLocServreConfig msg;
    msg.set_m_serverconfig_json([config.rawString UTF8String]);
    NSString *gameId = static_cast<bzbee::SystemUtil_Impl_Ios&>(bzbee::SystemUtil::getInstance()).getGameId();
    msg.set_m_gameid([gameId UTF8String]);
	long sertm = [[NSNumber numberWithInt:config.serverTimestamp] longValue];
	msg.set_m_servertm(sertm);
    ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
}

- (void)onAppConfigLoader:(IGGAppConfigLoader *)loader loadingFinishedWithFinalError:(IGGError *)error backup:(IGGAppConfigBackup *)backup
{
    
}

- (void)onAppConfigLoader:(IGGAppConfigLoader *)loader loadingPhaseFailed:(IGGAppConfigLoadingPhase *)phase
{
    
}

@end
