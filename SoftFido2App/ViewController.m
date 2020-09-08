//
//  ViewController.m
//  SoftFido2App
//
//  Created by Eddie Hua on 2020/9/3.
//  Copyright © 2020 GoTrustID. All rights reserved.
//

#import "ViewController.h"
#import <SystemExtensions/SystemExtensions.h>
#import "SoftFido2Lib.h"

@interface ViewController() <OSSystemExtensionRequestDelegate>
@property (nonatomic) NSTextView* textView;
@property (nonatomic) NSButton* btnEnable;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    _textView = [[NSTextView alloc] initWithFrame:self.view.bounds];
    [self.view addSubview:_textView];
    // Do any additional setup after loading the view.
    [self activate];
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

#pragma mark -
- (void) activate {
    NSLog(@"activate");
    NSString* extBundleId = @"com.gotrustid.SoftFIDO2";
//    NSString* extBundleId = @"org.pqrs.Karabiner-DriverKit-VirtualHIDDevice";
    OSSystemExtensionRequest* request = [OSSystemExtensionRequest activationRequestForExtension:extBundleId queue:dispatch_get_main_queue()];
    request.delegate = self;
    [[OSSystemExtensionManager sharedManager] submitRequest:request];
}

- (void) deactivate {
    NSLog(@"deactivate");
    OSSystemExtensionRequest* request = [OSSystemExtensionRequest deactivationRequestForExtension:@"com.gotrustid.SoftFIDO2" queue:dispatch_get_main_queue()];
    request.delegate = self;
    [[OSSystemExtensionManager sharedManager] submitRequest:request];
}

- (void) afterActivateSuccess {
    NSLog(@"afterActivateSuccess");
    [SoftFido2Lib test];
}
#pragma mark - OSSystemExtensionRequestDelegate
// 決定要替換還是要無視，開發中 一律 replace
- (OSSystemExtensionReplacementAction)request:(OSSystemExtensionRequest *)request actionForReplacingExtension:(OSSystemExtensionProperties *)existing withExtension:(OSSystemExtensionProperties *)ext {
    NSLog(@"request: actionForReplacingExtension: withExtension");
    NSLog(@"existing = %@", existing.description);
    NSLog(@"ext = %@", ext.description);
    // Sample 是比較新舊版本後，決定 Replace or Cancel
    
    return OSSystemExtensionReplacementActionReplace;
//    return OSSystemExtensionReplacementActionCancel;
}

// 需要使用者 Approval 時，就會進來
- (void)requestNeedsUserApproval:(OSSystemExtensionRequest *)request {
    NSLog(@"requestNeedsUserApproval");
}

// 完成 Request 時…
- (void)request:(OSSystemExtensionRequest *)request didFinishWithResult:(OSSystemExtensionRequestResult)result {
    switch (result) {
        case OSSystemExtensionRequestCompleted:
            NSLog(@"didFinishWithResult : successfully completed");
            [self afterActivateSuccess];
            break;
        case OSSystemExtensionRequestWillCompleteAfterReboot:
            NSLog(@"didFinishWithResult : successfully completed after a reboot");
            break;
        default:
            NSLog(@"didFinishWithResult : %ld", result);
            break;
    }
}

// Request 失敗
- (void)request:(OSSystemExtensionRequest *)request didFailWithError:(NSError *)error {
    //NSLog(@"didFailWithError description : %@", error.description);
    //NSLog(@"didFailWithError debugDescription : %@", error.debugDescription);
    switch (error.code) {
        case OSSystemExtensionErrorUnknown: NSLog(@"didFailWithError: Unknown"); break;
        case OSSystemExtensionErrorMissingEntitlement: NSLog(@"didFailWithError: MissingEntitlement"); break;
        case OSSystemExtensionErrorUnsupportedParentBundleLocation: NSLog(@"didFailWithError: UnsupportedParentBundleLocation"); break;
        case OSSystemExtensionErrorExtensionNotFound: NSLog(@"didFailWithError: ExtensionNotFound"); break;
        case OSSystemExtensionErrorExtensionMissingIdentifier: NSLog(@"didFailWithError: ExtensionMissingIdentifier"); break;
        case OSSystemExtensionErrorDuplicateExtensionIdentifer: NSLog(@"didFailWithError: DuplicateExtensionIdentifer"); break;
        case OSSystemExtensionErrorUnknownExtensionCategory: NSLog(@"didFailWithError: UnknownExtensionCategory"); break;
        case OSSystemExtensionErrorCodeSignatureInvalid: NSLog(@"didFailWithError: CodeSignatureInvalid"); break;
        case OSSystemExtensionErrorValidationFailed: NSLog(@"didFailWithError: ValidationFailed"); break;
        case OSSystemExtensionErrorForbiddenBySystemPolicy: NSLog(@"didFailWithError: ForbiddenBySystemPolicy"); break;
        case OSSystemExtensionErrorRequestCanceled: NSLog(@"didFailWithError: RequestCanceled"); break;
        case OSSystemExtensionErrorRequestSuperseded: NSLog(@"didFailWithError: RequestSuperseded"); break;
        case OSSystemExtensionErrorAuthorizationRequired: NSLog(@"didFailWithError: AuthorizationRequired"); break;
        default:
            NSLog(@"didFailWithError localizedDescription : %@", error.localizedDescription);
            break;
    }
    //NSLog(@"didFailWithError localizedFailureReason : %@", error.localizedFailureReason);
    //NSLog(@"didFailWithError localizedRecoverySuggestion : %@", error.localizedRecoverySuggestion);
    _textView.string = error.localizedDescription;
}
@end
