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

inline kern_return_t createWithBytes(const void* bytes, size_t length, uint64_t options, IOMemoryDescriptor** memory) {
    if (!bytes || !memory) {
        return kIOReturnBadArgument;
    }

    IOBufferMemoryDescriptor* m = nullptr;
    auto kr = IOBufferMemoryDescriptor::Create(options, length, 0, &m);
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
    // ----------
    kern_return_t createMemoryDescriptorFromData(OSData* data, uint64_t options, IOMemoryDescriptor** memory) {
        if (!memory) {
            return kIOReturnBadArgument;
        }
        return createWithBytes(data->getBytesNoCopy(),
                               data->getLength(),
                               options,
                               memory);
    }
}

#endif /* BufMemoryUtils_h */
