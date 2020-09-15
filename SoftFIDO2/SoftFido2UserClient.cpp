//
//  SoftFido2UserClient.cpp
//  SoftFIDO2
//
//  Created by Eddie Hua on 2020/9/2.
//  Copyright © 2020 GoTrustID. All rights reserved.
//

#include <os/log.h>

#include <DriverKit/IOLib.h>
#include <DriverKit/IODispatchQueue.h>
#include "UserKernelShared.h"
#include "com_gotrustid_SoftFIDO2_SoftFido2Driver.h"
#include "SoftFido2UserClient.h"
#include "SoftFido2Device.h"

#define LOG_PREFIX "[SoftFido2][UserClient] "


struct SoftFido2UserClient_IVars {
    com_gotrustid_SoftFIDO2_SoftFido2Driver* provider;
    SoftFido2Device* fido2Device;
    IODispatchQueue* commandQueue = nullptr;
    //
    OSAction* notifyFrameAction = nullptr;
    // From SoftU2F (原本宣告在 SoftU2FUserClient.hpp 的 private
    //    IOCommandGate *_commandGate = nullptr;  => 用 IODispatchQueue 取代
    //
    //    typedef struct {
    //        uint32_t                    selector;
    //        IOExternalMethodArguments * arguments;
    //        IOExternalMethodDispatch *  dispatch;
    //        OSObject *                  target;
    //        void *                      reference;
    //    } ExternalMethodGatedArguments;
};


bool SoftFido2UserClient::init() {
    if (super::init()) {
        ivars = IONewZero(SoftFido2UserClient_IVars, 1);
        if (ivars != nullptr) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "init success.");
            return true;
        }
    }
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "init failed!");
    return false;
}

void SoftFido2UserClient::free() {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "free");

    OSSafeReleaseNULL(ivars->fido2Device);
    IOSafeDeleteNULL(ivars, SoftFido2UserClient_IVars, 1);
    super::free();
}

kern_return_t IMPL(SoftFido2UserClient, Start) {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "Start");

    ivars->provider = OSDynamicCast(com_gotrustid_SoftFIDO2_SoftFido2Driver, provider);
    if (!ivars->provider) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "Cast to com_gotrustid_SoftFIDO2_SoftFido2Driver Failed!");
        return kIOReturnError;
    }
    if (super::Start(provider, SUPERDISPATCH) != kIOReturnSuccess) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "Start Failed!");
        return kIOReturnError;
    }
    kern_return_t ret = kIOReturnSuccess;
    IOService* service = nullptr;
    ret = Create(this, "Fido2HidProperties", &service);
    if (ret != kIOReturnSuccess) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "IOService::Create failed: 0x%x", ret);
        return ret;
    }
    
    SoftFido2Device* device = OSDynamicCast(SoftFido2Device, service);
    ivars->fido2Device = device;
    if (ivars->fido2Device == nullptr) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "SoftFido2Device is null");
        service->release();
        return kIOReturnError;
    }
//    ret = IODispatchQueue::Create("commandGate", 0 /*options*/ , 0 /*priority*/, &(ivars->commandQueue));
//    if (ret != kIOReturnSuccess) {
//        os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODispatchQueue::Create CommandGate Failed : %d", ret);
//        return ret;
//    }
//    device->release();
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "--- Start ---");
    return ret;
}

kern_return_t IMPL(SoftFido2UserClient, Stop) {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "Stop");
    return Stop(provider, SUPERDISPATCH);
}
//void SoftFido2UserClient::frameReceivedGated(IOMemoryDescriptor *report) {
//    os_log(OS_LOG_DEFAULT, LOG_PREFIX "frameReceivedGated Report = %p", report);
//    IOMemoryMap *reportMap = nullptr;
//    if (isInactive() || !_notifyRef)
//        return;
//
//    if (report->getLength() != sizeof(U2FHID_FRAME) || report->prepare() != kIOReturnSuccess)
//        return;
//
//    // Map report into kernel space.
//    reportMap = report->map();
//
//    if (reportMap != nullptr) {
//        // Notify userland that we got a report.
//        io_user_reference_t *args = (io_user_reference_t *)reportMap->getAddress();
//        sendAsyncResult64(*_notifyRef, kIOReturnSuccess, args, sizeof(U2FHID_FRAME) / sizeof(io_user_reference_t));
//        reportMap->release();
//    }
//
//    report->complete();
//}

/*
 DriverKit 沒有
 IOReturn SoftU2FUserClient::clientClose(void) {}
 */
kern_return_t IMPL(SoftFido2UserClient, frameReceived) {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "frameReceived Report = %p", report);
    // <<< GetLength >>>
    uint64_t length = 0;
    kern_return_t ret = report->GetLength(&length);
    if (ret != kIOReturnSuccess) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->GetLength Failed!");
        return ret;
    }
    // (結果)都是 64bytes
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->GetLength = %llu", length);
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "sizeof(U2FHID_FRAME) = %lu", sizeof(U2FHID_FRAME));
    
    // <<< CreateMapping >>>
    uint64_t options = 0;
//    IOMemoryMap* map = nullptr;
//    ret = report->CreateMapping(kIOMemoryMapReadOnly, 0, 0, 0, 0, &map);
//    if (ret != kIOReturnSuccess) {
//        os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->CreateMapping Failed!");
//        //return ret;
//    }
    
    // <<< PrepareForDMA >>>
    uint64_t flags = 0;
    uint64_t dmaLength = 0;
    uint32_t dmaSegmentCount = 0;
    //IOAddressSegment segments;
    IOAddressSegment segments[32];
    ret = report->PrepareForDMA(options, this, 0, 0, &flags, &dmaLength, &dmaSegmentCount, segments);
    if (ret != kIOReturnSuccess) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->PrepareForDMA Failed!");
        //return ret;
    } else {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->PrepareForDMA flags = %llu", flags);
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->PrepareForDMA Length = %llu", dmaLength);
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->PrepareForDMA SegmentCount = %u", dmaSegmentCount);
        if (dmaSegmentCount > 0) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->PrepareForDMA segments.address = %llu", segments[0].address);
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->PrepareForDMA segments.length = %llu", segments[0].length);
        }
    }
    // -----
    // <<< Map >>>
    uint64_t address = 0;
    uint64_t len = 0;
    //ret =
    report->Map(options, 0, 0, 0, &address, &len);
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->Map > address = %llu,  len = %llu", address, len);
//    if (ret != kIOReturnSuccess) {
//        os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->Map Failed!");
//        //return ret;
//    } else {
//    }

    // ------
    // 1. Create 要跟外界溝通的記憶體空間 (考慮由外部提供，就不用一直 allocate )
//    IOBufferMemoryDescriptor* bufferMemoryDesc = nullptr;
//    ret = IOBufferMemoryDescriptor::Create(kIOMemoryDirectionOut, length, 0, &bufferMemoryDesc);
//    memcpy(reinterpret_cast<void*>(address), bytes, length);
    
    // ------
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "Before AsyncCompletion");
    AsyncCompletion(ivars->notifyFrameAction, kIOReturnSuccess, (uint64_t*) address, (uint32_t) len);
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "After AsyncCompletion");
    return kIOReturnSuccess;
}

/*
 ExternalMethod arguments->version = 2
 ExternalMethod arguments->scalarInputCount = 0
 ExternalMethod arguments->scalarOutputCount = 0
 ExternalMethod arguments->structureOutputMaximumSize = 0
 ExternalMethod arguments->completion
 ExternalMethod arguments->scalarInput
 ExternalMethod arguments->scalarOutput
 ExternalMethod arguments->structureInput = null
 ExternalMethod arguments->structureInputDescriptor = null
 ExternalMethod arguments->structureOutput = null
 ExternalMethod arguments->structureOutputDescriptor = null
 */
kern_return_t SoftFido2UserClient::ExternalMethod(uint64_t selector,
                                                  IOUserClientMethodArguments* arguments,
                                                  const IOUserClientMethodDispatch* dispatch,
                                                  OSObject* target,
                                                  void* reference) {
    if (arguments != nullptr) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod check arguments");
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->version = %llu", arguments->version);
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->scalarInputCount = %u", arguments->scalarInputCount);
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->scalarOutputCount = %u", arguments->scalarOutputCount);
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureOutputMaximumSize = %llu", arguments->structureOutputMaximumSize);
        if (arguments->completion != nullptr) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->completion");
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->completion = null");
        }
        if (arguments->scalarInput != nullptr) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->scalarInput");
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->scalarInput = null");
        }
        if (arguments->scalarOutput != nullptr) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->scalarOutput");
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->scalarOutput = null");
        }
        if (arguments->structureInput != nullptr) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureInput");
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureInput = null");
        }
        if (arguments->structureInputDescriptor != nullptr) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureInputDescriptor");
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureInputDescriptor = null");
        }
        
        if (arguments->structureOutput != nullptr) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureOutput");
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureOutput = null");
        }
        
        if (arguments->structureOutputDescriptor != nullptr) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureOutputDescriptor");
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureOutputDescriptor = null");
        }
    } else {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments is null!");
    }
    
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod selector = %llu", selector);
    // 區分不同的 selector，對應到不同的行為
    // 交由 IODispatchQueue 來處理
    switch (selector) {
        case kSoftFidoUserClientSendFrame:
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod kSoftFidoUserClientSendFrame");
            // sSendFrame, 0, sizeof(U2FHID_FRAME), 0, 0
            return super::ExternalMethod(selector, arguments, dispatch, target, reference);
        case kSoftFidoUserClientNotifyFrame:
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod kSoftFidoUserClientNotifyFrame");
            ivars->notifyFrameAction = arguments->completion;
            return kIOReturnSuccess;
            //return super::ExternalMethod(selector, arguments, dispatch, target, reference);
        default:
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod No support selector : %llu", selector);
            break;
    }
    
//    ivars->commandQueue->DispatchAsync(^{ 
//    });
    return kIOReturnBadArgument;
}

kern_return_t IMPL(SoftFido2UserClient, notifyFrame) {
    return kIOReturnSuccess;
}

