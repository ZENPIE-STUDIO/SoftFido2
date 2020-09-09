//
//  SoftFIDO2.cpp
//  SoftFIDO2
//
//  Created by Eddie Hua on 2020/9/1.
//  Copyright © 2020 GoTrustID. All rights reserved.
//

#include <os/log.h>

#include <DriverKit/IOUserClient.h>
#include <DriverKit/IODispatchQueue.h>
#include "com_gotrustid_SoftFIDO2_SoftFido2Driver.h"

#define LOG_PREFIX "[SoftFido2][Driver] "

static const char* kDispatchQueueName = "SoftFidoQueue";

bool com_gotrustid_SoftFIDO2_SoftFido2Driver::init() {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "init");
    if (!super::init()) {
        return false;
    }
    return true;
}

void com_gotrustid_SoftFIDO2_SoftFido2Driver::free() {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "free");
    super::free();
}


kern_return_t IMPL(com_gotrustid_SoftFIDO2_SoftFido2Driver, Start) {
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
    RegisterService();
    return ret;
}

kern_return_t IMPL(com_gotrustid_SoftFIDO2_SoftFido2Driver, Stop) {
    kern_return_t ret = kIOReturnSuccess;
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "Stop");
    return ret;
}

kern_return_t IMPL(com_gotrustid_SoftFIDO2_SoftFido2Driver, NewUserClient) {
    kern_return_t ret = kIOReturnSuccess;
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "NewUserClient");
    IOService* client;
    ret = Create(this, "UserClientProperties", &client);
    if (ret != kIOReturnSuccess) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "IOService::Create failed: 0x%x", ret);
        return ret;
    }

    os_log(OS_LOG_DEFAULT, LOG_PREFIX "UserClient is created");

    *userClient = OSDynamicCast(IOUserClient, client);
    if (!*userClient) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "OSDynamicCast failed");
        client->release();
        return kIOReturnError;
    }
    return ret;
}
