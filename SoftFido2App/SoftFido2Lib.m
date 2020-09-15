//
//  SoftFido2Lib.m
//  SoftFido2App
//
//  Created by Eddie Hua on 2020/9/4.
//  Copyright © 2020 GoTrustID. All rights reserved.
//

#import "SoftFido2Lib.h"
#include "u2f_hid.h"
#include "UserKernelShared.h"

@interface SoftFido2Lib()
@property (nonatomic) softu2f_ctx* ctx;
@end

@implementation SoftFido2Lib

- (instancetype) init {
    io_connect_t connect = [SoftFido2Lib initConnect];
    if (connect == 0) {
        return nil;
    }
    if (self = [super init]) {
        self.ctx = (softu2f_ctx *)calloc(1, sizeof(softu2f_ctx));
        self.ctx->debug = false;
        self.ctx->con = connect;
        int err = pthread_mutex_init(&(self.ctx->mutex), NULL);
        if (err) {
            NSLog(@"[SoftFido2Lib:init] Error creating mutex.\n");
        }
    }
    return self;
}

- (void) close {
    if (_ctx->con) {
        IOServiceClose(_ctx->con);
    }
}
// Read HID messages from device in loop.
// softu2f_run
- (void) run {
    IONotificationPortRef notification_port;
    mach_port_t mnotification_port;
    CFRunLoopSourceRef run_loop_source;
    CFRunLoopTimerRef run_loop_timer;
    CFRunLoopTimerContext timer_ctx;
    io_async_ref64_t async_ref;
    kern_return_t ret;

    if (_ctx->run_loop) {
        NSLog(@"[SoftFido2Lib:run] Already Running!");
        return;
    }

    // Create port to listen for kernel notifications on.
    notification_port = IONotificationPortCreate(kIOMasterPortDefault);
    if (!notification_port) {
        NSLog(@"[SoftFido2Lib:run] Error getting notification port");
        return;
    }

    // Get lower level mach port from notification port.
    mnotification_port = IONotificationPortGetMachPort(notification_port);
    if (!mnotification_port) {
        NSLog(@"[SoftFido2Lib:run] Error getting mach notification port.");
        return;
    }

    // Create a run loop source from our notification port so we can add the port to our run loop.
    run_loop_source = IONotificationPortGetRunLoopSource(notification_port);
    if (run_loop_source == NULL) {
        NSLog(@"[SoftFido2Lib:run] Error getting run loop source.");
        return;
    }

    // Create a timer to run periodically.
    memset(&timer_ctx, 0, sizeof(CFRunLoopTimerContext));
    timer_ctx.info = _ctx;
    run_loop_timer = CFRunLoopTimerCreate(NULL, 0, 0.2, 0, 0, softu2f_async_timer_callback, &timer_ctx);
    if (run_loop_source == NULL) {
        NSLog(@"[SoftFido2Lib:run] Error creating timer.");
        return;
    }

    // Add the notification port and timer to the run loop.
    CFRunLoopAddSource(CFRunLoopGetCurrent(), run_loop_source, kCFRunLoopDefaultMode);
    CFRunLoopAddTimer(CFRunLoopGetCurrent(), run_loop_timer, kCFRunLoopDefaultMode);

    // Params to pass to the kernel.
    async_ref[kIOAsyncCalloutFuncIndex] = (uint64_t) softu2f_async_callback;
    async_ref[kIOAsyncCalloutRefconIndex] = (uint64_t) _ctx;

    // Tell the kernel how to notify us.
    ret = IOConnectCallAsyncScalarMethod(_ctx->con, kSoftFidoUserClientNotifyFrame, mnotification_port, async_ref, kIOAsyncCalloutCount, NULL, 0, NULL, 0);
    if (ret != kIOReturnSuccess) {
        NSLog(@"[SoftFido2Lib:run] Error registering for setFrame notifications.");
        return;
    }

    // Blocks until the run loop is stopped in our callback.
    NSLog(@"[SoftFido2Lib:run] Starting softu2f async run loop.");
    _ctx->run_loop = CFRunLoopGetCurrent();
    CFRunLoopRun();
    _ctx->run_loop = NULL;

    // Clean up.
    IONotificationPortDestroy(notification_port);
}

+ (io_connect_t) initConnect {
    NSLog(@"SoftFido2Lib - init");
    const char* name0 = "com_gotrustid_SoftFIDO2_SoftFido2Driver";
    mach_port_t masterPort = kIOMasterPortDefault;
    //IOMasterPort(MACH_PORT_NULL, &masterPort);
    
    io_connect_t connect = 0;
    io_service_t service = IOServiceGetMatchingService(masterPort, IOServiceNameMatching(name0));
    //if (service) NSLog(@"[NAME] Found : %s", name0);
    if (!service) {
        //softu2f_log(ctx, "SoftU2F.kext not loaded.\n");
        NSLog(@"Driver Not Found : %s", name0);
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
    NSLog(@"Success");
    return connect;
fail:
    NSLog(@"Failed!");
    if (service)
        IOObjectRelease(service);
    if (connect)
        IOServiceClose(connect);
    return 0;
}

#pragma mark - softu2f.c
// Free a HID message and associated data.
void softu2f_hid_msg_free(softu2f_hid_message *msg) {
    if (msg) {
        if (msg->data)
            CFRelease(msg->data);
        if (msg->buf)
            CFRelease(msg->buf);
        free(msg);
    }
}

// Log a message if logging is enabled.
void softu2f_log(softu2f_ctx *ctx, char *fmt, ...) {
    if (ctx->debug) {
        va_list argp;
        va_start(argp, fmt);
        vfprintf(stderr, fmt, argp);
        va_end(argp);
    }
}
// Log a U2FHID_FRAME if logging is enabled.
void softu2f_debug_frame(softu2f_ctx *ctx, U2FHID_FRAME *frame, bool recv) {
  uint8_t *data = NULL;
  uint16_t dlen = 0;

  if (recv) {
    softu2f_log(ctx, "Received frame:\n");
  } else {
    softu2f_log(ctx, "Sending frame:\n");
  }

  softu2f_log(ctx, "\tCID: 0x%08x\n", frame->cid);

  switch (FRAME_TYPE(*frame)) {
  case TYPE_INIT:
    softu2f_log(ctx, "\tTYPE: INIT\n");
    softu2f_log(ctx, "\tCMD: 0x%02x\n", frame->init.cmd & ~TYPE_MASK);
    softu2f_log(ctx, "\tBCNTH: 0x%02x\n", frame->init.bcnth);
    softu2f_log(ctx, "\tBCNTL: 0x%02x\n", frame->init.bcntl);
    data = frame->init.data;
    dlen = HID_RPT_SIZE - 7;

    break;

  case TYPE_CONT:
    softu2f_log(ctx, "\tTYPE: CONT\n");
    softu2f_log(ctx, "\tSEQ: 0x%02x\n", frame->cont.seq);
    data = frame->cont.data;
    dlen = HID_RPT_SIZE - 5;

    break;
  }

  softu2f_log(ctx, "\tDATA:");
  for (int i = 0; i < dlen; i++) {
    softu2f_log(ctx, " %02x", data[i]);
  }

  softu2f_log(ctx, "\n\n");
}
// Called by the kernel when setReport is called on our device.
// Eddie: solo - ctaphid_buffer_packet
void softu2f_async_callback(void *refcon, IOReturn result, io_user_reference_t* args, uint32_t numArgs) {
  softu2f_ctx *ctx = NULL;
  U2FHID_FRAME *frame;

  if (!refcon || result != kIOReturnSuccess) {
    NSLog(@"Unexpected call to softu2f_async_callback.\n");
    goto stop;
  }

  ctx = (softu2f_ctx *)refcon;

  if (numArgs * sizeof(io_user_reference_t) != sizeof(U2FHID_FRAME)) {
      NSLog(@"Unexpected argument count in softu2f_async_callback.");
      goto stop;
  }

  frame = (U2FHID_FRAME *)args;
  softu2f_debug_frame(ctx, frame, true);
  pthread_mutex_lock(&ctx->mutex);
  // Read frame into a HID message.
  softu2f_hid_frame_read(ctx, frame);

  // Handle any completed messages.
  softu2f_hid_handle_messages(ctx);
  pthread_mutex_unlock(&ctx->mutex);

  return;

stop:
  if (ctx)
      NSLog(@"Shutting down softu2f async run loop because of error.");

  CFRunLoopStop(CFRunLoopGetCurrent());
}
// Called periodically in our runloop.
void softu2f_async_timer_callback(CFRunLoopTimerRef timer, void* info) {
    softu2f_ctx *ctx = (softu2f_ctx *)info;
    if (!ctx)
        goto stop;

    pthread_mutex_lock(&ctx->mutex);
    // Handle any completed messages (checking for timeouts).
    softu2f_hid_handle_messages(ctx);
    pthread_mutex_unlock(&ctx->mutex);
    return;

stop:
    if (ctx)
        softu2f_log(ctx, "Shutting down softu2f async run loop because of error.\n");
    CFRunLoopStop(CFRunLoopGetCurrent());
}
@end
