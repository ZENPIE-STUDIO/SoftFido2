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
@property (nonatomic) NSButton* btnActivate;
@property (nonatomic) NSButton* btnDeactivate;
@property (nonatomic) NSButton* btnTry;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self initLayout];
    // Do any additional setup after loading the view.
    //[self activate];
}

- (void) initLayout {
    CGFloat fMargin = 10;
    CGFloat fButtonWidth = 120;
    CGFloat fButtonHeight = 40;
    CGFloat fPosY = 200;
    _btnActivate = [NSButton buttonWithTitle:@"Activate" target:self action:@selector(activate)];
    _btnActivate.frame = NSMakeRect(fMargin, fPosY, fButtonWidth, fButtonHeight);
    [self.view addSubview:_btnActivate];
    //
    fPosY -= (fButtonHeight + fMargin);
    _btnDeactivate = [NSButton buttonWithTitle:@"Deactivate" target:self action:@selector(deactivate)];
    _btnDeactivate.frame = NSMakeRect(fMargin, fPosY, fButtonWidth, fButtonHeight);
    [self.view addSubview:_btnDeactivate];
    //
    fPosY -= (fButtonHeight + fMargin);
    _btnTry = [NSButton buttonWithTitle:@"Try" target:self action:@selector(trySomething)];
    _btnTry.frame = NSMakeRect(fMargin, fPosY, fButtonWidth, fButtonHeight);
    [self.view addSubview:_btnTry];
    
    // ------
    NSRect rcTextView = self.view.bounds;
    rcTextView.origin.x = fButtonWidth + fMargin * 2;
    rcTextView.origin.y = fMargin;
    rcTextView.size.width -= fButtonWidth + (fMargin * 3);
    rcTextView.size.height -= fMargin * 2;
    _textView = [[NSTextView alloc] initWithFrame:rcTextView];
    [self.view addSubview:_textView];
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

- (void) trySomething {
    NSLog(@"trySomething");
    [SoftFido2Lib test];
}
#pragma mark - OSSystemExtensionRequestDelegate
// 決定要替換還是要無視，開發中 一律 replace
- (OSSystemExtensionReplacementAction)request:(OSSystemExtensionRequest *)request actionForReplacingExtension:(OSSystemExtensionProperties *)existing withExtension:(OSSystemExtensionProperties *)ext {
    NSLog(@"request: actionForReplacingExtension: withExtension");
    NSLog(@"existing Short Version = %@", existing.bundleShortVersion);
    NSLog(@"existing Version = %@", existing.bundleVersion);
    NSLog(@"ext Short Version = %@", ext.bundleShortVersion);
    NSLog(@"ext Version = %@", ext.bundleVersion);
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
            //[self afterActivateSuccess];
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
        case OSSystemExtensionErrorUnknown:
            _textView.string = @"Unknown";
            NSLog(@"didFailWithError: Unknown");
            break;
        case OSSystemExtensionErrorMissingEntitlement:
            _textView.string = @"MissingEntitlement";
            NSLog(@"didFailWithError: MissingEntitlement");
            break;
        case OSSystemExtensionErrorUnsupportedParentBundleLocation:
            _textView.string = @"UnsupportedParentBundleLocation";
            NSLog(@"didFailWithError: UnsupportedParentBundleLocation");
            break;
        case OSSystemExtensionErrorExtensionNotFound:
            _textView.string = @"ExtensionNotFound";
            NSLog(@"didFailWithError: ExtensionNotFound");
            break;
        case OSSystemExtensionErrorExtensionMissingIdentifier:
            _textView.string = @"ExtensionMissingIdentifier";
            NSLog(@"didFailWithError: ExtensionMissingIdentifier");
            break;
        case OSSystemExtensionErrorDuplicateExtensionIdentifer:
            _textView.string = @"DuplicateExtensionIdentifer";
            NSLog(@"didFailWithError: DuplicateExtensionIdentifer");
            break;
        case OSSystemExtensionErrorUnknownExtensionCategory:
            _textView.string = @"UnknownExtensionCategory";
            NSLog(@"didFailWithError: UnknownExtensionCategory");
            break;
        case OSSystemExtensionErrorCodeSignatureInvalid:
            _textView.string = @"CodeSignatureInvalid";
            NSLog(@"didFailWithError: CodeSignatureInvalid");
            break;
        case OSSystemExtensionErrorValidationFailed:
            _textView.string = @"ValidationFailed";
            NSLog(@"didFailWithError: ValidationFailed");
            break;
        case OSSystemExtensionErrorForbiddenBySystemPolicy:
            _textView.string = @"ForbiddenBySystemPolicy";
            NSLog(@"didFailWithError: ForbiddenBySystemPolicy");
            break;
        case OSSystemExtensionErrorRequestCanceled:
            _textView.string = @"RequestCanceled";
            NSLog(@"didFailWithError: RequestCanceled");
            break;
        case OSSystemExtensionErrorRequestSuperseded:
            _textView.string = @"RequestSuperseded";
            NSLog(@"didFailWithError: RequestSuperseded");
            break;
        case OSSystemExtensionErrorAuthorizationRequired:
            _textView.string = @"AuthorizationRequired";
            NSLog(@"didFailWithError: AuthorizationRequired");
            break;
        default:
            _textView.string = error.localizedDescription;
            NSLog(@"didFailWithError localizedDescription : %@", error.localizedDescription);
            break;
    }
    //NSLog(@"didFailWithError localizedFailureReason : %@", error.localizedFailureReason);
    //NSLog(@"didFailWithError localizedRecoverySuggestion : %@", error.localizedRecoverySuggestion);
}
@end
