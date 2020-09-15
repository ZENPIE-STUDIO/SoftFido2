//
//  SoftU2FContants.m
//  MacLogon
//
//  Created by Eddie Hua on 2019/10/14.
//  Copyright © 2019 GoTrustID. All rights reserved.
//

#import "SoftU2FContants.h"


const NSInteger U2F_CHAL_SIZE = 32;
const NSInteger U2F_APPID_SIZE = 32;

const NSInteger U2F_EC_KEY_SIZE = 32;    // EC key size in bytes
const NSInteger U2F_EC_POINT_SIZE = ((U2F_EC_KEY_SIZE * 2) + 1); // Size of EC point

const NSInteger MaxResponseSize = 0xFFFF + 1;
