//
//  SoftFido2Lib.h
//  SoftFido2App
//
//  Created by Eddie Hua on 2020/9/4.
//  Copyright © 2020 GoTrustID. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef struct softu2f_ctx softu2f_ctx;
typedef struct softu2f_hid_message softu2f_hid_message;
// Handler function for HID message.
typedef bool (*softu2f_hid_message_handler)(softu2f_ctx *ctx, softu2f_hid_message *req);

// U2FHID message.
struct softu2f_hid_message {
    uint8_t cmd;
    uint16_t bcnt;
    uint32_t cid;
    CFDataRef data;
    CFMutableDataRef buf;
    uint8_t lastSeq;
    struct timeval start;
    softu2f_hid_message *next;
};


struct softu2f_ctx {
    io_connect_t con;
    uint32_t next_cid;
    pthread_mutex_t mutex;    // 1
    CFRunLoopRef run_loop;

    // Incomming messages.
    softu2f_hid_message *msg_list; // 2

    // Verbose logging.
    bool debug;

    // Handlers registered for HID msg types.
    softu2f_hid_message_handler ping_handler;
    softu2f_hid_message_handler msg_handler;
    softu2f_hid_message_handler init_handler;
    softu2f_hid_message_handler wink_handler;
    softu2f_hid_message_handler sync_handler;
    softu2f_hid_message_handler cbor_handler;   // FIDO2
};



NS_ASSUME_NONNULL_BEGIN

@interface SoftFido2Lib : NSObject
- (void) run;
@end

NS_ASSUME_NONNULL_END
