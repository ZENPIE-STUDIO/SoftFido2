//
//  U2FHID.m
//  MacLogon
//
//  Created by Eddie Hua on 2019/10/4.
//  Copyright © 2019 GoTrustID. All rights reserved.
//

#import "U2FHID.h"
#import "softu2f.h"
#import "UserKernelShared.h"
//#import "Utils.h"

static U2FHID* gU2fHid = nil;

@interface U2FHID()
@property (nonatomic) softu2f_ctx* ctx;
@property (nonatomic) NSMutableDictionary<NSNumber*, HIDMessageHandler>* handlers;

@property (nonatomic) BOOL isUnexpectedStop;    // 用來判斷是不是非預期的Stop

@property (atomic) NSThread* runThread;
@end

@implementation U2FHID

- (void) dealloc {
    if (_ctx != NULL) {
        NSLog(@"U2FHID dealloc");
        softu2f_deinit(_ctx);
        _ctx = NULL;
    }
}

//- (void) initSoftU2fOutputLogFolder {
//    NSString* strPath = [[NSString alloc] initWithUTF8String:kSoftU2FLogOutputFolder];
//    if ([Utils createFolderIfNotExists:strPath]) {
//        LOGD(@"OK");
//    } else {
//        LOGD(@"FAILED!");
//    }
//}

- (instancetype) init {
    boolean_t useDriverKit = true;//false;
//    if (@available(macOS 10.16, *)) {
//        useDriverKit = true;
//    }
    boolean_t succ = false;
    if (self = [super init]) {
        // 利用 SOFTU2F_DEBUG flag，當作要輸出 Softu2f log file
//#ifdef OUTPUT_SOFTU2F_LOGFILE
//        [self initSoftU2fOutputLogFolder];
//#endif
        _isUnexpectedStop = YES;    // 突然離開就是非預期，使用者呼叫stop才會設為no
        _ctx = softu2f_init(SOFTU2F_DEBUG, useDriverKit);
        if (_ctx != NULL) {
            _handlers = [NSMutableDictionary new];
            _runThread = nil;
            gU2fHid = self;
            succ = true;
        } else {
            NSLog(@"U2FHID init (softu2f_init) failed!");
        }
    }
    if (!succ) {
        return nil;
    }
    return self;
}

- (void) handleType:(CTAPHID_CMD) type Handler:(HIDMessageHandler) handler {
    if (_ctx == NULL) {
        return;
    }
    NSLog(@"U2FHID handleType = 0x%02x", type);
    _handlers[@(type)] = handler;
    // register handler
    softu2f_hid_msg_handler_register(_ctx, type, my_message_handler);
}

bool my_message_handler(softu2f_ctx *ctx, softu2f_hid_message *req) {
    bool ret = false;
    if (req != NULL && gU2fHid != nil) {
        //req->cmd
        NSLog(@"U2FHID Received message (code %d) on channel(cid) %d.\n", req->cmd, req->cid);

        HIDMessageHandler handler = gU2fHid.handlers[@(req->cmd)];
        if (handler != nil) {
            ret = handler(req);
        }
    }
    return ret;
}

- (bool) isRuning {
    return (_runThread != nil);
}

- (bool) run {
    if (_runThread != nil) {
        NSLog(@"U2FHID run SKIP, U2FHID thread running...");
        return false;
    }
    NSLog(@"U2FHID run ...");
    U2FHID* __weak weakSelf = self;
    _runThread = [[NSThread alloc] initWithBlock:^{
        NSLog(@"U2FHID thread started");
        U2FHID* u2fhid = weakSelf;
        int ret = 0;
        if (u2fhid != nil) {
            if (u2fhid.ctx != nil) {
                ret = softu2f_run(u2fhid.ctx);
            }
            if (u2fhid.runThread != nil) {
                [u2fhid.runThread cancel];
                u2fhid.runThread = nil;
            }
        }
        NSLog(@"U2FHID thread stopped : %d", ret);
        [weakSelf.delegate didStopU2FHID:self Unexpected:weakSelf.isUnexpectedStop];
        gU2fHid = nil;
    }];
    // 2. 启动线程
    [_runThread start];
    return true;
}

- (bool) stop {
    bool ok = false;
    _isUnexpectedStop = NO;
    if (_runThread) {
        NSLog(@"U2FHID Stopping thread");
        softu2f_shutdown(_ctx);
        // 等一下
        for (int i = 0; i < 3 ; i++) {
            if (_runThread.isFinished) {
                ok = true;
                break;
            } else {
                [NSThread sleepForTimeInterval:1];
            }
        }
        _runThread = nil;
    }
    return ok;
}
// 因為 FIDO2 用不到 CTAPHID_MSG，所以我就直接替換
- (bool) sendMsg:(NSData*) data CID:(uint32_t) cid {
    softu2f_hid_message msg;
    //msg.cmd = CTAPHID_MSG;
    msg.cmd = CTAPHID_CBOR;  // FIDO2 (MI_Windows 也用這個)
    msg.bcnt = (uint16_t) data.length;
    msg.cid = cid;
    msg.data = (__bridge CFDataRef) data;
    return softu2f_hid_msg_send(_ctx, &msg);
}
//- (bool) sendCbor:(NSData*) cbor CID:(uint32_t) cid {
//    softu2f_hid_message msg;
//    msg.cmd = CTAPHID_CBOR;  // FIDO2 (MI_Windows 也用這個)
//    msg.bcnt = (uint16_t) data.length;
//    msg.cid = cid;
//    msg.data = (__bridge CFDataRef) data;
//    return softu2f_hid_msg_send(_ctx, &msg);
//}

- (bool) sendFidoError:(uint8_t) errCode CID:(uint32_t) cid {
    return softu2f_hid_err_send(_ctx, cid, errCode);
}
// 只有兩種 STATUS_PROCESSING (0x01), STATUS_UPNEEDED (0x02)
- (bool) sendKeepAliveStatus:(uint8_t) status CID:(uint32_t) cid {
    NSLog(@"U2FHID sendKeepAlive status = %x, cid = %u", status, cid);
    softu2f_hid_message msg;
    msg.cid = cid;
    msg.bcnt = 1;
    msg.cmd = CTAPHID_KEEPALIVE;
    msg.data = CFDataCreateWithBytesNoCopy(NULL, &status, 1, NULL);
    return softu2f_hid_msg_send(_ctx, &msg);
}
@end
