//
//  ViewController.m
//  UserClient
//
//  Created by Eddie Hua on 2021/1/28.
//  Copyright © 2021 GoTrustID. All rights reserved.
//

#import "ViewController.h"
#import "U2FHID.h"

@interface ViewController()<U2fHidDelegate>
@property (nonatomic) U2FHID* u2fhid;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // Do any additional setup after loading the view.
    [self startFidoDriverUserClient];
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
}

#pragma mark - U2fHidDelegate
- (void) didStopU2FHID:(U2FHID *)u2fhid Unexpected:(BOOL)isUnexpected {
    if (isUnexpected) {
        NSLog(@"didStopU2FHID Unexpected Stop!");
    } else {
        NSLog(@"didStopU2FHID Stop");
    }
}
@end
