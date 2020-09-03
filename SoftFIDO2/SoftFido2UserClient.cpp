//
//  SoftFido2UserClient.cpp
//  SoftFIDO2
//
//  Created by Eddie Hua on 2020/9/2.
//  Copyright © 2020 GoTrustID. All rights reserved.
//

#include <os/log.h>

#include "UserKernelShared.h"
#include "SoftFido2UserClient.h"

#define LOG_PREFIX "[SoftFido2][UserClient] "

bool SoftFido2UserClient::init() {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "init");
    return true;
}

void SoftFido2UserClient::free() {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "free");
}


kern_return_t IMPL(SoftFido2UserClient, Start) {
    kern_return_t ret = kIOReturnSuccess;
    ret = Start(provider, SUPERDISPATCH);
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "Start");
    return ret;
}

kern_return_t IMPL(SoftFido2UserClient, Stop) {
    kern_return_t ret = kIOReturnSuccess;
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "Stop");
    return ret;
}

kern_return_t SoftFido2UserClient::ExternalMethod(uint64_t selector,
                                                  IOUserClientMethodArguments* arguments,
                                                  const IOUserClientMethodDispatch* dispatch,
                                                  OSObject* target,
                                                  void* reference) {
    kern_return_t ret = kIOReturnSuccess;
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod");
    return ret;
}
