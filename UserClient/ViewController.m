//
//  ViewController.m
//  UserClient
//
//  Created by Eddie Hua on 2021/1/28.
//  Copyright © 2021 GoTrustID. All rights reserved.
//

#import "ViewController.h"
#import "U2FHID.h"
#include <IOKit/hid/IOHIDManager.h>

@interface ViewController()<U2fHidDelegate>
@property (nonatomic) U2FHID* u2fhid;

// Try SetReport to SoftFIDO2 Driver
@property (nonatomic) NSButton* btnSetReport;
//
@property (nonatomic) IOHIDManagerRef hidManager;
@property (nonatomic) io_iterator_t addedIter;
@property (nonatomic) IOHIDDeviceRef softu2fDevice;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    NSTextField* textfield = [[NSTextField alloc] init];
    textfield.alignment = NSTextAlignmentCenter;
    textfield.editable = NO;
    textfield.bordered = NO;
    textfield.bezeled = NO;
    //textfield.textColor = newColorRGB(40, 40, 40);
    textfield.font = [NSFont systemFontOfSize:20];
    textfield.backgroundColor = [NSColor clearColor];
    textfield.stringValue = @"Here will be no information, see your xcode log.";
    [textfield sizeToFit];
    NSRect frame = textfield.frame;
    frame.origin.y = CGRectGetMidY(self.view.frame);
    frame.size.width = self.view.frame.size.width;
    textfield.frame = frame;
    [self.view addSubview:textfield];

    // Do any additional setup after loading the view.
    [self startFidoDriverUserClient];
    //
    [self initDriverTester];
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];
    // Update the view, if already loaded.
}

- (void) startFidoDriverUserClient {
    if (_u2fhid != nil) {
        NSLog(@"FidoDriverUserClient Already Start");
        return;
    }
    _u2fhid = [U2FHID new];
    if (_u2fhid == nil) {
        NSLog(@"FidoDriverUserClient Start ❌");
        return;
    }
    _u2fhid.delegate = self;
    ViewController* __weak weakSelf = self;
    [_u2fhid handleType:CTAPHID_CBOR Handler:^bool(softu2f_hid_message * _Nonnull msg) {
        bool ok = false;
        NSData *data = (__bridge NSData *) msg->data;
        NSLog(@"[CTAPHID_CBOR] CMD:0x%02x  CID:%u  data = %@", msg->cmd, msg->cid, data);
        if (weakSelf != nil) {
            //ok = [weakSelf handle_ctaphid_cbor:msg->cmd Data:data CID:msg->cid];
        } else {
            NSLog(@"[CTAPHID_CBOR] Lost my self");
        }
        return ok;
    }];
    NSLog(@"FidoDriverUserClient Start OK ✅\n\
          To view the Log, enter the command in the console:\n\
           > log show --predicate 'sender == \"sysextd\" or sender CONTAINS[c] \"Fido\" or sender CONTAINS[c] \"HID\"' --info --debug --last 2m");
}

#pragma mark - U2fHidDelegate
- (void) didStopU2FHID:(U2FHID *)u2fhid Unexpected:(BOOL)isUnexpected {
    if (isUnexpected) {
        NSLog(@"didStopU2FHID Unexpected Stop!");
    } else {
        NSLog(@"didStopU2FHID Stop");
    }
}


#pragma mark - Test SoftFIDO2 Driver Known Issues

- (void) initDriverTester {
    _btnSetReport = [NSButton buttonWithTitle:@"Try SetReport" target:self action:@selector(trySetReport)];
    [_btnSetReport sizeToFit];
    NSRect frame = _btnSetReport.frame;
    frame.origin.x = 10;
    frame.origin.y = 10;
    _btnSetReport.frame = frame;
    [self.view addSubview:_btnSetReport];
    // Do any additional setup after loading the view.
    [self initHIDManager];
}

+ (NSModalResponse) alertMessage:(NSString* _Nonnull) message Info:(NSString* _Nullable) info ButtonTitle:(NSString* _Nonnull) buttonTitle {
    NSAlert* alert = [[NSAlert alloc] init];
    alert.messageText = message;
    if (info != nil)
        alert.informativeText = info;
    [alert addButtonWithTitle:buttonTitle];
    return [alert runModal];
}
// ⚠️When receiving HID Frame, data loss will occur. ⚠️
- (void) trySetReport {
    if (_softu2fDevice == nil) {
        NSLog(@"softu2fDevice is nil");
        NSAlert* alert = [[NSAlert alloc] init];
        alert.messageText = @"SoftFIDO2 Driver not work.";
        [alert runModal];
        return;
    }
    
    IOReturn ret = IOHIDDeviceOpen(_softu2fDevice, kIOHIDOptionsTypeNone );
    NSLog(@"IOHIDDeviceOpen ret = 0x%X", ret);  // kIOReturnNotPermitted (2E2)
    ViewController* __weak weakSelf = self;
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        const CFIndex reportLength = 64;
        uint8_t report[reportLength] = {0x0};
        // 連續送個10次
        for (int i = 1; i < 10; i++) {
            memset(report, i, reportLength);
            IOReturn ret = IOHIDDeviceSetReport(weakSelf.softu2fDevice, kIOHIDReportTypeOutput,
                                       0, // CFIndex reportID
                                       report, reportLength);
            NSLog(@"IOHIDDeviceSetReport %d  ret = 0x%X", i, ret);
            if (ret != 0) {
                NSLog(@"       system err = %x", err_get_system(ret));
                NSLog(@"          sub err = %x", err_get_sub(ret));
                NSLog(@"         code err = %x", err_get_code(ret));
                break;
            }
            // kIOReturnNotOpen (2CD)
            // kIOReturnTimeout (2D6) <= After updated macOS 11.1 Beta (20C5048k)
            [NSThread sleepForTimeInterval:1.1];
        }
    });
//    IOHIDDeviceClose(_softu2fDevice, kIOHIDOptionsTypeNone);
}
#pragma mark -
// --------------------------------------------
void onDeviceAddedCallback(void * _Nullable        context,
                           IOReturn                result,
                           void * _Nullable        sender,
                           IOHIDDeviceRef          device) {
    //NSLog(@"--------------- onDeviceAdded ---------------");
    ViewController* vc = (__bridge ViewController*) context;
    //[vc dumpDeviceInfo:device];
    // -----
    NSString* productName = getStringProperty(device, CFSTR(kIOHIDProductKey), @"");
    if ([productName isEqualToString:@"SoftFido2Driver (DriverKit)"]) {
//    if ([productName isEqualToString:@"SoftU2F"]) {
        vc.softu2fDevice = device;
    }
}
void onDeviceRemovedCallback(void * _Nullable        context,
                             IOReturn                result,
                             void * _Nullable        sender,
                             IOHIDDeviceRef          device) {
    //NSLog(@"--------------- onDeviceRemoved ---------------");
    //ViewController* vc = (__bridge ViewController*) context;
    //[vc dumpDeviceInfo:device];
}

int32_t getIntProperty(IOHIDDeviceRef device, CFStringRef key, int32_t defaultValue) {
    int32_t value = 0;
    CFNumberRef number = IOHIDDeviceGetProperty(device, key);
    if (number != NULL && CFNumberGetValue(number, kCFNumberSInt32Type, &value)) {
    } else {
        value = defaultValue;
    }
    return value;
}

NSString* getStringProperty(IOHIDDeviceRef device, CFStringRef key, NSString* defaultValue) {
    //CFStringRef value = defaultValue;
    CFStringRef value = IOHIDDeviceGetProperty(device, key);
    if (value == NULL) {
        value = (__bridge CFStringRef) defaultValue;
    }
    return (__bridge NSString*) value;
}
#pragma mark - IOHIDManager
- (void) initHIDManager {
    _hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    IOHIDManagerSetDeviceMatching(_hidManager, NULL);
    IOHIDManagerRegisterDeviceMatchingCallback(_hidManager, onDeviceAddedCallback, (void*)self);
    IOHIDManagerRegisterDeviceRemovalCallback(_hidManager,
          onDeviceRemovedCallback, (void*)self);
    IOHIDManagerScheduleWithRunLoop(_hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    // ---
}

- (void) releaseHIDManager {
    if (_hidManager) {
        IOHIDManagerUnscheduleFromRunLoop(_hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
        IOHIDManagerClose(_hidManager, kIOHIDOptionsTypeNone);
        CFRelease(_hidManager);
        _hidManager = NULL;
    }
}


- (void) dumpDeviceInfo:(IOHIDDeviceRef) device {
    int32_t locationId = getIntProperty(device, CFSTR(kIOHIDLocationIDKey), 0);
    int32_t vendorId = getIntProperty(device, CFSTR(kIOHIDVendorIDKey), 0);
    int32_t productId = getIntProperty(device, CFSTR(kIOHIDProductIDKey), 0);
    int32_t usage = getIntProperty(device, CFSTR(kIOHIDPrimaryUsageKey), 0);
    int32_t usage_page = getIntProperty(device, CFSTR(kIOHIDPrimaryUsagePageKey), 0);
    int32_t maxInputReportSize = getIntProperty(device, CFSTR(kIOHIDMaxInputReportSizeKey), 0);
    int32_t maxOutputReportSize = getIntProperty(device, CFSTR(kIOHIDMaxOutputReportSizeKey), 0);
    int32_t maxFeatureReportSize = getIntProperty(device, CFSTR(kIOHIDMaxFeatureReportSizeKey), 0);
    //
    NSString* productName = getStringProperty(device, CFSTR(kIOHIDProductKey), @"");
    NSString* serialNumber = getStringProperty(device, CFSTR(kIOHIDSerialNumberKey), @"");
    NSString* manufacturer = getStringProperty(device, CFSTR(kIOHIDManufacturerKey), @"");
    NSLog(@"productName = %@", productName);
    NSLog(@"serialNumber = %@", serialNumber);
    NSLog(@"manufacturer = %@", manufacturer);
    NSLog(@"LocationID = %d", locationId);
    NSLog(@"vendorId = %d", vendorId);
    NSLog(@"productId = %d", productId);
    NSLog(@"usage = %d", usage);
    NSLog(@"usage_page = %d", usage_page);
    NSLog(@"maxInputReportSize = %d", maxInputReportSize);
    NSLog(@"maxOutputReportSize = %d", maxOutputReportSize);
    NSLog(@"maxFeatureReportSize = %d", maxFeatureReportSize);
}

@end
