//
//  DriverKitManager.h
//  MobileIron Authenticator
//
//  Created by Eddie Hua on 2020/10/6.
//  Copyright © 2020 GoTrustID. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface DriverKitManager : NSObject

+ (instancetype) shared;

- (void) activate;
- (void) deactivate;

@end

NS_ASSUME_NONNULL_END
