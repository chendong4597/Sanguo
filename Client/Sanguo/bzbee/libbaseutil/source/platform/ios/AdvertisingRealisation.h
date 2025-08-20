//
//  MobAdsView.h
//  Match3
//
//  Created by administrator on 2018/3/8.
//  Copyright © 2018年 administrator. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GoogleMobileAds/GADRewardBasedVideoAdDelegate.h>


@interface AdvertisingRealisation : NSObject<GADRewardBasedVideoAdDelegate>
{
   
}

@property (nonatomic) BOOL bInitAds;
@property (nonatomic) BOOL bInitingAds;
- (void) initMobAdsVideo;
- (void) showMobAdsVideo;

/// Tells the delegate that the reward based video ad failed to load.
- (void)rewardBasedVideoAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd
    didFailToLoadWithError:(NSError *)error;

/// Tells the delegate that a reward based video ad was received.
- (void)rewardBasedVideoAdDidReceiveAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd;

/// Tells the delegate that the reward based video ad opened.
- (void)rewardBasedVideoAdDidOpen:(GADRewardBasedVideoAd *)rewardBasedVideoAd;

/// Tells the delegate that the reward based video ad started playing.
- (void)rewardBasedVideoAdDidStartPlaying:(GADRewardBasedVideoAd *)rewardBasedVideoAd;

/// Tells the delegate that the reward based video ad completed playing.
- (void)rewardBasedVideoAdDidCompletePlaying:(GADRewardBasedVideoAd *)rewardBasedVideoAd;

/// Tells the delegate that the reward based video ad closed.
- (void)rewardBasedVideoAdDidClose:(GADRewardBasedVideoAd *)rewardBasedVideoAd;

/// Tells the delegate that the reward based video ad will leave the application.
- (void)rewardBasedVideoAdWillLeaveApplication:(GADRewardBasedVideoAd *)rewardBasedVideoAd;

+ (AdvertisingRealisation*) getInstance;
@end
