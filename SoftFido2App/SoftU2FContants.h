//
//  APDU.h
//  MacLogon
//
//  Created by Eddie Hua on 2019/10/14.
//  Copyright © 2019 GoTrustID. All rights reserved.
//

#import <Foundation/Foundation.h>


extern const NSInteger U2F_CHAL_SIZE;
extern const NSInteger U2F_APPID_SIZE;

extern const NSInteger U2F_EC_KEY_SIZE;   // EC key size in bytes
extern const NSInteger U2F_EC_POINT_SIZE; // Size of EC point
extern const NSInteger MaxResponseSize;


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


typedef NS_ENUM(uint8_t, ApduCommandClass) {
    ApduCmdClassReserved = 0
};

typedef NS_ENUM(uint8_t, ApduCommandCode) {
    ApduCmdCodeRegister = 1,
    ApduCmdCodeAuthenticate = 2,
    ApduCmdCodeVersion = 3,
    //ApduCmdCodeCheckRegister = 4,
    //ApduCmdCodeAuthenticateBatch = 5
};

typedef NS_ENUM(uint8_t, ApduControl) {
    ApduCtrlEnforceUserPresenceAndSign = 0x03,
    ApduCtrlCheckOnly = 0x07,
    ApduCtrlInvalid = 0xFF
};

// ISO7816-4
typedef NS_ENUM(uint16_t, ResponseStatus) {
    RespStatusNoError = 0x9000,
    RespStatusReceiveTimeout = 0x6600,  // 自己加的 Error while receiving (timeout)
    RespStatusWrongData = 0x6A80,
    RespStatusConditionsNotSatisfied = 0x6985,
    RespStatusCommandNotAllowed = 0x6986,
    RespStatusInsNotSupported = 0x6D00,
    RespStatusWrongLength = 0x6700,
    RespStatusClassNotSupported = 0x6E00,
    RespStatusOtherError = 0x6F00 // "No precise diagnosis"
};

typedef NS_ENUM(uint8_t, CTAPHID_ERR_CODE) {
    CTAPHID_ERR_INVALID_CMD = 0x01,
    CTAPHID_ERR_INVALID_PAR = 0x02,
    CTAPHID_ERR_INVALID_LEN = 0x03,
    CTAPHID_ERR_INVALID_SEQ = 0x04,
    CTAPHID_ERR_MSG_TIMEOUT = 0x05,
    CTAPHID_ERR_CHANNEL_BUSY = 0x06,
    CTAPHID_ERR_LOCK_REQUIRED = 0x0A,
    CTAPHID_ERR_INVALID_CHANNEL = 0x0B,
    CTAPHID_ERR_OTHER = 0x7F
};
