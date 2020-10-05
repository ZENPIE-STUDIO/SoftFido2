//
//  U2FHID.h
//  MacLogon
//
//  Created by Eddie Hua on 2019/10/4.
//  Copyright © 2019 GoTrustID. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "softu2f.h"
#import "SoftU2FContants.h"

typedef bool (^HIDMessageHandler)(softu2f_hid_message* _Nonnull msg);

NS_ASSUME_NONNULL_BEGIN

@interface U2FHID : NSObject

//+ (instancetype) sharedInstance;

- (bool) isRuning;

- (bool) run;
- (bool) stop;
- (void) handleType:(CTAPHID_CMD) type Handler:(HIDMessageHandler) handler;
- (bool) sendMsg:(NSData*) data CID:(uint32_t) cid;
#pragma mark - FIDO2 -
- (bool) sendFido2Error:(CTAPHID_ERR_CODE) errCode CID:(uint32_t) cid;
- (bool) sendKeepAliveStatus:(uint8_t) status CID:(uint32_t) cid;
@end

NS_ASSUME_NONNULL_END
