/* iig(DriverKit-73.100.4) generated from SoftFido2Driver.iig */

#undef	IIG_IMPLEMENTATION
#define	IIG_IMPLEMENTATION 	SoftFido2Driver.iig

#include <DriverKit/IOReturn.h>
#include "SoftFido2Driver.h"


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
extern OSMetaClass * gOSActionMetaClass;
extern OSMetaClass * gIOUserClientMetaClass;
#endif /* !KERNEL */

#if !KERNEL

#define SoftFido2Driver_QueueNames  ""

#define SoftFido2Driver_MethodNames  ""

#define SoftFido2DriverMetaClass_MethodNames  ""

struct OSClassDescription_SoftFido2Driver_t
{
    OSClassDescription base;
    uint64_t           methodOptions[2 * 0];
    uint64_t           metaMethodOptions[2 * 0];
    char               queueNames[sizeof(SoftFido2Driver_QueueNames)];
    char               methodNames[sizeof(SoftFido2Driver_MethodNames)];
    char               metaMethodNames[sizeof(SoftFido2DriverMetaClass_MethodNames)];
};

const struct OSClassDescription_SoftFido2Driver_t
OSClassDescription_SoftFido2Driver =
{
    .base =
    {
        .descriptionSize         = sizeof(OSClassDescription_SoftFido2Driver_t),
        .name                    = "SoftFido2Driver",
        .superName               = "IOService",
        .flags                   = 0*kOSClassCanRemote,
        .methodOptionsSize       = 2 * sizeof(uint64_t) * 0,
        .metaMethodOptionsSize   = 2 * sizeof(uint64_t) * 0,
        .queueNamesSize       = sizeof(SoftFido2Driver_QueueNames),
        .methodNamesSize         = sizeof(SoftFido2Driver_MethodNames),
        .metaMethodNamesSize     = sizeof(SoftFido2DriverMetaClass_MethodNames),
        .methodOptionsOffset     = __builtin_offsetof(struct OSClassDescription_SoftFido2Driver_t, methodOptions),
        .metaMethodOptionsOffset = __builtin_offsetof(struct OSClassDescription_SoftFido2Driver_t, metaMethodOptions),
        .queueNamesOffset     = __builtin_offsetof(struct OSClassDescription_SoftFido2Driver_t, queueNames),
        .methodNamesOffset       = __builtin_offsetof(struct OSClassDescription_SoftFido2Driver_t, methodNames),
        .metaMethodNamesOffset   = __builtin_offsetof(struct OSClassDescription_SoftFido2Driver_t, metaMethodNames),
    },
    .methodOptions =
    {
    },
    .metaMethodOptions =
    {
    },
    .queueNames      = SoftFido2Driver_QueueNames,
    .methodNames     = SoftFido2Driver_MethodNames,
    .metaMethodNames = SoftFido2DriverMetaClass_MethodNames,
};

OSMetaClass * gSoftFido2DriverMetaClass;

static kern_return_t
SoftFido2Driver_New(OSMetaClass * instance);

const OSClassLoadInformation
SoftFido2Driver_Class = 
{
    .version           = 1,
    .description       = &OSClassDescription_SoftFido2Driver.base,
    .instanceSize      = sizeof(SoftFido2Driver),

    .metaPointer       = &gSoftFido2DriverMetaClass,
    .New               = &SoftFido2Driver_New,
};

extern const void * const
gSoftFido2Driver_Declaration;
const void * const
gSoftFido2Driver_Declaration
__attribute__((visibility("hidden"),section("__DATA_CONST,__osclassinfo,regular,no_dead_strip")))
    = &SoftFido2Driver_Class;

static kern_return_t
SoftFido2Driver_New(OSMetaClass * instance)
{
    if (!new(instance) SoftFido2DriverMetaClass) return (kIOReturnNoMemory);
    return (kIOReturnSuccess);
}

kern_return_t
SoftFido2DriverMetaClass::New(OSObject * instance)
{
    if (!new(instance) SoftFido2Driver) return (kIOReturnNoMemory);
    return (kIOReturnSuccess);
}

#endif /* !KERNEL */

kern_return_t
SoftFido2Driver::Dispatch(const IORPC rpc)
{
    return _Dispatch(this, rpc);
}

kern_return_t
SoftFido2Driver::_Dispatch(SoftFido2Driver * self, const IORPC rpc)
{
    kern_return_t ret = kIOReturnUnsupported;
    IORPCMessage * msg = IORPCMessageFromMach(rpc.message, false);

    switch (msg->msgid)
    {
        case IOService_Start_ID:
        {
            union {
                kern_return_t (SoftFido2Driver::*fIn)(IOService_Start_Args);
                IOService::Start_Handler handler;
            } map;
            map.fIn = &SoftFido2Driver::Start_Impl;
            ret = IOService::Start_Invoke(rpc, self, map.handler);
            break;
        }
        case IOService_Stop_ID:
        {
            union {
                kern_return_t (SoftFido2Driver::*fIn)(IOService_Stop_Args);
                IOService::Stop_Handler handler;
            } map;
            map.fIn = &SoftFido2Driver::Stop_Impl;
            ret = IOService::Stop_Invoke(rpc, self, map.handler);
            break;
        }
        case IOService_NewUserClient_ID:
        {
            union {
                kern_return_t (SoftFido2Driver::*fIn)(IOService_NewUserClient_Args);
                IOService::NewUserClient_Handler handler;
            } map;
            map.fIn = &SoftFido2Driver::NewUserClient_Impl;
            ret = IOService::NewUserClient_Invoke(rpc, self, map.handler);
            break;
        }

        default:
            ret = IOService::_Dispatch(self, rpc);
            break;
    }

    return (ret);
}

#if KERNEL
kern_return_t
SoftFido2Driver::MetaClass::Dispatch(const IORPC rpc)
{
#else /* KERNEL */
kern_return_t
SoftFido2DriverMetaClass::Dispatch(const IORPC rpc)
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



