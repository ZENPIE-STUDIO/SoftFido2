//
//  UserKernelShared.h
//  SoftFIDO2
//
//  Created by Eddie Hua on 2020/9/2.
//  Copyright © 2020 GoTrustID. All rights reserved.
//

#ifndef UserKernelShared_h
#define UserKernelShared_h


#define kSoftFidoDriverClassName "com_gotrustid_SoftFIDO2_SoftFido2Driver"
// User client method dispatch selectors.
enum {
    kSoftFidoUserClientSendFrame,
    kSoftFidoUserClientNotifyFrame,
    kNumberOfMethods // Must be last
};


// Eddie 參考 HID_ReportDesc_fido
// https://github.com/LedgerHQ/nanos-secure-sdk/blob/master/lib_stusb_samples/Class/HID%2BFIDO/usbd_hid_impl.c
unsigned char const kFido2HidReportDescriptor[] = {
    0x06, 0xD0, 0xF1, // Usage Page (Reserved 0xF1D0)
    0x09, 0x01,       // Usage (0x01)
    0xA1, 0x01,       // Collection (Application)
    // The Input report
    0x09, 0x03,       // Usage ID - Vendor defined
    0x15, 0x00,       //   Logical Minimum (0)
    0x26, 0xFF, 0x00, //   Logical Maximum (255)
    0x75, 0x08,       //   Report Size (8)
    0x95, 0x40,       //   Report Count (64)
    0x81, 0x08,       //   Input (Data,Var,Abs)
    // The Output report
    0x09, 0x04,       // Usage ID - Vendor defined
    0x15, 0x00,       //   Logical Minimum (0)
    0x26, 0xFF, 0x00, //   Logical Maximum (255)
    0x75, 0x08,       //   Report Size (8)
    0x95, 0x40,       //   Report Count (64)
    0x91, 0x08,       //   Output (Data,Var,Abs)
    0xC0,             // End Collection
};

// FIDO2 (Ming)
//const uint8_t kFido2HidReportDescriptor[] = {
//    0x06, 0xD0, 0xF1, // Usage Page (Reserved 0xF1D0)
//    0x09, 0x01,       // Usage (0x01)
//    0xA1, 0x01,       // Collection (Application)
//
//    //0x85,0x01,      // REPORT_ID (1) For FIDO, NO Report Id
//    0x15, 0x00,       //   Logical Minimum (0)
//    0x26, 0xFF, 0x00, //   Logical Maximum (255)
//
//    0x09, 0x01,       //   Usage (Vendor Usage 0x01)
//    0x75, 0x08,       //   Report Size (8)
//    0x96, 0x40,       //   Report Count
//    0x81, 0x00,       //   Input (Data,Var,Abs)
//
//    0x09, 0x01,       //   Usage (Vendor Usage 0x01)
//    0x75, 0x08,       //   Report Size (8)
//    0x96, 0x40,       //   Report Count (64)
//    0x91, 0x00,       //   Output (Data,Var,Abs)
//    0xC0,             // End Collection
//};

// 原本 SoftU2F 的
//uint8_t const u2fhid_report_descriptor[] = {
//    0x06, 0xD0, 0xF1, // Usage Page (FIDO Alliance : Reserved 0xF1D0)
//    0x09, 0x01,       // Usage (U2F HID Auth. Device : 0x01)
//    0xA1, 0x01,       // Collection (Application)
//    0x09, 0x20,       //   Usage (Input Report Data: 0x20)
//    0x15, 0x00,       //   Logical Minimum (0)
//    0x26, 0xFF, 0x00, //   Logical Maximum (255)
//    0x75, 0x08,       //   Report Size (8)
//    0x95, 0x40,       //   Report Count (64)
//    0x81, 0x02,       //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null
//                      //   Position)
//    0x09, 0x21,       //   Usage (Output Report Data: 0x21)
//    0x15, 0x00,       //   Logical Minimum (0)
//    0x26, 0xFF, 0x00, //   Logical Maximum (255)
//    0x75, 0x08,       //   Report Size (8)
//    0x95, 0x40,       //   Report Count (64)
//    0x91, 0x02,       //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null
//                      //   Position,Non-volatile)
//    0xC0,             // End Collection
//};

#endif /* UserKernelShared_h */
