/* iig(DriverKit-73.100.4) generated from SoftFido2Device.iig */

/* SoftFido2Device.iig:1-16 */
//
//  SoftFido2Device.iig
//  SoftFIDO2
//
//  Created by Eddie Hua on 2020/9/2.
//  Copyright © 2020 GoTrustID. All rights reserved.
//

#ifndef SoftFido2Device_h
#define SoftFido2Device_h

#include <Availability.h>
#include <DriverKit/IOService.h>  /* .iig include */
#include <HIDDriverKit/IOUserHIDDevice.h>  /* .iig include */


/* source class SoftFido2Device SoftFido2Device.iig:17-36 */

#if __DOCUMENTATION__
#define KERNEL IIG_KERNEL

class SoftFido2Device: public IOUserHIDDevice
{
public:
    virtual bool init() override;
    virtual void free() override;
    
    // IOHIDDevice Virtual Method
    virtual kern_return_t setReport(IOMemoryDescriptor* report,
                                    IOHIDReportType reportType,
                                    IOOptionBits options,
                                    uint32_t completionTimeout,
                                    OSAction* action) override;

    // 裡面是呼叫 handleReport
    //virtual kern_return_t postReport(IOMemoryDescriptor* report);
    
    // IOHIDDeviceInterface Virtual Method
    virtual bool handleStart(IOService* provider) override;
    virtual OSDictionary* newDeviceDescription(void) override;
    virtual OSData* newReportDescriptor(void) override;
};

#undef KERNEL
#else /* __DOCUMENTATION__ */

/* generated class SoftFido2Device SoftFido2Device.iig:17-36 */


#define SoftFido2Device_Methods \
\
public:\
\
    virtual kern_return_t\
    Dispatch(const IORPC rpc) APPLE_KEXT_OVERRIDE;\
\
    static kern_return_t\
    _Dispatch(SoftFido2Device * self, const IORPC rpc);\
\
\
protected:\
    /* _Impl methods */\
\
\
public:\
    /* _Invoke methods */\
\


#define SoftFido2Device_KernelMethods \
\
protected:\
    /* _Impl methods */\
\


#define SoftFido2Device_VirtualMethods \
\
public:\
\
    virtual bool\
    init(\
) APPLE_KEXT_OVERRIDE;\
\
    virtual void\
    free(\
) APPLE_KEXT_OVERRIDE;\
\
    virtual kern_return_t\
    setReport(\
        IOMemoryDescriptor * report,\
        IOHIDReportType reportType,\
        IOOptionBits options,\
        uint32_t completionTimeout,\
        OSAction * action) APPLE_KEXT_OVERRIDE;\
\
    virtual bool\
    handleStart(\
        IOService * provider) APPLE_KEXT_OVERRIDE;\
\
    virtual OSDictionary *\
    newDeviceDescription(\
) APPLE_KEXT_OVERRIDE;\
\
    virtual OSData *\
    newReportDescriptor(\
) APPLE_KEXT_OVERRIDE;\
\


#if !KERNEL

extern OSMetaClass          * gSoftFido2DeviceMetaClass;
extern const OSClassLoadInformation SoftFido2Device_Class;

class SoftFido2DeviceMetaClass : public OSMetaClass
{
public:
    virtual kern_return_t
    New(OSObject * instance) override;
    virtual kern_return_t
    Dispatch(const IORPC rpc) override;
};

#endif /* !KERNEL */

#if !KERNEL

class SoftFido2DeviceInterface : public OSInterface
{
public:
};

struct SoftFido2Device_IVars;
struct SoftFido2Device_LocalIVars;

class SoftFido2Device : public IOUserHIDDevice, public SoftFido2DeviceInterface
{
#if !KERNEL
    friend class SoftFido2DeviceMetaClass;
#endif /* !KERNEL */

#if !KERNEL
public:
    union
    {
        SoftFido2Device_IVars * ivars;
        SoftFido2Device_LocalIVars * lvars;
    };
#endif /* !KERNEL */

    using super = IOUserHIDDevice;

#if !KERNEL
    SoftFido2Device_Methods
    SoftFido2Device_VirtualMethods
#endif /* !KERNEL */

};
#endif /* !KERNEL */


#endif /* !__DOCUMENTATION__ */

/* SoftFido2Device.iig:38- */


#endif /* SoftFido2Device_h */
