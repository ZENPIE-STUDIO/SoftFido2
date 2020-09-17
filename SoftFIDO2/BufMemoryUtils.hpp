//
//  BufMemoryUtils.hpp
//  SoftFIDO2
//
//  Created by Eddie Hua on 2020/9/17.
//  Copyright © 2020 GoTrustID. All rights reserved.
//

#ifndef BufMemoryUtils_h
#define BufMemoryUtils_h

#include <DriverKit/DriverKit.h>
#include <DriverKit/IOBufferMemoryDescriptor.h>
#include <DriverKit/OSCollections.h>
#include <DriverKit/IOMemoryDescriptor.h>

namespace BufMemoryUtils {

inline kern_return_t createWithBytes(const void* bytes, size_t length, IOMemoryDescriptor** memory) {
    if (!bytes || !memory) {
        return kIOReturnBadArgument;
    }

    IOBufferMemoryDescriptor* m = nullptr;
    auto kr = IOBufferMemoryDescriptor::Create(kIOMemoryDirectionOut, length, 0, &m);
    if (kr == kIOReturnSuccess) {
        uint64_t address;
        uint64_t len;
        m->Map(0, 0, 0, 0, &address, &len);

        if (length != len) {
            kr = kIOReturnNoMemory;
            goto error;
        }
        memcpy(reinterpret_cast<void*>(address), bytes, length);
    }
    *memory = m;
    return kr;
error:
    if (m) {
        OSSafeReleaseNULL(m);
    }
    return kr;
}
    
kern_return_t createIOMemoryDescriptor(IOUserClientMethodArguments* arguments, IOMemoryDescriptor** memory) {
    if (!memory) {
        return kIOReturnBadArgument;
    }
    *memory = nullptr;

    if (arguments->structureInput) {
        auto kr = createWithBytes(arguments->structureInput->getBytesNoCopy(),
                                                               arguments->structureInput->getLength(),
                                                               memory);
        if (kr != kIOReturnSuccess) {
            return kr;
        }
    } else if (arguments->structureInputDescriptor) {
        *memory = arguments->structureInputDescriptor;
        (*memory)->retain();
    }
    return kIOReturnSuccess;
}

}

#endif /* BufMemoryUtils_h */
