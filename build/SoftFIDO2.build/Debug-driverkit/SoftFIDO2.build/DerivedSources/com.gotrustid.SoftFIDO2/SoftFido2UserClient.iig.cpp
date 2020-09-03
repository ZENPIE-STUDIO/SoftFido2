/* iig(DriverKit-73.100.4) generated from SoftFido2UserClient.iig */

#undef	IIG_IMPLEMENTATION
#define	IIG_IMPLEMENTATION 	SoftFido2UserClient.iig

#include <DriverKit/IOReturn.h>
#include "SoftFido2UserClient.h"


#if !KERNEL
extern OSMetaClass * gOSContainerMetaClass;
extern OSMetaClass * gOSDataMetaClass;
extern OSMetaClass * gOSNumberMetaClass;
extern OSMetaClass * gOSStringMetaClass;
extern OSMetaClass * gOSBooleanMetaClass;
extern OSMetaClass * gOSDictionaryMetaClass;
extern OSMetaClass * gOSArrayMetaClass;
extern OSMetaClass * gIODispatchQueueMetaClass;
extern OSMetaClass * gIOMemoryDescriptorMetaClass;
extern OSMetaClass * gIOBufferMemoryDescriptorMetaClass;
#endif /* !KERNEL */

#if !KERNEL

#define SoftFido2UserClient_QueueNames  ""

#define SoftFido2UserClient_MethodNames  ""

#define SoftFido2UserClientMetaClass_MethodNames  ""

struct OSClassDescription_SoftFido2UserClient_t
{
    OSClassDescription base;
    uint64_t           methodOptions[2 * 0];
    uint64_t           metaMethodOptions[2 * 0];
    char               queueNames[sizeof(SoftFido2UserClient_QueueNames)];
    char               methodNames[sizeof(SoftFido2UserClient_MethodNames)];
    char               metaMethodNames[sizeof(SoftFido2UserClientMetaClass_MethodNames)];
};

const struct OSClassDescription_SoftFido2UserClient_t
OSClassDescription_SoftFido2UserClient =
{
    .base =
    {
        .descriptionSize         = sizeof(OSClassDescription_SoftFido2UserClient_t),
        .name                    = "SoftFido2UserClient",
        .superName               = "IOUserClient",
        .flags                   = 0*kOSClassCanRemote,
        .methodOptionsSize       = 2 * sizeof(uint64_t) * 0,
        .metaMethodOptionsSize   = 2 * sizeof(uint64_t) * 0,
        .queueNamesSize       = sizeof(SoftFido2UserClient_QueueNames),
        .methodNamesSize         = sizeof(SoftFido2UserClient_MethodNames),
        .metaMethodNamesSize     = sizeof(SoftFido2UserClientMetaClass_MethodNames),
        .methodOptionsOffset     = __builtin_offsetof(struct OSClassDescription_SoftFido2UserClient_t, methodOptions),
        .metaMethodOptionsOffset = __builtin_offsetof(struct OSClassDescription_SoftFido2UserClient_t, metaMethodOptions),
        .queueNamesOffset     = __builtin_offsetof(struct OSClassDescription_SoftFido2UserClient_t, queueNames),
        .methodNamesOffset       = __builtin_offsetof(struct OSClassDescription_SoftFido2UserClient_t, methodNames),
        .metaMethodNamesOffset   = __builtin_offsetof(struct OSClassDescription_SoftFido2UserClient_t, metaMethodNames),
    },
    .methodOptions =
    {
    },
    .metaMethodOptions =
    {
    },
    .queueNames      = SoftFido2UserClient_QueueNames,
    .methodNames     = SoftFido2UserClient_MethodNames,
    .metaMethodNames = SoftFido2UserClientMetaClass_MethodNames,
};

OSMetaClass * gSoftFido2UserClientMetaClass;

static kern_return_t
SoftFido2UserClient_New(OSMetaClass * instance);

const OSClassLoadInformation
SoftFido2UserClient_Class = 
{
    .version           = 1,
    .description       = &OSClassDescription_SoftFido2UserClient.base,
    .instanceSize      = sizeof(SoftFido2UserClient),

    .metaPointer       = &gSoftFido2UserClientMetaClass,
    .New               = &SoftFido2UserClient_New,
};

extern const void * const
gSoftFido2UserClient_Declaration;
const void * const
gSoftFido2UserClient_Declaration
__attribute__((visibility("hidden"),section("__DATA_CONST,__osclassinfo,regular,no_dead_strip")))
    = &SoftFido2UserClient_Class;

static kern_return_t
SoftFido2UserClient_New(OSMetaClass * instance)
{
    if (!new(instance) SoftFido2UserClientMetaClass) return (kIOReturnNoMemory);
    return (kIOReturnSuccess);
}

kern_return_t
SoftFido2UserClientMetaClass::New(OSObject * instance)
{
    if (!new(instance) SoftFido2UserClient) return (kIOReturnNoMemory);
    return (kIOReturnSuccess);
}

#endif /* !KERNEL */

kern_return_t
SoftFido2UserClient::Dispatch(const IORPC rpc)
{
    return _Dispatch(this, rpc);
}

kern_return_t
SoftFido2UserClient::_Dispatch(SoftFido2UserClient * self, const IORPC rpc)
{
    kern_return_t ret = kIOReturnUnsupported;
    IORPCMessage * msg = IORPCMessageFromMach(rpc.message, false);

    switch (msg->msgid)
    {
        case IOService_Start_ID:
        {
            union {
                kern_return_t (SoftFido2UserClient::*fIn)(IOService_Start_Args);
                IOService::Start_Handler handler;
            } map;
            map.fIn = &SoftFido2UserClient::Start_Impl;
            ret = IOService::Start_Invoke(rpc, self, map.handler);
            break;
        }
        case IOService_Stop_ID:
        {
            union {
                kern_return_t (SoftFido2UserClient::*fIn)(IOService_Stop_Args);
                IOService::Stop_Handler handler;
            } map;
            map.fIn = &SoftFido2UserClient::Stop_Impl;
            ret = IOService::Stop_Invoke(rpc, self, map.handler);
            break;
        }

        default:
            ret = IOUserClient::_Dispatch(self, rpc);
            break;
    }

    return (ret);
}

#if KERNEL
kern_return_t
SoftFido2UserClient::MetaClass::Dispatch(const IORPC rpc)
{
#else /* KERNEL */
kern_return_t
SoftFido2UserClientMetaClass::Dispatch(const IORPC rpc)
{
#endif /* !KERNEL */

    kern_return_t ret = kIOReturnUnsupported;
    IORPCMessage * msg = IORPCMessageFromMach(rpc.message, false);

    switch (msg->msgid)
    {

        default:
            ret = OSMetaClassBase::Dispatch(rpc);
            break;
    }

    return (ret);
}



