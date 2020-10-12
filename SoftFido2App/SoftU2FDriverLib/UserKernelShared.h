//
//  UserKernelShared.h
//  SoftU2F
//
//  Created by Benjamin P Toews on 1/12/17.
//

#ifndef UserKernelShared_h
#define UserKernelShared_h

#define kSoftU2FDriverClassName "SoftU2FDriver"     // KEXT
#define kSoftFidoDriverClassName "com_gotrustid_SoftFIDO2_SoftFido2Driver"  // DriverKit

// User client method dispatch selectors.
enum {
  kSoftU2FUserClientSendFrame,
  kSoftU2FUserClientNotifyFrame,
  kNumberOfMethods // Must be last
};

#endif /* UserKernelShared_h */
