//
//  SoftFido2UserClient.cpp
//  SoftFIDO2
//
//  Created by Eddie Hua on 2020/9/2.
//  Copyright © 2020 GoTrustID. All rights reserved.
//

#include <os/log.h>

#include <DriverKit/IOLib.h>
#include <DriverKit/OSData.h>
#include <DriverKit/IODispatchQueue.h>
#include <DriverKit/IOMemoryMap.h>
#include "UserKernelShared.h"
#include "com_gotrustid_SoftFIDO2_SoftFido2Driver.h"
#include "SoftFido2UserClient.h"
#include "SoftFido2Device.h"
#include "BufMemoryUtils.hpp"

#define LOG_PREFIX "[SoftFido2][UserClient] "

void debugArguments(IOUserClientMethodArguments* arguments);

struct SoftFido2UserClient_IVars {
    com_gotrustid_SoftFIDO2_SoftFido2Driver* provider;
    SoftFido2Device* fido2Device;
    //IODispatchQueue* commandQueue = nullptr;
    //
    OSAction* notifyFrameAction = nullptr;
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

    OSSafeReleaseNULL(ivars->notifyFrameAction);
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
    if (ivars->fido2Device != nullptr) {
        ivars->fido2Device->release();
    }
    return Stop(provider, SUPERDISPATCH);
}

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
    
    // <<< PrepareForDMA >>>
    uint64_t flags = 0;
    uint64_t dmaLength = 0;
    uint32_t dmaSegmentCount = 0;
    IOAddressSegment segments[32];
    ret = report->PrepareForDMA(0, this, 0, 0, &flags, &dmaLength, &dmaSegmentCount, segments);
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
    report->Map(0, 0, 0, 0, &address, &len);
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->Map > address = %llu,  len = %llu", address, len);
    //    if (ret != kIOReturnSuccess) {
    //        os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->Map Failed!");
    //        //return ret;
    //    } else {
    //
    // <<< CreateMapping >>>
    uint64_t options[5] = {kIOMemoryMapReadOnly, kIOMemoryMapCacheModeDefault, kIOMemoryMapCacheModeInhibit, kIOMemoryMapCacheModeCopyback, kIOMemoryMapCacheModeWriteThrough};
    for (int i = 0; i < 5; i++) {
        IOMemoryMap* map = nullptr;
        ret = report->CreateMapping(options[i], 0, 0, 0, 0, &map);
        if (ret != kIOReturnSuccess) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->CreateMapping %d Failed!", i);
            //return ret;
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->CreateMapping %d Success", i);
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "   map->GetAddress = %llu", map->GetAddress());
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "   map->GetLength = %llu", map->GetLength());
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "   map->GetOffset = %llu", map->GetOffset());
            
        }
    }
    
    // ------
    //os_log(OS_LOG_DEFAULT, LOG_PREFIX "Before AsyncCompletion");
    //ivars->notifyFrameAction->ivars->address
    if (ivars->notifyFrameAction != nullptr) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "notifyFrameAction ");
    } else {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "notifyFrameAction is null");
    }
    AsyncCompletion(ivars->notifyFrameAction, kIOReturnSuccess, (uint64_t*) address, (uint32_t) len);
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "After AsyncCompletion");
    return kIOReturnSuccess;
}

/* IOUserClientMethodDispatch
 IOUserClientMethodFunction function;
 uint32_t                   checkCompletionExists;
 uint32_t                   checkScalarInputCount;
 uint32_t                   checkStructureInputSize;
 uint32_t                   checkScalarOutputCount;
 uint32_t                   checkStructureOutputSize;
 */
const IOUserClientMethodDispatch sMethods[kNumberOfMethods] = {
    {(IOUserClientMethodFunction)&SoftFido2UserClient::sSendFrame, 0, 0, sizeof(U2FHID_FRAME), 0, 0},
    {(IOUserClientMethodFunction)&SoftFido2UserClient::sNotifyFrame, 0, 0, 0, 0, sizeof(U2FHID_FRAME)},
};

// 心得:
//      - ExternalMethod的 arguments->structureInput 有值，但透過 super::ExternalMethod 傳入 static sSendFrame就變null
//      - 而 reference 有值，只是不知道是什麼? 是宣告的 U2FHID_FRAME ?
kern_return_t SoftFido2UserClient::ExternalMethod(uint64_t selector,
                                                  IOUserClientMethodArguments* arguments,
                                                  const IOUserClientMethodDispatch* dispatch,
                                                  OSObject* target,
                                                  void* reference) {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "------------------<ExternalMethod>------------------");
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod selector = %llu", selector);
    debugArguments(arguments);
    // 區分不同的 selector，對應到不同的行為
    // 交由 IODispatchQueue 來處理
    switch (selector) {
        case kSoftFidoUserClientSendFrame: {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod SendFrame");
            // sSendFrame, 0, sizeof(U2FHID_FRAME), 0, 0
            dispatch = &sMethods[kSoftFidoUserClientSendFrame];
            target = this;
            //【嘗試方法】此處直接取用 arguments->structureInput，是可以取到內容。
            //  直接呼叫 sendFrame好像更方便
//                OSData* input = arguments->structureInput;
//                //frameLength = input->getLength();
//                os_log(OS_LOG_DEFAULT, LOG_PREFIX "  SendFrame input length = %lu", input->getLength());
//                U2FHID_FRAME* frame = (U2FHID_FRAME*) input->getBytesNoCopy();
//                sendFrame(frame, input->getLength());   // 直接呼叫
//                return kIOReturnSuccess;
                        
            //【傳統方法】透過此法呼叫 sSendFrame, reference的資料是傳入 U2FHID_FRAME，不需要自己轉換。
            //  應該是在宣告時有指定 checkStructureInputSize
            return super::ExternalMethod(selector, arguments, dispatch, target, reference);
        }
        case kSoftFidoUserClientNotifyFrame:
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod NotifyFrame");
            dispatch = &sMethods[kSoftFidoUserClientNotifyFrame];
            target = this;
            ivars->notifyFrameAction = arguments->completion;
            ivars->notifyFrameAction->retain();
            return kIOReturnSuccess;
            //return super::ExternalMethod(selector, arguments, dispatch, target, reference);   // 用這個會失敗!? 可能是 定義的 與 傳入的不符合
        default:
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod No support selector : %llu", selector);
            break;
    }
    
    return kIOReturnBadArgument;
}

//SoftFido2UserClient *target, uint64_t* reference, IOUserClientMethodArguments *arguments
kern_return_t IMPL(SoftFido2UserClient, sSendFrame) {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "sSendFrame");
    size_t frameLength = 0;
    U2FHID_FRAME* frame = nullptr;
//    // ---- Debug Start ----
//    if (arguments->structureInput != nullptr) {
//        OSData* input = arguments->structureInput;
//        frameLength = input->getLength();
//        os_log(OS_LOG_DEFAULT, LOG_PREFIX "  SendFrame input length = %lu", frameLength);
//        frame = (U2FHID_FRAME*) input->getBytesNoCopy();
//        uint8_t* byteArray = (uint8_t*) input->getBytesNoCopy();
//        if (byteArray != nullptr) {
//            for (int i = 0; i < input->getLength(); i++) {
//                os_log(OS_LOG_DEFAULT, LOG_PREFIX "  SendFrame input(%d) = %d", i, byteArray[i]);
//            }
//        } else {
//            os_log(OS_LOG_DEFAULT, LOG_PREFIX "  SendFrame input getBytesNoCopy is null");
//        }
//    } else {
//        os_log(OS_LOG_DEFAULT, LOG_PREFIX "  SendFrame arguments->structureInput is null");
//    }
    if (reference != nullptr) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "  SendFrame reference ✅");
        frame = (U2FHID_FRAME*) reference;
        frameLength = sizeof(U2FHID_FRAME);
        //OSObject* object = (OSObject*) reference;
        //os_log(OS_LOG_DEFAULT, LOG_PREFIX "  (OSObject) reference = %s", object->GetClassName());
        //OSData* input = (OSData*) reference;
        //frameLength = input->getLength();
        //frame = (U2FHID_FRAME*) input->getBytesNoCopy();
    } else {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "  SendFrame reference is null");
    }
    // ---- Debug Stop ----
    if (frameLength == HID_RPT_SIZE) {
        return target->sendFrame(frame, frameLength);
    }
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "  SendFrame kIOReturnBadArgument");
    return kIOReturnBadArgument;
}

//virtual kern_return_t sendFrame(U2FHID_FRAME *frame, size_t frameSize);
kern_return_t IMPL(SoftFido2UserClient, sendFrame) {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "sendFrame refCount = %lu", frameSize);
    // 把資料做成 report
    IOMemoryDescriptor* report = nullptr;
    auto ret = BufMemoryUtils::createWithBytes(frame, frameSize, &report);
    if (ret != kIOReturnSuccess) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "sendFrame > create report failed");
        return ret;
    }
    uint64_t reportLength;
    ret = report->GetLength(&reportLength);
    if (ret != kIOReturnSuccess) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "sendFrame > get report length failed");
        return ret;
    }
    ret = ivars->fido2Device->handleReport(mach_absolute_time(), report, static_cast<uint32_t>(reportLength), kIOHIDReportTypeInput, 0);
    if (ret != kIOReturnSuccess) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "sendFrame > fido2Device->handleReport failed");
    }
    return ret;
}

kern_return_t IMPL(SoftFido2UserClient, sNotifyFrame) {
//kern_return_t SoftFido2UserClient::sNotifyFrame(SoftFido2UserClient *target, IOUserClientMethodArguments *arguments) {
    //
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "sNotifyFrame");
    return target->notifyFrame(999);
}

kern_return_t IMPL(SoftFido2UserClient, notifyFrame) {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "notifyFrame refCount = %u", refCount);
    return kIOReturnSuccess;
}

/*
 CopyClientMemoryForType(\
     uint64_t type,\
     uint64_t * options,\
     IOMemoryDescriptor ** memory,\
     OSDispatchMethod supermethod = NULL);\
 */
kern_return_t IMPL(SoftFido2UserClient, CopyClientMemoryForType) {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "CopyClientMemoryForType = %llu", type);
    kern_return_t ret;
    if (type == 0) {
        IOBufferMemoryDescriptor* buffer = nullptr;
        ret = IOBufferMemoryDescriptor::Create(kIOMemoryDirectionInOut, 128 /* capacity */, 8 /* alignment */, &buffer);
        if (ret != kIOReturnSuccess) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "CopyClientMemoryForType > IOBufferMemoryDescriptor::Create failed: 0x%x", ret);
        } else {
            *memory = buffer; // returned with refcount 1
        }
    } else {
        ret = super::CopyClientMemoryForType(type, options, memory);
    }
    return ret;
}


#pragma mark - Debug

void debugArguments(IOUserClientMethodArguments* arguments) {
    // ExternalMethod target is null.
    // ExternalMethod dispatch is null.
    // ExternalMethod reference is null.
    if (arguments != nullptr) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod check arguments");
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->version = %llu", arguments->version);
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->scalarInputCount = %u", arguments->scalarInputCount);
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->scalarOutputCount = %u", arguments->scalarOutputCount);
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureOutputMaximumSize = %llu", arguments->structureOutputMaximumSize);
        if (arguments->completion != nullptr) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->completion ✅");
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->completion = null");
        }
        if (arguments->scalarInput != nullptr) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->scalarInput ✅");
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->scalarInput = null");
        }
        if (arguments->scalarOutput != nullptr) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->scalarOutput ✅");
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->scalarOutput = null");
        }
        if (arguments->structureInput != nullptr) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureInput ✅");
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureInput = null");
        }
        if (arguments->structureInputDescriptor != nullptr) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureInputDescriptor ✅");
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureInputDescriptor = null");
        }
        
        if (arguments->structureOutput != nullptr) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureOutput ✅");
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureOutput = null");
        }
        
        if (arguments->structureOutputDescriptor != nullptr) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureOutputDescriptor ✅");
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureOutputDescriptor = null");
        }
    } else {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments is null!");
    }
}
