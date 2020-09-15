//
//  SoftFido2Device.cpp
//  SoftFIDO2
//
//  Created by Eddie Hua on 2020/9/2.
//  Copyright © 2020 GoTrustID. All rights reserved.
//
#include <os/log.h>

#include <DriverKit/IOLib.h>
#include <DriverKit/OSCollections.h>
#include <HIDDriverKit/IOHIDInterface.h>
#include <HIDDriverKit/IOHIDDeviceKeys.h>
#include <HIDDriverKit/IOHIDUsageTables.h>
#include "u2f_hid.h"
#include "UserKernelShared.h"
#include "SoftFido2Device.h"
#include "SoftFido2UserClient.h"


#define LOG_PREFIX "[SoftFido2][Device] "

struct SoftFido2Device_IVars {
    SoftFido2UserClient* provider;
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
//kern_return_t SoftFido2Device::handleReport(uint64_t timestamp,
//                                            IOMemoryDescriptor * report,
//                                            uint32_t reportLength,
//                                            IOHIDReportType reportType,
//                                            IOOptionBits options) {
//    kern_return_t ret = kIOReturnSuccess;
//    os_log(OS_LOG_DEFAULT, LOG_PREFIX "handleReport");
//    return ret;
//}
//
//kern_return_t SoftFido2Device::getReport(IOMemoryDescriptor * report,
//                                        IOHIDReportType reportType,
//                                        IOOptionBits options,
//                                        uint32_t completionTimeout,
//                                        OSAction * action) {
//    kern_return_t ret = kIOReturnSuccess;
//    os_log(OS_LOG_DEFAULT, LOG_PREFIX "getReport");
//    return ret;
//}

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
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "setReport completionTimeout = %u", completionTimeout);
    // 用 User Client 去處理接收到的資料
    SoftFido2UserClient *userClient = ivars->provider;
    if (userClient != nullptr) {
        ret = userClient->frameReceived(report, action);
    }
    // Sleep for a bit to make the HID conformance tests happy.
    IOSleep(1); // 1ms
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
    //ivars->provider-> 不用 setReady
    return true;
}

OSDictionary* SoftFido2Device::newDeviceDescription(void) {
    os_log(OS_LOG_DEFAULT, LOG_PREFIX "newDeviceDescription");
    auto dictionary = OSDictionary::withCapacity(10);
    if (!dictionary) {
        os_log(OS_LOG_DEFAULT, LOG_PREFIX "OSDictionary::withCapacity failed");
        return nullptr;
    }

    // Set kIOHIDRegisterServiceKey in order to call registerService in IOHIDDevice::start.
    OSDictionarySetValue(dictionary, "RegisterService", kOSBooleanTrue);
//    OSDictionarySetValue(dictionary, "HIDDefaultBehavior", kOSBooleanTrue);
//    OSDictionarySetValue(dictionary, "AppleVendorSupported", kOSBooleanTrue);
    
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
