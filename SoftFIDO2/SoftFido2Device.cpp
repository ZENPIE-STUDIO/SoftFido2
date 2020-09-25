//
//  SoftFido2Device.cpp
//  SoftFIDO2
//
//  Created by Eddie Hua on 2020/9/2.
//  Copyright © 2020 GoTrustID. All rights reserved.
//
#include <os/log.h>

#include <DriverKit/IODMACommand.h>
#include <DriverKit/IOLib.h>
#include <DriverKit/IOMemoryDescriptor.h>
#include <DriverKit/IOMemoryMap.h>
#include <DriverKit/OSCollections.h>
#include <HIDDriverKit/IOHIDInterface.h>
#include <HIDDriverKit/IOHIDDeviceKeys.h>
#include <HIDDriverKit/IOHIDUsageTables.h>
#include <USBDriverKit/IOUSBHostInterface.h>
#include "u2f_hid.h"
#include "UserKernelShared.h"
#include "SoftFido2Device.h"
#include "SoftFido2UserClient.h"


#define LOG_PREFIX "[SoftFido2][Device] "

struct SoftFido2Device_IVars {
    SoftFido2UserClient*    provider;
    IOUSBHostInterface*     interface;
};

bool SoftFido2Device::init() {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "init");
    if (super::init()) {
        ivars = IONewZero(SoftFido2Device_IVars, 1);
        return (ivars != nullptr);
    }
    return false;
}

void SoftFido2Device::free() {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "free");
    IOSafeDeleteNULL(ivars, SoftFido2Device_IVars, 1);
    super::free();
}

#pragma mark - IOHIDDevice
kern_return_t SoftFido2Device::handleReport(uint64_t timestamp,
                                            IOMemoryDescriptor * report,
                                            uint32_t reportLength,
                                            IOHIDReportType reportType,
                                            IOOptionBits options) {
    kern_return_t ret = kIOReturnSuccess;
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "handleReport");
    return ret;
}

//kern_return_t SoftFido2Device::getReport(IOMemoryDescriptor * report,
//                                        IOHIDReportType reportType,
//                                        IOOptionBits options,
//                                        uint32_t completionTimeout,
//                                        OSAction * action) {
//    kern_return_t ret = kIOReturnSuccess;
//    os_log(OS_LOG_DEFAULT, LOG_PREFIX "getReport");
//    return ret;
//}

void tryIODMACommand(SoftFido2Device* device, IOMemoryDescriptor* report) {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "tryIODMACommand");
    // <<< IODMACommand >>>
    IODMACommandSpecification spec;
    spec.maxAddressBits = 64;
    IODMACommand* dmaCmd = nullptr;
    kern_return_t ret = IODMACommand::Create(device, 0, &spec, &dmaCmd);
    if (ret == kIOReturnSuccess) {
        uint64_t flags = 0;
        uint32_t dmaSegmentCount = 1;
        IOAddressSegment segments[32];
        dmaCmd->PrepareForDMA(0, report, 0, 0, &flags, &dmaSegmentCount, segments);
        if (ret == kIOReturnSuccess) {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODMACommand PrepareForDMA flags = %llu", flags);
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODMACommand PrepareForDMA SegmentCount = %u", dmaSegmentCount);

            uint64_t offset = 0;
            uint64_t length = 0;
            IOMemoryDescriptor* testMemDesc = nullptr;
            ret = dmaCmd->GetPreparation(&offset, &length, &testMemDesc);
            if (ret == kIOReturnSuccess) {
                os_log(OS_LOG_DEFAULT, LOG_PREFIX "  GetPreparation offset = %llu", offset);
                os_log(OS_LOG_DEFAULT, LOG_PREFIX "  GetPreparation length = %llu", length);
            } else {
                os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODMACommand GetPreparation failed = %d", ret);
                os_log(OS_LOG_DEFAULT, LOG_PREFIX "       system err = %x", err_get_system(ret));
                os_log(OS_LOG_DEFAULT, LOG_PREFIX "          sub err = %x", err_get_sub(ret));
                os_log(OS_LOG_DEFAULT, LOG_PREFIX "         code err = %x", err_get_code(ret));
            }
            
            ret = dmaCmd->CompleteDMA(0);
            if (ret == kIOReturnSuccess) {
                os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODMACommand CompleteDMA Success");
            } else {
                os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODMACommand CompleteDMA failed = %d", ret);
            }
        } else {
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "IODMACommand PrepareForDMA failed = %d", ret);
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "       system err = %x", err_get_system(ret));
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "          sub err = %x", err_get_sub(ret));
            os_log(OS_LOG_DEFAULT, LOG_PREFIX "         code err = %x", err_get_code(ret));
        }
    }
}

kern_return_t SoftFido2Device::setReport(IOMemoryDescriptor* report,
                                         IOHIDReportType reportType,
                                         IOOptionBits options,
                                         uint32_t completionTimeout,
                                         OSAction* action) {
    kern_return_t ret = kIOReturnSuccess;
    switch (reportType) {
        case kIOHIDReportTypeInput: os_log(OS_LOG_DEFAULT, LOG_PREFIX "setReport (Input)"); break;
        case kIOHIDReportTypeOutput: os_log(OS_LOG_DEFAULT, LOG_PREFIX "setReport (Output)"); break;
        case kIOHIDReportTypeFeature: os_log(OS_LOG_DEFAULT, LOG_PREFIX "setReport (Feature)"); break;
        case kIOHIDReportTypeCount: os_log(OS_LOG_DEFAULT, LOG_PREFIX "setReport (Count)"); break;
    }
    //tryIODMACommand(this, report);
    // 心得：結果這裡options=0
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "setReport IOOptionBits = %u", options);
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "setReport completionTimeout = %u", completionTimeout);
    _IOMDPrivateState state;
    ret = report->_CopyState(&state);
    if (ret == kIOReturnSuccess) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "state.length = %llu", state.length);
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "state.options = %llu", state.options);
        // state.options = 33050
        //    可知 kIOMemoryPersistent | kIOMemoryMapCopyOnWrite
        //          kIOMemoryHostOnly | 2000(_kIOMemorySourceSegment?)
        //          kIOMemoryTypeUIO
    } else {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "_CopyState failed = %d", ret);
    }
    // 實驗：IOBufferMemoryDescriptor 建立時，無論用什麼參數 In/Out，都可 Map address = 4439891968
    // (怪) 用 Map, CreateMapping 都失敗
    //uint64_t address;
    uint64_t length;
    IOMemoryMap* map = nullptr;
    ret = report->CreateMapping(kIOMemoryMapReadOnly, 0, 0, 0, 0, &map);
    //ret = report->Map(0, 0, 0, 0, &address, &length);
    if (ret == kIOReturnSuccess) {
        //os_log(OS_LOG_DEFAULT, LOG_PREFIX "address = %llu", address);
        //os_log(OS_LOG_DEFAULT, LOG_PREFIX "length = %llu", length);
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "address = %llu", map->GetAddress());
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "length = %llu", map->GetLength());
    } else {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "report->Map failed = %d", ret);
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "       system err = %x", err_get_system(ret));
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "          sub err = %x", err_get_sub(ret));
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "         code err = %x", err_get_code(ret));
    }
    // 用 User Client 去處理接收到的資料
    SoftFido2UserClient *userClient = ivars->provider;
    if (userClient != nullptr) {
        ret = userClient->frameReceived(report, action);
    }
    // Sleep for a bit to make the HID conformance tests happy.
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "setReport - CompleteReport");
    report->GetLength(&length);
    CompleteReport(action, kIOReturnSuccess, (uint32_t) length); // 沒有這個，LOG會有
    // SoftFido2Device:0x1000008a6 Action aborted 0 1
    // SoftFido2Device:0x1000008a6 ProcessReport:0xe00002eb 1 0
    IOSleep(1); // 1ms
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "setReport - return %d", ret);
    return ret;
}

#pragma mark - IOHIDDeviceInterface
bool SoftFido2Device::handleStart(IOService* provider) {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "handleStart");
    ivars->provider = OSDynamicCast(SoftFido2UserClient, provider);
    if (!ivars->provider) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "handleStart > provider is not SoftFido2UserClient");
        return false;
    }
    
    if (!super::handleStart(provider)) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "handleStart > super::handleStart failed");
        return false;
    }
    // IOUSBHostInterface 可以幹什麼？
    //ivars->interface = OSDynamicCast(IOUSBHostInterface, provider);
    return true;
}

OSDictionary* SoftFido2Device::newDeviceDescription(void) {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "newDeviceDescription");
    auto dictionary = OSDictionary::withCapacity(12);
    if (!dictionary) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "OSDictionary::withCapacity failed");
        return nullptr;
    }

    // Set kIOHIDRegisterServiceKey in order to call registerService in IOHIDDevice::start.
    OSDictionarySetValue(dictionary, "RegisterService", kOSBooleanTrue);
    OSDictionarySetValue(dictionary, "HIDDefaultBehavior", kOSBooleanTrue);
    OSDictionarySetValue(dictionary, "AppleVendorSupported", kOSBooleanTrue);
    
    if (auto usagePage = OSNumber::withNumber(static_cast<uint32_t>(FIDO_USAGE_PAGE), 32)) {
        OSDictionarySetValue(dictionary, kIOHIDPrimaryUsagePageKey, usagePage);
        usagePage->release();
    }

    if (auto usage = OSNumber::withNumber(static_cast<uint32_t>(FIDO_USAGE_FIDOHID), 32)) {
        OSDictionarySetValue(dictionary, kIOHIDPrimaryUsageKey, usage);
        usage->release();
    }
    if (auto manufacturer = OSString::withCString("GoTrustID Inc.")) {
        OSDictionarySetValue(dictionary, kIOHIDManufacturerKey, manufacturer);
        manufacturer->release();
    }
    // Product Key
    if (auto product = OSString::withCString("SoftFido2Driver (DriverKit)")) {
        OSDictionarySetValue(dictionary, kIOHIDProductKey, product);
        product->release();
    }
    // Serial Number
    if (auto serialNumber = OSString::withCString("123")) {
        OSDictionarySetValue(dictionary, kIOHIDSerialNumberKey, serialNumber);
        serialNumber->release();
    }
    // Version
    if (auto version = OSNumber::withNumber(static_cast<uint32_t>(0), 32)) {
        OSDictionarySetValue(dictionary, kIOHIDVersionNumberKey, version);
        version->release();
    }
    // Vendor ID
    if (auto vendorId = OSNumber::withNumber(static_cast<uint32_t>(12963), 32)) {
        OSDictionarySetValue(dictionary, kIOHIDVendorIDKey, vendorId);
        vendorId->release();
    }
    // Product ID?
    if (auto productId = OSNumber::withNumber(static_cast<uint32_t>(20737), 32)) {
        OSDictionarySetValue(dictionary, kIOHIDProductIDKey, productId);
        productId->release();
    }
    return dictionary;
}

OSData* SoftFido2Device::newReportDescriptor(void) {
    const size_t kSizeOfReportDescriptor = sizeof(u2fhid_report_descriptor);
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "newReportDescriptor size = %lu", kSizeOfReportDescriptor);
    return OSData::withBytes(u2fhid_report_descriptor, kSizeOfReportDescriptor);
}

#pragma mark - 備份一些測試function
// 結果: 無法建立，連LOG都看不到…
