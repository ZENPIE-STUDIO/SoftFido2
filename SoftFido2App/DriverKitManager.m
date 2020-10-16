//
//  DriverKitManager.m
//  MobileIron Authenticator
//
//  Created by Eddie Hua on 2020/10/6.
//  Copyright © 2020 GoTrustID. All rights reserved.
//

#import "DriverKitManager.h"
#import <SystemExtensions/SystemExtensions.h>
#import <AppKit/AppKit.h>

#define LOGD(format,...)    {NSLog(format, ##__VA_ARGS__); }
#define LOGE(format,...)    {NSLog(format, ##__VA_ARGS__); }

static NSString* const kDriverExtBundleId = @"com.gotrustid.SoftFIDO2";

@interface DriverKitManager() <OSSystemExtensionRequestDelegate>
@end

@implementation DriverKitManager

+ (instancetype) shared {
    static dispatch_once_t predicate = 0;
    static DriverKitManager* instance = nil;

    dispatch_once(&predicate, ^{
        instance = [DriverKitManager new];
    });
    return instance;
}

- (void) activate {
    LOGD(@"activate");
    if (@available(macOS 10.15, *)) {
        OSSystemExtensionRequest* request = [OSSystemExtensionRequest activationRequestForExtension:kDriverExtBundleId queue:dispatch_get_main_queue()];
        request.delegate = self;
        [[OSSystemExtensionManager sharedManager] submitRequest:request];
    }
}

- (void) deactivate {
    LOGD(@"deactivate");
    if (@available(macOS 10.15, *)) {
        OSSystemExtensionRequest* request = [OSSystemExtensionRequest deactivationRequestForExtension:kDriverExtBundleId queue:dispatch_get_main_queue()];
        request.delegate = self;
        [[OSSystemExtensionManager sharedManager] submitRequest:request];
    }
}

#pragma mark - OSSystemExtensionRequestDelegate
// 決定要替換還是要無視，開發中 一律 replace
- (OSSystemExtensionReplacementAction)request:(OSSystemExtensionRequest *)request actionForReplacingExtension:(OSSystemExtensionProperties *)existing withExtension:(OSSystemExtensionProperties *)ext API_AVAILABLE(macos(10.15)) {
    LOGD(@"request: actionForReplacingExtension: withExtension");
    LOGD(@"existing Short Version = %@", existing.bundleShortVersion);
    LOGD(@"existing Version = %@", existing.bundleVersion);
    LOGD(@"ext Short Version = %@", ext.bundleShortVersion);
    LOGD(@"ext Version = %@", ext.bundleVersion);
    // Sample 是比較新舊版本後，決定 Replace or Cancel
    
    return OSSystemExtensionReplacementActionReplace;
//    return OSSystemExtensionReplacementActionCancel;
}

// 需要使用者 Approval 時，就會進來
- (void)requestNeedsUserApproval:(OSSystemExtensionRequest *)request API_AVAILABLE(macos(10.15)) {
    LOGD(@"requestNeedsUserApproval");
}

// 完成 Request 時…
- (void)request:(OSSystemExtensionRequest *)request didFinishWithResult:(OSSystemExtensionRequestResult)result  API_AVAILABLE(macos(10.15)) {
    switch (result) {
        case OSSystemExtensionRequestCompleted:
            LOGD(@"successfully completed");
            break;
        case OSSystemExtensionRequestWillCompleteAfterReboot:
            LOGD(@"successfully completed after a reboot");
            break;
        default:
            LOGD(@"%ld", result);
            break;
    }
    LOGD(@"exit");
    //exit(EXIT_SUCCESS);
    [DriverKitManager closeAllRunningInstance];
}

// Request 失敗
- (void)request:(OSSystemExtensionRequest *)request didFailWithError:(NSError *)error API_AVAILABLE(macos(10.15)) {
    //NSLog(@"didFailWithError description : %@", error.description);
    //NSLog(@"didFailWithError debugDescription : %@", error.debugDescription);
    switch (error.code) {
        case OSSystemExtensionErrorUnknown: LOGD(@"Unknown"); break;
        case OSSystemExtensionErrorMissingEntitlement: LOGD(@"MissingEntitlement"); break;
        case OSSystemExtensionErrorUnsupportedParentBundleLocation: LOGD(@"UnsupportedParentBundleLocation"); break;
        case OSSystemExtensionErrorExtensionNotFound: LOGD(@"ExtensionNotFound"); break;
        case OSSystemExtensionErrorExtensionMissingIdentifier: LOGD(@"ExtensionMissingIdentifier"); break;
        case OSSystemExtensionErrorDuplicateExtensionIdentifer: LOGD(@"DuplicateExtensionIdentifer"); break;
        case OSSystemExtensionErrorUnknownExtensionCategory: LOGD(@"UnknownExtensionCategory"); break;
        case OSSystemExtensionErrorCodeSignatureInvalid: LOGD(@"CodeSignatureInvalid"); break;
        case OSSystemExtensionErrorValidationFailed: LOGD(@"ValidationFailed"); break;
        case OSSystemExtensionErrorForbiddenBySystemPolicy: LOGD(@"ForbiddenBySystemPolicy"); break;
        case OSSystemExtensionErrorRequestCanceled: LOGD(@"RequestCanceled"); break;
        case OSSystemExtensionErrorRequestSuperseded: LOGD(@"RequestSuperseded"); break;
        case OSSystemExtensionErrorAuthorizationRequired: LOGD(@"AuthorizationRequired"); break;
        default: LOGD(@"Other Error : %@", error.localizedDescription); break;
    }
    LOGD(@"exit");
    //exit(EXIT_SUCCESS);
    [DriverKitManager closeAllRunningInstance];
    //NSLog(@"didFailWithError localizedFailureReason : %@", error.localizedFailureReason);
    //NSLog(@"didFailWithError localizedRecoverySuggestion : %@", error.localizedRecoverySuggestion);
}


+ (void) closeAllRunningInstance {
    NSLog(@"Close All Running Instance");
    NSArray<NSRunningApplication*>* apps = [NSRunningApplication runningApplicationsWithBundleIdentifier:[[NSBundle mainBundle] bundleIdentifier]];
    [apps enumerateObjectsUsingBlock:^(NSRunningApplication * _Nonnull app, NSUInteger idx, BOOL * _Nonnull stop) {
        [app terminate];
    }];
}
@end
