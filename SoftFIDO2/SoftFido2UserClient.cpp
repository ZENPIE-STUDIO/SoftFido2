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
#include <string.h>
#include <stdio.h>

#define LOG_PREFIX "[SoftFido2][UserClient] "

void debugArguments(IOUserClientMethodArguments* arguments);

struct SoftFido2UserClient_IVars {
    com_gotrustid_SoftFIDO2_SoftFido2Driver* provider;
    SoftFido2Device* fido2Device;
    //IODispatchQueue* commandQueue = nullptr;
    //
    uint64_t notifyArgs[8];
    OSAction* notifyFrameAction = nullptr;
    IOBufferMemoryDescriptor* notifyFrameMemoryDesc = nullptr;  // 目前停用：因為傳出給app，也沒得到假資料
    IOMemoryDescriptor* outputDescriptor = nullptr;  // structureOutputDescriptor
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
// textBuffer 列印不出東西, 會看到 <private>
kern_return_t IMPL(SoftFido2UserClient, dump) {
    uint8_t* byteArray = reinterpret_cast<uint8_t*>(address);
    //char textBuffer[512], hexBuf[6];
    //strlcpy(textBuffer, "DATA: ", 256);
    // 因為是 64/8，所以不特地處理其他長度
    for (int i = 0; i < length; i+=8) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "%02x %02x %02x %02x %02x %02x %02x %02x",
               byteArray[i], byteArray[i+1], byteArray[i+2], byteArray[i+3],
               byteArray[i+4], byteArray[i+5], byteArray[i+6], byteArray[i+7]);
        //snprintf(hexBuf, 6, "%02x ", byteArray[i]);
        //strlcat(textBuffer, hexBuf, 256);
    }
    //strlcat(textBuffer, "\n", 256);
    //os_log(OS_LOG_DEFAULT, LOG_PREFIX "textBuffer = %s", textBuffer);
    return kIOReturnSuccess;
}

kern_return_t IMPL(SoftFido2UserClient, frameReceived) {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "frameReceived Report = %p", report);
    // <<< GetLength >>>
    //
    uint64_t length = 0;
    kern_return_t ret = report->GetLength(&length);
    if (ret != kIOReturnSuccess) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->GetLength Failed!");
        return ret;
    }
    // (結果)都是 64bytes
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->GetLength = %llu", length);
    //os_log(OS_LOG_DEFAULT, LOG_PREFIX "sizeof(U2FHID_FRAME) = %lu", sizeof(U2FHID_FRAME));
    
    // --------------------------------
    IODispatchQueue* queue = NULL;
    ivars->provider->getDispatchQueue(&queue);
    if (queue != NULL) {
        queue->DispatchAsync(^{
            innerFrameReceived(report);
        });
    }
    // --------------------------------
    
    return kIOReturnSuccess;
}
kern_return_t IMPL(SoftFido2UserClient, innerFrameReceived) {
//void SoftFido2UserClient::innerFrameReceived() {
    kern_return_t ret = kIOReturnSuccess;
    uint64_t outAddress = 0;
    uint64_t outLength = 0;
    ret = ivars->outputDescriptor->Map(0, 0, 0, 0, &outAddress, &outLength);
    // <<< IODMACommand >>>
    uint64_t flags = 0;
    uint64_t dmaLength = 0;
    uint32_t dmaSegmentCount = 1;
    IOAddressSegment segments[32];
    IODMACommandSpecification spec;
    spec.maxAddressBits = 64;
    IODMACommand* dmaCmd = nullptr;
    ret = IODMACommand::Create(ivars->fido2Device, 0, &spec, &dmaCmd);
    if (dmaCmd != nullptr) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODMACommand::Create Success");
        ret = dmaCmd->PrepareForDMA(0, report, 0, 0, &flags, &dmaSegmentCount, segments);
        if (ret == kIOReturnSuccess) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "dmaCmd->PrepareForDMA flags = %llu", flags);
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "dmaCmd->PrepareForDMA Length = %llu", dmaLength);
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "dmaCmd->PrepareForDMA SegmentCount = %u", dmaSegmentCount);
            if (dmaSegmentCount > 0) {
                os_log(OS_LOG_DEFAULT, LOG_PREFIX "dmaCmd->PrepareForDMA segments.address = %llu", segments[0].address);
                os_log(OS_LOG_DEFAULT, LOG_PREFIX "dmaCmd->PrepareForDMA segments.length = %llu", segments[0].length);
            }
            uint64_t offset;
            uint64_t length;
            IOMemoryDescriptor* out = nullptr;
            ret = dmaCmd->GetPreparation(&offset, &length, &out);
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "dmaCmd->GetPreparation = %d", ret);
            if (out != nullptr) {
                IOMemoryMap* outMemMap = nullptr;
                out->CreateMapping(0, 0, 0, 0, 0, &outMemMap);
                if (outMemMap != nullptr) {
                    os_log(OS_LOG_DEFAULT, LOG_PREFIX "outMemMap CreateMapping address = %llu", outMemMap->GetAddress());
                    os_log(OS_LOG_DEFAULT, LOG_PREFIX "outMemMap CreateMapping length = %llu", outMemMap->GetLength());
                    
                    memcpy((void*) outAddress, (void*) outMemMap->GetAddress(), outMemMap->GetLength());
                    ivars->notifyArgs[0] = outAddress;
                    OSSafeReleaseNULL(outMemMap);
                }
            }
            ret = dmaCmd->CompleteDMA(0);
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "dmaCmd->CompleteDMA = %d", ret);
        }
        OSSafeReleaseNULL(dmaCmd);
    } else {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODMACommand::Create failed = %d", ret);
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODMACommand::Create system err = %x", err_get_system(ret));
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODMACommand::Create    sub err = %x", err_get_sub(ret));
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODMACommand::Create   code err = %x", err_get_code(ret));
    }
    //----------------------------
    uint32_t asyncDataCount = 1;
    AsyncCompletion(ivars->notifyFrameAction, kIOReturnSuccess, ivars->notifyArgs, asyncDataCount);
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "After AsyncCompletion args = %llu, count = %u", (uint64_t) ivars->notifyArgs, asyncDataCount);
    return kIOReturnSuccess;
}

/*
 CopyClientMemoryForType(\
     uint64_t type,\
     uint64_t * options,\
     IOMemoryDescriptor ** memory,\
     OSDispatchMethod supermethod = NULL);\
 */
//kern_return_t IMPL(SoftFido2UserClient, CopyClientMemoryForType) {
//    os_log(OS_LOG_DEFAULT, LOG_PREFIX "CopyClientMemoryForType = %llu", type);
//    kern_return_t ret;
//    if (type == 0) {
//        IOBufferMemoryDescriptor* buffer = nullptr;
//        ret = IOBufferMemoryDescriptor::Create(kIOMemoryDirectionInOut, 128 /* capacity */, 8 /* alignment */, &buffer);
//        if (ret != kIOReturnSuccess) {
//            os_log(OS_LOG_DEFAULT, LOG_PREFIX "CopyClientMemoryForType > IOBufferMemoryDescriptor::Create failed: 0x%x", ret);
//        } else {
//            *memory = buffer; // returned with refcount 1
//        }
//    } else {
//        ret = super::CopyClientMemoryForType(type, options, memory);
//    }
//    return ret;
//}

#pragma mark - Send

//SoftFido2UserClient *target, uint64_t* reference, IOUserClientMethodArguments *arguments
//kern_return_t sSendFrame(SoftFido2UserClient* target, IOMemoryDescriptor* report) {
//    os_log(OS_LOG_DEFAULT, LOG_PREFIX "sSendFrame");
//    kern_return_t ret = kIOReturnBadArgument;
//    if (report != nullptr) {
//        ret = target->sendReport(report);
//        OSSafeReleaseNULL(report);
//    }
//    os_log(OS_LOG_DEFAULT, LOG_PREFIX "  sendReport ret = %d", ret);
//    return ret;
//}

//virtual kern_return_t sendReport(IOMemoryDescriptor* report);
kern_return_t IMPL(SoftFido2UserClient, sendReport) {
    uint64_t reportLength;
    auto ret = report->GetLength(&reportLength);
    if (ret != kIOReturnSuccess) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "sendReport > get report length failed");
        return ret;
    }
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "sendReport Length = %llu", reportLength);
    // ---------<DEBUG DUMP>----------
//    uint64_t address = 0;
//    uint64_t length = 0;
//    report->Map(0, 0, 0, 0, &address, &length);
//    dump(address, length);
    // -------------------------------
    ret = ivars->fido2Device->handleReport(mach_absolute_time(), report, static_cast<uint32_t>(reportLength), kIOHIDReportTypeInput, 0);
    if (ret != kIOReturnSuccess) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "sendReport > fido2Device->handleReport failed");
    } else {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "sendReport > fido2Device->handleReport ✅");
    }
    return ret;
}
#pragma mark - ExternalMethod
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

// 心得:
//      - ExternalMethod的 arguments->structureInput 有值，但透過 super::ExternalMethod 傳入 static sSendFrame就變null
//      - 而 reference 有值，只是不知道是什麼? 是宣告的 U2FHID_FRAME ?
kern_return_t SoftFido2UserClient::ExternalMethod(uint64_t selector,
                                                  IOUserClientMethodArguments* arguments,
                                                  const IOUserClientMethodDispatch* dispatch,
                                                  OSObject* target,
                                                  void* reference) {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "-------------<ExternalMethod>-------------");
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod selector = %llu", selector);
    
    // ExternalMethod target is null.
    // ExternalMethod dispatch is null.
    // ExternalMethod reference is null.
//    if (reference != nullptr) {
//        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod reference ✅");
//    } else {
//        os_log(OS_LOG_DEFAULT, LOG_PREFIX "ExternalMethod reference = null");
//    }
    //debugArguments(arguments);
    // 區分不同的 selector，對應到不同的行為
    // 交由 IODispatchQueue 來處理
    switch (selector) {
        case kSoftU2FUserClientSendFrame: {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "<<<<<<< SendFrame >>>>>>>");
            IOMemoryDescriptor* report = nullptr;
            if (arguments->structureInput != nullptr) {
                // ---------<DEBUG DUMP>----------
                dump((uint64_t) arguments->structureInput->getBytesNoCopy(), arguments->structureInput->getLength());
                // -------------------------------
                BufMemoryUtils::createMemoryDescriptorFromData(arguments->structureInput,
                                                               kIOMemoryDirectionIn,
                                                               &report);
            } else if (arguments->structureInputDescriptor) {
                report = arguments->structureInputDescriptor;
                report->retain();
            }
            kern_return_t ret = kIOReturnBadArgument;
            if (report != nullptr) {
                ret = sendReport(report);
                OSSafeReleaseNULL(report);
            }
            return ret;
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
        case kSoftU2FUserClientNotifyFrame: {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "<<<<<<< NotifyFrame(Async) >>>>>>>");
            //dispatch = &sMethods[kSoftFidoUserClientNotifyFrame];
            //target = this;
            ivars->notifyFrameAction = arguments->completion;
            ivars->notifyFrameAction->retain();
            //
            if (arguments->structureOutputDescriptor != nullptr) {
                ivars->outputDescriptor = arguments->structureOutputDescriptor;
                ivars->outputDescriptor->retain();
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


#pragma mark - Backup
// (有用: 不過是假資料) 用外部傳入的 Output Buffer，就可以成功把資料傳給App
/*
void SoftFido2UserClient::putDummyDataToOutputBuffer() {
    uint64_t outAddress = 0;
    uint64_t outLength = 0;
    ivars->outputDescriptor->Map(0, 0, 0, 0, &outAddress, &outLength);
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "notifyFrameMemoryDesc address = %llu", outAddress);
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "notifyFrameMemoryDesc length = %llu", outLength);
    // TRY: 故意寫一些假資料進去
    uint8_t testByteArray[64] = {1,2,3,4,5,6,7,8,
        1,2,3,4,5,6,7,8,
        1,2,3,4,5,6,7,8,
        1,2,3,4,5,6,7,8,
        1,2,3,4,5,6,7,8,
        1,2,3,4,5,6,7,8,
        1,2,3,4,5,6,7,8,
        1,2,3,4,5,6,7,8
    };
    memcpy((void*) outAddress, testByteArray, 64);  // 這樣ok的
    ivars->notifyArgs[0] = outAddress;
}*/
// 故意寫一些假資料，看能不能傳送給 App端 => 在Driver內部建立的Buffer，結果傳不出去????
/*
void SoftFido2UserClient::putDummyDataInNotifyFrameMemoryDesc() {
    uint64_t outAddress = 0;
    uint64_t outLength = 0;
    IOBufferMemoryDescriptor* frameMemoryDesc = nullptr;
    // TRY: 故意寫一些假資料進去
    uint8_t testByteArray[64] = {1,2,3,4,5,6,7,8,
        1,2,3,4,5,6,7,8,
        1,2,3,4,5,6,7,8,
        1,2,3,4,5,6,7,8,
        1,2,3,4,5,6,7,8,
        1,2,3,4,5,6,7,8,
        1,2,3,4,5,6,7,8,
        1,2,3,4,5,6,7,8
    };
    BufMemoryUtils::createWithBytes(testByteArray, 64, (IOMemoryDescriptor**) &frameMemoryDesc);
    ivars->notifyFrameMemoryDesc = frameMemoryDesc;
    frameMemoryDesc->Map(0, 0, 0, 0, &outAddress, &outLength);
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "notifyFrameMemoryDesc address = %llu", outAddress);
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "notifyFrameMemoryDesc length = %llu", outLength);
    dump(outAddress, outLength);
}
*/

// 利用 Output Buffer - CreateMapping 時指定 prepareForDMA 得到的位置，結果會失敗
/*
void SoftFido2UserClient::tryMappingForPrepareDMA() {
    IOMemoryMap* map = nullptr;
    kern_return_t ret = ivars->outputDescriptor->CreateMapping(kIOMemoryMapFixedAddress, segments[0].address, 0, length, 0, &map);
    if (ret == kIOReturnSuccess) {
        ivars->notifyArgs[0] = map->GetAddress();
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "outputDescriptor CreateMapping address = %llu", map->GetAddress());
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "outputDescriptor CreateMapping length = %llu", map->GetLength());
    } else {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "outputDescriptor CreateMapping failed = %d", ret);
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "outputDescriptor CreateMapping system err = %x", err_get_system(ret));
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "outputDescriptor CreateMapping    sub err = %x", err_get_sub(ret));
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "outputDescriptor CreateMapping   code err = %x", err_get_code(ret));
    }
 
 // 這個也是失敗
 uint64_t outAddress1 = 0;
 uint64_t outLength1 = 0;
 ret = ivars->outputDescriptor->Map(kIOMemoryMapFixedAddress,
                                    segments[0].address, segments[0].length,
                                    0,
                                    &outAddress1, &outLength1);
 if (ret == kIOReturnSuccess) {
     os_log(OS_LOG_DEFAULT, LOG_PREFIX "outputDescriptor Map Fixed address = %llu", outAddress1);
     os_log(OS_LOG_DEFAULT, LOG_PREFIX "outputDescriptor Map length = %llu", outLength1);
     //memcpy((void*) outAddress, (void*) segments[0].address, segments[0].length);
     ivars->notifyArgs[0] = outAddress1;
 } else {
     os_log(OS_LOG_DEFAULT, LOG_PREFIX "outputDescriptor Map failed = %d", ret);
     os_log(OS_LOG_DEFAULT, LOG_PREFIX "outputDescriptor Map system err = %x", err_get_system(ret));
     os_log(OS_LOG_DEFAULT, LOG_PREFIX "outputDescriptor Map    sub err = %x", err_get_sub(ret));
     os_log(OS_LOG_DEFAULT, LOG_PREFIX "outputDescriptor Map   code err = %x", err_get_code(ret));
 }
}
*/

// 試 IODMACommand，都是 Driver停止
/*
void SoftFido2UserClient::tryIODMACommand() {
    // SPEC 傳null也是
    IODMACommandSpecification spec;
    spec.maxAddressBits = 64;
    IODMACommand* dmaCmd = nullptr;
    ret = IODMACommand::Create(ivars->fido2Device, 0, &spec, &dmaCmd);
    if (dmaCmd != nullptr) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODMACommand::Create Success");
    } else {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODMACommand::Create failed = %d", ret);
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODMACommand::Create system err = %x", err_get_system(ret));
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODMACommand::Create    sub err = %x", err_get_sub(ret));
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODMACommand::Create   code err = %x", err_get_code(ret));
    }
}
*/

/* Get Physical Address Only...
void SoftFido2UserClient::tryPrepareForDMA {
    // <<< PrepareForDMA >>>
    uint64_t flags = 0;
    uint64_t dmaLength = 0;
    uint32_t dmaSegmentCount = 1;
    IOAddressSegment segments[32];
    // 可以得到 1個 block 64 bytes
    ret = report->PrepareForDMA(0, ivars->fido2Device, 0, 0, &flags, &dmaLength, &dmaSegmentCount, segments);
    if (ret != kIOReturnSuccess) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->PrepareForDMA Failed!");
        //return ret;
    } else {
        // flags = 2 (kIOMemoryDirectionOut)
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->PrepareForDMA flags = %llu", flags);
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->PrepareForDMA Length = %llu", dmaLength);
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->PrepareForDMA SegmentCount = %u", dmaSegmentCount);
        if (dmaSegmentCount > 0) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->PrepareForDMA segments.address = %llu", segments[0].address);
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->PrepareForDMA segments.length = %llu", segments[0].length);
            // 注意：直接存取 segments[0].address，會導致Driver停止
        }
    }
}*/
