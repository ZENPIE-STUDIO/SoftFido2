//
//  SoftFido2Lib.m
//  SoftFido2App
//
//  Created by Eddie Hua on 2020/9/4.
//  Copyright © 2020 GoTrustID. All rights reserved.
//

#import "SoftFido2Lib.h"

@implementation SoftFido2Lib

+ (void) tryGetMatchingServices {
    io_iterator_t * iterator = NULL;
    kern_return_t retX = IOServiceGetMatchingServices(kIOMasterPortDefault, IOServiceMatching("IOUserService"), &iterator);
    if (iterator != NULL) {
        io_service_t serviceA = NULL;
        while ((serviceA = IOIteratorNext(iterator)) != IO_OBJECT_NULL) {
            io_name_t object_name;
            IOObjectGetClass(serviceA, object_name);
            NSLog(@"object_name = %s", object_name);
            IOObjectRelease(serviceA);
        }
        IOObjectRelease(iterator);
    }
}

+ (void) test {
    NSLog(@"SoftFido2Lib - test");
    [SoftFido2Lib tryGetMatchingServices];
    const char* name0 = "SoftFido2Driver";
    const char* name1 = "com.gotrustid.SoftFIDO2.SoftFido2Driver";
    const char* name2 = "SoftFido2Device";
    // 找得到名單:
    // (IOServiceNameMatching) org_pqrs_Karabiner_DriverKit_VirtualHIDDeviceRoot
    // (IOServiceMatching) SoftU2FDriver;  // 這個看得到
    mach_port_t masterPort = kIOMasterPortDefault;
    //IOMasterPort(MACH_PORT_NULL, &masterPort);
    
    io_connect_t connect = 0;
    io_service_t service = IOServiceGetMatchingService(masterPort, IOServiceMatching(name0));
    io_service_t service1 = IOServiceGetMatchingService(masterPort, IOServiceMatching(name1));
    io_service_t service2 = IOServiceGetMatchingService(masterPort, IOServiceMatching(name2));
    if (service) NSLog(@"Found : %s", name0);
    if (service1) NSLog(@"Found : %s", name1);
    if (service2) NSLog(@"Found : %s", name2);

    
    service = IOServiceGetMatchingService(masterPort, IOServiceNameMatching(name0));
    service1 = IOServiceGetMatchingService(masterPort, IOServiceNameMatching(name1));
    service2 = IOServiceGetMatchingService(masterPort, IOServiceNameMatching(name2));
    if (service) NSLog(@"[NAME] Found : %s", name0);
    if (service1) NSLog(@"[NAME] Found : %s", name1);
    if (service2) NSLog(@"[NAME] Found : %s", name2);
    
    
    if (!service) {
        //softu2f_log(ctx, "SoftU2F.kext not loaded.\n");
        goto fail;
    }
    // Open connection to user client.
    kern_return_t ret = IOServiceOpen(service, mach_task_self(), 0, &connect);
    if (ret != KERN_SUCCESS) {
        //softu2f_log(ctx, "Error connecting to SoftU2F.kext: %d\n", ret);
        goto fail;
    }
    IOObjectRelease(service);
    service = IO_OBJECT_NULL;

fail:
    if (service)
        IOObjectRelease(service);
    if (connect)
        IOServiceClose(connect);
}
@end
