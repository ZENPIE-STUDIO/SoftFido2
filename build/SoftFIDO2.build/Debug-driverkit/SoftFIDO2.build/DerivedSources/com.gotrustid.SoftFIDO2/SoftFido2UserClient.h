/* iig(DriverKit-73.100.4) generated from SoftFido2UserClient.iig */

/* SoftFido2UserClient.iig:1-15 */
//
//  SoftFido2UserClient.iig
//  SoftFIDO2
//
//  Created by Eddie Hua on 2020/9/2.
//  Copyright © 2020 GoTrustID. All rights reserved.
//

#ifndef SoftFido2UserClient_h
#define SoftFido2UserClient_h

#include <Availability.h>
#include <DriverKit/IOUserClient.h>  /* .iig include */


/* source class SoftFido2UserClient SoftFido2UserClient.iig:16-29 */

#if __DOCUMENTATION__
#define KERNEL IIG_KERNEL

class SoftFido2UserClient: public IOUserClient
{
public:
    virtual bool init() override;
    virtual void free() override;

    virtual kern_return_t Start(IOService* provider) override;
    virtual kern_return_t Stop(IOService* provider) override;

    virtual kern_return_t ExternalMethod(uint64_t selector,
                                         IOUserClientMethodArguments* arguments,
                                         const IOUserClientMethodDispatch* dispatch,
                                         OSObject* target,
                                         void* reference) override;
};

#undef KERNEL
#else /* __DOCUMENTATION__ */

/* generated class SoftFido2UserClient SoftFido2UserClient.iig:16-29 */


#define SoftFido2UserClient_Start_Args \
        IOService * provider

#define SoftFido2UserClient_Stop_Args \
        IOService * provider

#define SoftFido2UserClient_Methods \
\
public:\
\
    virtual kern_return_t\
    Dispatch(const IORPC rpc) APPLE_KEXT_OVERRIDE;\
\
    static kern_return_t\
    _Dispatch(SoftFido2UserClient * self, const IORPC rpc);\
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
\
public:\
    /* _Invoke methods */\
\


#define SoftFido2UserClient_KernelMethods \
\
protected:\
    /* _Impl methods */\
\


#define SoftFido2UserClient_VirtualMethods \
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
    ExternalMethod(\
        uint64_t selector,\
        IOUserClientMethodArguments * arguments,\
        const IOUserClientMethodDispatch * dispatch,\
        OSObject * target,\
        void * reference) APPLE_KEXT_OVERRIDE;\
\


#if !KERNEL

extern OSMetaClass          * gSoftFido2UserClientMetaClass;
extern const OSClassLoadInformation SoftFido2UserClient_Class;

class SoftFido2UserClientMetaClass : public OSMetaClass
{
public:
    virtual kern_return_t
    New(OSObject * instance) override;
    virtual kern_return_t
    Dispatch(const IORPC rpc) override;
};

#endif /* !KERNEL */

#if !KERNEL

class SoftFido2UserClientInterface : public OSInterface
{
public:
};

struct SoftFido2UserClient_IVars;
struct SoftFido2UserClient_LocalIVars;

class SoftFido2UserClient : public IOUserClient, public SoftFido2UserClientInterface
{
#if !KERNEL
    friend class SoftFido2UserClientMetaClass;
#endif /* !KERNEL */

#if !KERNEL
public:
    union
    {
        SoftFido2UserClient_IVars * ivars;
        SoftFido2UserClient_LocalIVars * lvars;
    };
#endif /* !KERNEL */

    using super = IOUserClient;

#if !KERNEL
    SoftFido2UserClient_Methods
    SoftFido2UserClient_VirtualMethods
#endif /* !KERNEL */

};
#endif /* !KERNEL */


#endif /* !__DOCUMENTATION__ */

/* SoftFido2UserClient.iig:31- */

#endif /* SoftFido2UserClient_h */
