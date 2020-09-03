//
//  SoftFIDO2.cpp
//  SoftFIDO2
//
//  Created by Eddie Hua on 2020/9/1.
//  Copyright © 2020 GoTrustID. All rights reserved.
//

#include <os/log.h>

#include <DriverKit/IOUserServer.h>
#include <DriverKit/IOLib.h>
#include <DriverKit/IODispatchQueue.h>
#include "UserKernelShared.h"
#include "SoftFido2Driver.h"

#define LOG_PREFIX "[SoftFido2][Driver] "

static const char* kDispatchQueueName = "SoftFidoQueue";

bool SoftFido2Driver::init() {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "init");
    return true;
}

void SoftFido2Driver::free() {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "free");
}


kern_return_t IMPL(SoftFido2Driver, Start) {
    kern_return_t ret = kIOReturnSuccess;
    ret = Start(provider, SUPERDISPATCH);
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "Start");
    IODispatchQueue* workQueue = nullptr;
    ret = IODispatchQueue::Create(kDispatchQueueName, 0 /*options*/ , 0 /*priority*/, &workQueue);
    if (ret != kIOReturnSuccess) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODispatchQueue::Create Failed!");
    }
    
//    workQueue->DispatchSync(^{
//
//    });
    return ret;
}

kern_return_t IMPL(SoftFido2Driver, Stop) {
    kern_return_t ret = kIOReturnSuccess;
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "Stop");
    return ret;
}

kern_return_t IMPL(SoftFido2Driver, NewUserClient) {
//kern_return_t SoftFIDO2::NewUserClient(uint32_t type, IOUserClient **userClient) {    // 這個沒辦法compile
    kern_return_t ret = kIOReturnSuccess;
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "NewUserClient");
    return ret;
}
