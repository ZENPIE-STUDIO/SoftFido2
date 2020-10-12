//
//  main.m
//  SoftFido2App
//
//  Created by Eddie Hua on 2020/9/3.
//  Copyright © 2020 GoTrustID. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "DriverKitManager.h"
#ifdef DEBUG
    #import "U2FHID.h"
#endif

void closePreviousRunningInstance() {
    NSLog(@"Close Previous Running Instance");
    NSArray<NSRunningApplication*>* apps = [NSRunningApplication runningApplicationsWithBundleIdentifier:[[NSBundle mainBundle] bundleIdentifier]];
    if ([apps count] > 1) {
        //[[apps objectAtIndex:1] terminate];   // 自己
        [[apps objectAtIndex:0] terminate];     // 先前的
    }
}

void processArguments(NSArray<NSString *> *arguments) {
    boolean_t __block doActivate = true;
    // Idx(0) 自己的執行路徑
    // Idx(1) 我帶的參數 removeAuthPlugin
    //NSLog(@"processArguments count = %ld", arguments.count);
    // 在此我只會處理3種事 activate, deactivate, exit
    [arguments enumerateObjectsUsingBlock:^(NSString * _Nonnull argument, NSUInteger idx, BOOL * _Nonnull stop) {
        //LOGD(@"%ld) %@", idx, argument);
        if ([argument isEqualToString:@"deactivate"]) {
            NSLog(@"deactivate");
            doActivate = false;
            [[DriverKitManager shared] deactivate];
        } else if ([argument isEqualToString:@"exit"]) {
            NSLog(@"exit");
            doActivate = false;
            closePreviousRunningInstance();
            exit(EXIT_SUCCESS);
        }
    }];
    if (doActivate) {
#ifdef DEBUG
        U2FHID* u2fhid = [U2FHID new];
        [u2fhid run];
#endif
        //[[DriverKitManager shared] activate];
    }
}

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        [NSApplication sharedApplication];
        processArguments([NSProcessInfo processInfo].arguments);
        // Setup code that might create autoreleased objects goes here.
        [NSApp setActivationPolicy:NSApplicationActivationPolicyAccessory];
        [NSApp run];
        //exit(EXIT_SUCCESS); - 直接離開是看不到 activate 要求的
    }
    //return NSApplicationMain(argc, argv);
}
