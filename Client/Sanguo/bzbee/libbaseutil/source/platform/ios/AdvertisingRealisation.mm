//
//  MobAdsView.m
//  Match3
//
//  Created by administrator on 2018/3/8.
//  Copyright © 2018年 administrator. All rights reserved.
//

#import "AdvertisingRealisation.h"
#import <GoogleMobileAds/GoogleMobileAds.h>
#import <GoogleMobileAds/GADRequest.h>
#import <GoogleMobileAds/GADBannerView.h>
#import <GoogleMobileAds/GADRewardBasedVideoAd.h>
#import <GoogleMobileAds/GADRequest.h>
#import "ViewController.h"
#import "AppDelegate.h"
#import <Foundation/Foundation.h>
#import <msg/clientMsgPb.pb.h>
#import <DispatchMsg.h>

AdvertisingRealisation *s_cIosHandlerMobAds = nullptr;

@implementation AdvertisingRealisation

using namespace com::igg::bzbee::app_sandbox::platform::network::MsgPB;

+ (AdvertisingRealisation*) getInstance
{
    if ( s_cIosHandlerMobAds == nullptr ) {
        s_cIosHandlerMobAds = [[AdvertisingRealisation alloc] init];
        s_cIosHandlerMobAds.bInitAds = false;
        s_cIosHandlerMobAds.bInitingAds = false;
    }
    return s_cIosHandlerMobAds;
}

GADRewardBasedVideoAd* rewardVideo = nil;

//down req
-(void) initMobAdsVideo
{
    if(_bInitingAds)
    {
        return;
    }
    if(!_bInitAds)
    {
        _bInitingAds = true;
        [[GADMobileAds sharedInstance] startWithCompletionHandler:^(GADInitializationStatus *_Nonnull status){
            _bInitingAds = false;
    
            _bInitAds = true;
            rewardVideo = [GADRewardBasedVideoAd sharedInstance];
            rewardVideo.delegate = self;
            GADRequest* request = [GADRequest request];
            [rewardVideo loadRequest:request withAdUnitID:@"ca-app-pub-9883228183528023/9420349120"];
        }];
    }else
    {
        rewardVideo = [GADRewardBasedVideoAd sharedInstance];
        rewardVideo.delegate = self;
        GADRequest* request = [GADRequest request];
        [rewardVideo loadRequest:request withAdUnitID:@"ca-app-pub-9883228183528023/9420349120"];
    }
    
    
}

/// Tells the delegate that the reward based video ad failed to load.
- (void)rewardBasedVideoAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd
    didFailToLoadWithError:(NSError *)error
{
    MsgWatchAdmobRes msg;
    msg.set_watchres(MsgWatchAdmobRes_watchEventType_loadFail);
    ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
}

/// Tells the delegate that a reward based video ad was received.
//download finish
- (void)rewardBasedVideoAdDidReceiveAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd
{
    if ([rewardVideo isReady])
    {
        MsgWatchAdmobRes msg;
        msg.set_watchres(MsgWatchAdmobRes_watchEventType_ready);
        ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
    }
}

//watch tv
- (void) showMobAdsVideo
{
    if ([rewardVideo isReady]) {
        AppDelegate* appDelegate = (AppDelegate*)[[UIApplication sharedApplication] delegate];
        ViewController *viewController = (ViewController*)appDelegate.window.rootViewController;
        [rewardVideo presentFromRootViewController:(UIViewController*)viewController];
        
        MsgWatchAdmobRes msg;
        msg.set_watchres(MsgWatchAdmobRes_watchEventType_begin_watch);
        ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
    }
}

/// Tells the delegate that the reward based video ad opened.
- (void)rewardBasedVideoAdDidOpen:(GADRewardBasedVideoAd *)rewardBasedVideoAd
{
    MsgLocForeBackgroundEvent switchMsg;
    switchMsg.set_eevent(BACK_GROUND_EVET);
    ClientMsgJavaToCpp::getInstance().dispatchOneMessage(switchMsg.msgtype(), &switchMsg);
}

/// Tells the delegate that the reward based video ad started playing.
- (void)rewardBasedVideoAdDidStartPlaying:(GADRewardBasedVideoAd *)rewardBasedVideoAd
{

}

/// Tells the delegate that the reward based video ad completed playing.
//finsih play
- (void)rewardBasedVideoAdDidCompletePlaying:(GADRewardBasedVideoAd *)rewardBasedVideoAd
{
    //[self.videoStatusDelegate mobAdsVideoStaus:4 type:1];
    MsgWatchAdmobRes msg;
    msg.set_watchres(MsgWatchAdmobRes_watchEventType_finish);
    ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
}

/// Tells the delegate that the reward based video ad closed.
- (void)rewardBasedVideoAdDidClose:(GADRewardBasedVideoAd *)rewardBasedVideoAd
{
    MsgWatchAdmobRes msg;
    msg.set_watchres(MsgWatchAdmobRes_watchEventType_closeView);
    ClientMsgJavaToCpp::getInstance().dispatchOneMessage(msg.msgtype(), &msg);
    MsgLocForeBackgroundEvent switchMsg;
    switchMsg.set_eevent(FORE_GROUND_EVET);
    ClientMsgJavaToCpp::getInstance().dispatchOneMessage(switchMsg.msgtype(), &switchMsg);
}

/// Tells the delegate that the reward based video ad will leave the application.
- (void)rewardBasedVideoAdWillLeaveApplication:(GADRewardBasedVideoAd *)rewardBasedVideoAd
{

}

@end

