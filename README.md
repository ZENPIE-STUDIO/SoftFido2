# SoftFido2

The FIDO Authenticator is developed using DriverKit Frameworks, in order to port SoftU2F to macOS Big Sur.

+ [x] Handle the Virtual HID part
+ [ ] Handle CTAP Command

### Known Issues

+ ⚠️When receiving HID Frame, data loss will occur. ⚠️

  + WebAuthn failed...
  + Wrote a test program using IOHIDDeviceSetReport API，Send 64bytes (00 ~ 09)，Lost rate about 30%...




### Roles in the project and required Entitlements 

#### Driver (SoftFIDO2)

+ Entitilements - **Need to apply to Apple**

```xml
	<key>com.apple.developer.driverkit</key> <true/>
	<key>com.apple.developer.driverkit.family.hid.device</key> <true/>
	<key>com.apple.developer.driverkit.family.hid.eventservice</key> <true/>
	<key>com.apple.developer.driverkit.family.hid.virtual.device</key> <true/>
	<key>com.apple.developer.driverkit.transport.hid</key> <true/>
    <key>com.apple.developer.driverkit.transport.usb</key>
				:
```

+ Non-public entitlement

```xml
    <!-- Can solve the problem of not being able to apply for 'com.apple.developer.driverkit.userclient-access' -->
    <key>com.apple.developer.driverkit.allow-any-userclient-access</key> <true/>
```



#### Manager (FidoDriverManager)：Activate or Deactivate Driver

+ Setup XCode > Build Phases > Embed System Extensions : `com.gotrustid.SoftFIDO2.dext`
+ Entitilements - 不需申請

```xml
		<!-- Permission to activate or deactivate system extensions. -->
    <key>com.apple.developer.system-extension.install</key>
    <true/>
```

#### Client (UserClient)：Communicate with Driver

+ Entitilements - **Need to apply to Apple**

```xml
		<!-- After macOS Big Sur 11.2 "mandatory requirement" -->
		<key>com.apple.developer.driverkit.userclient-access</key>
    <array>
      <string>com.gotrustid.SoftFIDO2</string>
    </array>
```

+ ℹ️ [xnu-7195.50.7.100.1的IOKitKeys.h](https://github.com/clemensg/xnu/blob/master/iokit/IOKit/IOKitKeys.h) 中找到一個未公開的 entitlement for driver，可以不需要上述 entitlement
  + `com.apple.developer.driverkit.allow-any-userclient-access`  => 加到 SoftFIDO2

### How to Debug

+ [Disable SIP (System Integrity Protection)](https://developer.apple.com/documentation/security/disabling_and_enabling_system_integrity_protection?language=objc)：Bypass Code-Signing, Notarization Check
  + 如果沒關掉，設定Entitlement及Provisioning Profile後，可能會Build不過。
  + XCode Build Settings > Signing
    + Code Signing Entitlement
    + Provisioning Profile
+ Enable Activation from Any Directory：`$ systemextensionsctl developer on`
  + 如果沒設定，會限制在`/Applications` 下的App才能Activate Driver
+ 觀看LOG

```bash
$ log show --predicate 'sender == "sysextd" or sender CONTAINS[c] "Fido" or sender CONTAINS[c] "HID"' --info --debug --last 2m
```

#### When you update SoftFIDO2.DEXT

+ `$ systemextensionsctl reset` ：Reset Old SoftFIDO2 driver
+ (Sometimes need) Reboot your macOS
+ Rebuild SoftFIDO2
+ Run FidoDriverManager -  Activate Driver
+ Run UserClient, If success will show...

```
FidoDriverUserClient Start OK ✅
   To view the Log, enter the command in the console:
   > log show --predicate 'sender == "sysextd" or sender CONTAINS[c] "Fido" or sender CONTAINS[c] "HID"' --info --debug --last 2m
```

+ You can click the [Try SetReport] Button, it will send 64bytes same data ( For loop from 0x01 to 0x09) HID Frame to Driver .
+ You can go to https://webauthndemo.appspot.com/ to try whether Webauthn has sent data to Driver.
+ Show Log



----

### IOKit / DriverKit  對應

+ Class 對照表

|      | SoftU2F (Kext) - IOKit                   | SoftFIDO2 (Dext) - DriverKit                                |
| ---- | ---------------------------------------- | ----------------------------------------------------------- |
|      | class `SoftU2FDriver` : IOService        | class `com_gotrustid_SoftFIDO2_SoftFido2Driver` : IOService |
|      | class `SoftU2FUserClient` : IOUserClient | class `SoftFido2UserClient` : IOUserClient                  |
|      | class `SoftU2FDevice` : IOHIDDevice      | class `SoftFido2Device` : IOUserHIDDevice                   |

+ Function不是 1vs1的關係，Source Code也不多，就不列表了。只擷取資料接收的部份程式碼，在下方比較。

#### Data reception

> 在 UserClient 的 frameReceived，從收到的 `IOMemoryDescriptor *report` 讀取 HID Frame data

+ SoftU2F (Kext)：簡單的 Map過後，就可以直接存取 IOMemoryDescriptor 內容

```objc
report->prepare()
IOMemoryMap *reportMap = report->map();
// 可以直接 Access Memory 的資料
reportMap->getAddress()
report->complete();
```



+ SoftFIDO2 (Dext)：需透過 IODMACommand存取 IOMemoryDescriptor

```objc
uint64_t flags = 0;
uint32_t dmaSegmentCount = 1;
IOAddressSegment segments[32];
ret = dmaCmd->PrepareForDMA(kIODMACommandPrepareForDMANoOptions, report, 0, 0, &flags, &dmaSegmentCount, segments);
if (ret == kIOReturnSuccess) {
    uint64_t offset;
    uint64_t length;
    IOMemoryDescriptor* out = nullptr;
    ret = dmaCmd->GetPreparation(&offset, &length, &out);
    // 得到 offset = 0, length = 64
    if (out != nullptr) {
        IOMemoryMap* outMemMap = nullptr;
        ret = out->CreateMapping(kIOMemoryMapCacheModeDefault, 0, 0, 0, 0, &outMemMap);
        if (outMemMap != nullptr) {
            memcpy((void*) notifyArgs, (void*) outMemMap->GetAddress(), outMemMap->GetLength());
            OSSafeReleaseNULL(outMemMap);
        }
    }
    ret = dmaCmd->CompleteDMA(kIODMACommandCompleteDMANoOptions);
}
```



----

### References

+ WWDC 2019 - [702.System Extensions and DriverKit](https://developer.apple.com/videos/play/wwdc2019/702/)
+ [VirtualHIDDevice-Development](https://github.com/pqrs-org/Karabiner-DriverKit-VirtualHIDDevice/blob/master/DEVELOPMENT.md)
+ [Debugging and Testing System Extensions](https://developer.apple.com/documentation/driverkit/debugging_and_testing_system_extensions)

