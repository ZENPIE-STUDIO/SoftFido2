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
#include <DriverKit/IOBufferMemoryDescriptor.h>
#include "UserKernelShared.h"
#include "com_gotrustid_SoftFIDO2_SoftFido2Driver.h"
#include "SoftFido2UserClient.h"
#include "SoftFido2Device.h"
#include "BufMemoryUtils.hpp"
//#include <string.h>
//#include <stdio.h>

#define LOG_PREFIX "[SoftFido2][UserClient] "

void debugArguments(IOUserClientMethodArguments* arguments);

struct SoftFido2UserClient_IVars {
    com_gotrustid_SoftFIDO2_SoftFido2Driver* provider;
    SoftFido2Device* fido2Device;
    //IODispatchQueue* commandQueue = nullptr;
    //
    OSAction* notifyFrameAction = nullptr;
    IOBufferMemoryDescriptor* notifyFrameMemoryDesc = nullptr;
    IOMemoryDescriptor* ouputDescriptor = nullptr;  // structureOutputDescriptor
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

    OSSafeReleaseNULL(ivars->notifyFrameMemoryDesc);
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
// 結果列印不出東西 textBuffer = <private>
//kern_return_t IMPL(SoftFido2UserClient, dump) {
//    uint8_t* byteArray = reinterpret_cast<uint8_t*>(address);
//    char textBuffer[512], hexBuf[6];
//    strlcpy(textBuffer, "DATA: ", 256);
//    for (int i = 0; i < length; i++) {
//        snprintf(hexBuf, 6, "%02x ", byteArray[i]);
//        strlcat(textBuffer, hexBuf, 256);
//    }
//    strlcat(textBuffer, "\n", 256);
//    os_log(OS_LOG_DEFAULT, LOG_PREFIX "textBuffer = %s", textBuffer);
//    return kIOReturnSuccess;
//}

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
    
    // << 這裡 Map 會失敗 >>
//    uint64_t address1;
//    uint64_t len1;
//    ret = report->Map(0, 0, 0, 0, &address1, &len1);
//    if (ret == kIOReturnSuccess) {
//        os_log(OS_LOG_DEFAULT, LOG_PREFIX "address1 = %llu", address1);
//        os_log(OS_LOG_DEFAULT, LOG_PREFIX "len1 = %llu", len1);
//        uint8_t* byteArray = reinterpret_cast<uint8_t*>(address1);
//        char textBuffer[256], hexBuf[6];
//        strlcpy(textBuffer, "DATA: ", 256);
//        for (int i = 0; i < len1; i++) {
//            snprintf(hexBuf, 6, "%02x ", byteArray[i]);
//            strlcat(textBuffer, hexBuf, 256);
//        }
//        strlcat(textBuffer, "\n", 256);
//        os_log(OS_LOG_DEFAULT, LOG_PREFIX "textBuffer = %s", textBuffer);
//    } else {
//        os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->Map failed = %d", ret);
//    }
    //----------------------------

//    uint64_t notifyAddress = 0;
//    uint64_t notifyLength = 0;
//    //IOMemoryMap* notifyMap = nullptr;
//    ivars->notifyFrameMemoryDesc->Map(0, 0, 0, 0, &notifyAddress, &notifyLength);
//    os_log(OS_LOG_DEFAULT, LOG_PREFIX "notifyAddress = %llu", notifyAddress);
//    os_log(OS_LOG_DEFAULT, LOG_PREFIX "notifyLength = %llu", notifyLength);
    
    // report CreateMapping 都失敗
//    ret = report->CreateMapping(kIOMemoryMapFixedAddress, notifyAddress, 0, notifyLength, 0, &notifyMap);
//    if (notifyMap != nullptr) {
//        os_log(OS_LOG_DEFAULT, LOG_PREFIX "kIOMemoryMap notifyMap address = %llu", notifyMap->GetAddress());
//        os_log(OS_LOG_DEFAULT, LOG_PREFIX "kIOMemoryMap notifyMap length = %llu", notifyMap->GetLength());
//    } else {
//        os_log(OS_LOG_DEFAULT, LOG_PREFIX "kIOMemoryMap notifyMap failed = %d", ret);
//    }
    
    IOUserClientAsyncArgumentsArray args;
    // <<< PrepareForDMA >>>
    uint64_t flags = 0;
    uint64_t dmaLength = 0;
    uint32_t dmaSegmentCount = 1;
    IOAddressSegment segments[32];
    // 可以得到 1個 block 64 bytes
    ret = report->PrepareForDMA(0, this, 0, 0, &flags, &dmaLength, &dmaSegmentCount, segments);
    //IOMemoryDescriptor* outputReport = nullptr;
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
            
            // ---------------
            uint64_t outAddress = 0;
            uint64_t outLength = 0;
            ivars->notifyFrameMemoryDesc->Map(0, segments[0].address, segments[0].length, 0, &outAddress, &outLength);
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "notifyFrameMemoryDesc address = %llu", outAddress);
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "notifyFrameMemoryDesc length = %llu", outLength);
            //dump(outAddress, outLength);
            args[0] = outAddress;
            
            // Test
            IOBufferMemoryDescriptor* frameMemoryDesc = nullptr;
            IOBufferMemoryDescriptor::Create(kIOMemoryDirectionOut, 64, 0, &frameMemoryDesc);
            frameMemoryDesc->Map(0, segments[0].address, segments[0].length, 0, &outAddress, &outLength);
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "frameMemoryDesc address = %llu", outAddress);
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "frameMemoryDesc length = %llu", outLength);
            args[1] = outAddress;
            //ivars->ouputDescriptor->Map(0, segments[0].address, segments[0].length, 0, &outAddress, &outLength);
            //os_log(OS_LOG_DEFAULT, LOG_PREFIX "ouputDescriptor address = %llu", outAddress);
            //os_log(OS_LOG_DEFAULT, LOG_PREFIX "ouputDescriptor length = %llu", outLength);
            //dump(outAddress, outLength);
//            address = outAddress;
        }
    }
    uint32_t asyncDataCount = 2;//(uint32_t) (segments[0].length / 64);
    //uint32_t asyncDataCount = sizeof(U2FHID_FRAME) / sizeof(uint64_t);
    //AsyncCompletion(OSAction *action, IOReturn status, IOUserClientAsyncArgumentsArray, uint32_t asyncDataCount)
    // TODO: 傳遞的內容是 asyncDataCount個 8bytes => U2FHID_FRAME size 64bytes
    //      asyncDataCount 是 64/8
    AsyncCompletion(ivars->notifyFrameAction, kIOReturnSuccess, args, asyncDataCount);
    //os_log(OS_LOG_DEFAULT, LOG_PREFIX "After AsyncCompletion address = %llu", address);
    return kIOReturnSuccess;
    // 舊的參考
    //    numArgs = sizeof(U2FHID_FRAME) / sizeof(io_user_reference_t) = 64/8
    //io_user_reference_t *args = (io_user_reference_t *)reportMap->getAddress();
    //sendAsyncResult64(*_notifyRef, kIOReturnSuccess, args, sizeof(U2FHID_FRAME) / sizeof(io_user_reference_t));

}

/* IOUserClientMethodDispatch
 IOUserClientMethodFunction function;
 uint32_t                   checkCompletionExists;
 uint32_t                   checkScalarInputCount;
 uint32_t                   checkStructureInputSize;
 uint32_t                   checkScalarOutputCount;
 uint32_t                   checkStructureOutputSize;
 */
//const IOUserClientMethodDispatch sMethods[kNumberOfMethods] = {
//    {(IOUserClientMethodFunction)&SoftFido2UserClient::sSendFrame, 0, 0, sizeof(U2FHID_FRAME), 0, 0},
//    {(IOUserClientMethodFunction)&SoftFido2UserClient::sNotifyFrame, 0, 0, 0, 0, sizeof(U2FHID_FRAME)},
//};
//SoftFido2UserClient *target, uint64_t* reference, IOUserClientMethodArguments *arguments
kern_return_t sSendFrame(SoftFido2UserClient* target, IOUserClientMethodArguments *arguments) {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "sSendFrame");
    size_t frameLength = 0;
    U2FHID_FRAME* frame = nullptr;

    if (arguments->structureInput != nullptr) {
        OSData* input = arguments->structureInput;
        frameLength = input->getLength();
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "  SendFrame input length = %lu", frameLength);
        frame = (U2FHID_FRAME*) input->getBytesNoCopy();
    } else {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "  SendFrame arguments->structureInput is null");
    }
    // ---- Debug Stop ----
    if (frameLength == HID_RPT_SIZE) {
        return target->sendFrame(frame, frameLength);
    }
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "  SendFrame kIOReturnBadArgument");
    return kIOReturnBadArgument;
}


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
    
    // ExternalMethod target is null.
    // ExternalMethod dispatch is null.
    // ExternalMethod reference is null.
    if (reference != nullptr) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod reference ✅");
    } else {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod reference = null");
    }
    
    debugArguments(arguments);
    // 區分不同的 selector，對應到不同的行為
    // 交由 IODispatchQueue 來處理
    switch (selector) {
        case kSoftFidoUserClientSendFrame: {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod SendFrame");
            //【嘗試方法】此處直接取用 arguments->structureInput，是可以取到內容。
            //  直接呼叫 sendFrame好像更方便
            return sSendFrame(this, arguments);
            //【傳統方法】透過此法呼叫 sSendFrame, reference的資料是傳入 U2FHID_FRAME，不需要自己轉換。
            //  應該是在宣告時有指定 checkStructureInputSize
            // sSendFrame, 0, sizeof(U2FHID_FRAME), 0, 0
            //dispatch = &sMethods[kSoftFidoUserClientSendFrame];
            //target = this;
            //return super::ExternalMethod(selector, arguments, dispatch, target, reference);
        }
        // 測試直接回傳
//        case kSoftFidoUserClientNotifyFrame + 1: {
//            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod NotifyFrame (Sync)");
//            U2FHID_FRAME frame;
//            frame.type = 0xFF;
//            frame.cid = 123;
//            arguments->structureOutput = OSData::withBytes(&frame, sizeof(frame));
//            return kIOReturnSuccess;
//        }
        case kSoftFidoUserClientNotifyFrame: {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod NotifyFrame (Async)");
            //dispatch = &sMethods[kSoftFidoUserClientNotifyFrame];
            //target = this;
            ivars->notifyFrameAction = arguments->completion;
            ivars->notifyFrameAction->retain();
            // 自行建立
            IOBufferMemoryDescriptor* frameMemoryDesc = nullptr;
            IOBufferMemoryDescriptor::Create(kIOMemoryDirectionOut, 64, 0, &frameMemoryDesc);
            ivars->notifyFrameMemoryDesc = frameMemoryDesc;
            ivars->notifyFrameMemoryDesc->retain();
            //
            if (arguments->structureOutputDescriptor != nullptr) {
                ivars->ouputDescriptor = arguments->structureOutputDescriptor;
                ivars->ouputDescriptor->retain();
            }
            return kIOReturnSuccess;
            //return super::ExternalMethod(selector, arguments, dispatch, target, reference);   // 用這個會失敗!? 可能是 定義的 與 傳入的不符合
        }
        default:
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod No support selector : %llu", selector);
            break;
    }
    
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
    if (arguments != nullptr) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->version = %llu", arguments->version);
        
        if (arguments->completion != nullptr) { // Async 才會有
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->completion ✅");
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->completion = null");
        }
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->scalarInputCount = %u", arguments->scalarInputCount);
        if (arguments->scalarInput != nullptr) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->scalarInput ✅");
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->scalarInput = null");
        }
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->scalarOutputCount = %u", arguments->scalarOutputCount);
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
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments->structureOutputMaximumSize = %llu", arguments->structureOutputMaximumSize);
    } else {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod arguments is null!");
    }
}
