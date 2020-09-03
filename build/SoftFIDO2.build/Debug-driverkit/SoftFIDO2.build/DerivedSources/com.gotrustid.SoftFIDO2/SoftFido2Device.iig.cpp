/* iig(DriverKit-73.100.4) generated from SoftFido2Device.iig */

#undef	IIG_IMPLEMENTATION
#define	IIG_IMPLEMENTATION 	SoftFido2Device.iig

#include <DriverKit/IOReturn.h>
#include "SoftFido2Device.h"


#if !KERNEL
extern OSMetaClass * gOSContainerMetaClass;
extern OSMetaClass * gOSNumberMetaClass;
extern OSMetaClass * gOSStringMetaClass;
extern OSMetaClass * gOSBooleanMetaClass;
extern OSMetaClass * gOSArrayMetaClass;
extern OSMetaClass * gIODispatchQueueMetaClass;
extern OSMetaClass * gIOBufferMemoryDescriptorMetaClass;
extern OSMetaClass * gIOUserClientMetaClass;
extern OSMetaClass * gIOMemoryDescriptorMetaClass;
extern OSMetaClass * gOSDataMetaClass;
extern OSMetaClass * gOSDictionaryMetaClass;
#endif /* !KERNEL */

#if !KERNEL

#define SoftFido2Device_QueueNames  ""

#define SoftFido2Device_MethodNames  ""

#define SoftFido2DeviceMetaClass_MethodNames  ""

struct OSClassDescription_SoftFido2Device_t
{
    OSClassDescription base;
    uint64_t           methodOptions[2 * 0];
    uint64_t           metaMethodOptions[2 * 0];
    char               queueNames[sizeof(SoftFido2Device_QueueNames)];
    char               methodNames[sizeof(SoftFido2Device_MethodNames)];
    char               metaMethodNames[sizeof(SoftFido2DeviceMetaClass_MethodNames)];
};

const struct OSClassDescription_SoftFido2Device_t
OSClassDescription_SoftFido2Device =
{
    .base =
    {
        .descriptionSize         = sizeof(OSClassDescription_SoftFido2Device_t),
        .name                    = "SoftFido2Device",
        .superName               = "IOUserHIDDevice",
        .flags                   = 0*kOSClassCanRemote,
        .methodOptionsSize       = 2 * sizeof(uint64_t) * 0,
        .metaMethodOptionsSize   = 2 * sizeof(uint64_t) * 0,
        .queueNamesSize       = sizeof(SoftFido2Device_QueueNames),
        .methodNamesSize         = sizeof(SoftFido2Device_MethodNames),
        .metaMethodNamesSize     = sizeof(SoftFido2DeviceMetaClass_MethodNames),
        .methodOptionsOffset     = __builtin_offsetof(struct OSClassDescription_SoftFido2Device_t, methodOptions),
        .metaMethodOptionsOffset = __builtin_offsetof(struct OSClassDescription_SoftFido2Device_t, metaMethodOptions),
        .queueNamesOffset     = __builtin_offsetof(struct OSClassDescription_SoftFido2Device_t, queueNames),
        .methodNamesOffset       = __builtin_offsetof(struct OSClassDescription_SoftFido2Device_t, methodNames),
        .metaMethodNamesOffset   = __builtin_offsetof(struct OSClassDescription_SoftFido2Device_t, metaMethodNames),
    },
    .methodOptions =
    {
    },
    .metaMethodOptions =
    {
    },
    .queueNames      = SoftFido2Device_QueueNames,
    .methodNames     = SoftFido2Device_MethodNames,
    .metaMethodNames = SoftFido2DeviceMetaClass_MethodNames,
};

OSMetaClass * gSoftFido2DeviceMetaClass;

static kern_return_t
SoftFido2Device_New(OSMetaClass * instance);

const OSClassLoadInformation
SoftFido2Device_Class = 
{
    .version           = 1,
    .description       = &OSClassDescription_SoftFido2Device.base,
    .instanceSize      = sizeof(SoftFido2Device),

    .metaPointer       = &gSoftFido2DeviceMetaClass,
    .New               = &SoftFido2Device_New,
};

extern const void * const
gSoftFido2Device_Declaration;
const void * const
gSoftFido2Device_Declaration
__attribute__((visibility("hidden"),section("__DATA_CONST,__osclassinfo,regular,no_dead_strip")))
    = &SoftFido2Device_Class;

static kern_return_t
SoftFido2Device_New(OSMetaClass * instance)
{
    if (!new(instance) SoftFido2DeviceMetaClass) return (kIOReturnNoMemory);
    return (kIOReturnSuccess);
}

kern_return_t
SoftFido2DeviceMetaClass::New(OSObject * instance)
{
    if (!new(instance) SoftFido2Device) return (kIOReturnNoMemory);
    return (kIOReturnSuccess);
}

#endif /* !KERNEL */

kern_return_t
SoftFido2Device::Dispatch(const IORPC rpc)
{
    return _Dispatch(this, rpc);
}

kern_return_t
SoftFido2Device::_Dispatch(SoftFido2Device * self, const IORPC rpc)
{
    kern_return_t ret = kIOReturnUnsupported;
    IORPCMessage * msg = IORPCMessageFromMach(rpc.message, false);

    switch (msg->msgid)
    {

        default:
            ret = IOUserHIDDevice::_Dispatch(self, rpc);
            break;
    }

    return (ret);
}

#if KERNEL
kern_return_t
SoftFido2Device::MetaClass::Dispatch(const IORPC rpc)
{
#else /* KERNEL */
kern_return_t
SoftFido2DeviceMetaClass::Dispatch(const IORPC rpc)
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



