/* iig(DriverKit-73.100.4) generated from SoftFido2Driver.iig */

/* SoftFido2Driver.iig:1-16 */
//
//  SoftFIDO2.iig
//  SoftFIDO2
//
//  Created by Eddie Hua on 2020/9/1.
//  Copyright © 2020 GoTrustID. All rights reserved.
//

#ifndef SoftFIDO2_h
#define SoftFIDO2_h

#include <Availability.h>
#include <DriverKit/IOService.h>  /* .iig include */

class IOUserClient;

/* source class SoftFido2Driver SoftFido2Driver.iig:17-33 */

#if __DOCUMENTATION__
#define KERNEL IIG_KERNEL

class SoftFido2Driver: public IOService
{
    IODispatchQueue* mWorkQueue = nullptr;
    //IOWorkLoop *_workLoop = nullptr;    // 用 IODispatchQueue 取代
    //   (START) workLoop->addEventSource(_commandGate)
    //   (STOP) workLoop->removeEventSource(_commandGate);
    
public:
    virtual bool init() override;
    virtual void free() override;
    virtual kern_return_t Start(IOService * provider) override;
    
    virtual kern_return_t Stop(IOService *provider) override;

    virtual kern_return_t NewUserClient(uint32_t type,
                                        IOUserClient **userClient) override;
    //
};

#undef KERNEL
#else /* __DOCUMENTATION__ */

/* generated class SoftFido2Driver SoftFido2Driver.iig:17-33 */


#define SoftFido2Driver_Start_Args \
        IOService * provider

#define SoftFido2Driver_Stop_Args \
        IOService * provider

#define SoftFido2Driver_NewUserClient_Args \
        uint32_t type, \
        IOUserClient ** userClient

#define SoftFido2Driver_Methods \
\
public:\
\
    virtual kern_return_t\
    Dispatch(const IORPC rpc) APPLE_KEXT_OVERRIDE;\
\
    static kern_return_t\
    _Dispatch(SoftFido2Driver * self, const IORPC rpc);\
\
\
protected:\
    /* _Impl methods */\
\
    kern_return_t\
    Start_Impl(IOService_Start_Args);\
\
    kern_return_t\
    Stop_Impl(IOService_Stop_Args);\
\
    kern_return_t\
    NewUserClient_Impl(IOService_NewUserClient_Args);\
\
\
public:\
    /* _Invoke methods */\
\


#define SoftFido2Driver_KernelMethods \
\
protected:\
    /* _Impl methods */\
\


#define SoftFido2Driver_VirtualMethods \
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


#if !KERNEL

extern OSMetaClass          * gSoftFido2DriverMetaClass;
extern const OSClassLoadInformation SoftFido2Driver_Class;

class SoftFido2DriverMetaClass : public OSMetaClass
{
public:
    virtual kern_return_t
    New(OSObject * instance) override;
    virtual kern_return_t
    Dispatch(const IORPC rpc) override;
};

#endif /* !KERNEL */

#if !KERNEL

class SoftFido2DriverInterface : public OSInterface
{
public:
};

struct SoftFido2Driver_IVars;
struct SoftFido2Driver_LocalIVars;

class SoftFido2Driver : public IOService, public SoftFido2DriverInterface
{
#if !KERNEL
    friend class SoftFido2DriverMetaClass;
#endif /* !KERNEL */

#if !KERNEL
public:
    union
    {
        SoftFido2Driver_IVars * ivars;
        SoftFido2Driver_LocalIVars * lvars;
    };
#endif /* !KERNEL */

    using super = IOService;

#if !KERNEL
    SoftFido2Driver_Methods
    SoftFido2Driver_VirtualMethods
#endif /* !KERNEL */

};
#endif /* !KERNEL */


#endif /* !__DOCUMENTATION__ */

/* SoftFido2Driver.iig:35- */

#endif /* SoftFIDO2_h */
