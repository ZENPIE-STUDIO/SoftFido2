//
//  U2FHID.h
//  MacLogon
//
//  Created by Eddie Hua on 2019/10/4.
//  Copyright © 2019 GoTrustID. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "softu2f.h"

#define TYPE_INIT               0x80
#define TYPE_CONT               0x00
typedef NS_ENUM(NSInteger, CTAPHID_CMD) {
    CTAPHID_PING = (TYPE_INIT | 0x01),
    CTAPHID_MSG = (TYPE_INIT | 0x03),
    CTAPHID_LOCK = (TYPE_INIT | 0x04),
    CTAPHID_INIT = (TYPE_INIT | 0x06),
    CTAPHID_WINK = (TYPE_INIT | 0x08),
    CTAPHID_CBOR = (TYPE_INIT | 0x10),
    CTAPHID_CANCEL = (TYPE_INIT | 0x11),
    CTAPHID_ERROR = (TYPE_INIT | 0x3f),
    CTAPHID_KEEPALIVE = (TYPE_INIT | 0x3b)
};

typedef bool (^HIDMessageHandler)(softu2f_hid_message* _Nonnull msg);

@class U2FHID;


NS_ASSUME_NONNULL_BEGIN

@protocol U2fHidDelegate <NSObject>
- (void) didStopU2FHID:(U2FHID*) u2fhid Unexpected:(BOOL) isUnexpected;
@end


@interface U2FHID : NSObject

@property (nonatomic, weak) id<U2fHidDelegate>    delegate;
//+ (instancetype) sharedInstance;

- (bool) isRuning;

- (bool) run;
- (bool) stop;
- (void) handleType:(CTAPHID_CMD) type Handler:(HIDMessageHandler) handler;
- (bool) sendMsg:(NSData*) data CID:(uint32_t) cid;
- (bool) sendFidoError:(uint8_t) errCode CID:(uint32_t) cid;
- (bool) sendKeepAliveStatus:(uint8_t) status CID:(uint32_t) cid;
@end

NS_ASSUME_NONNULL_END
