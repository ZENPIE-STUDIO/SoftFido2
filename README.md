# SoftFido2

The FIDO Authenticator is developed using DriverKit Frameworks, in order to port SoftU2F to macOS Big Sur.

### Known Issues

+ ⚠️When receiving HID Frame, data loss will occur. ⚠️

  + WebAuthn failed...
  + Wrote a test program using IOHIDDeviceSetReport API，Send 64bytes (00 ~ 09)，Lost rate about 30%...

+ After macOS Big Sur 11.2, App cannot connect to SoftFIDO2.dext

  + Need entitlement

  ```xml
  <key>com.apple.developer.driverkit.userclient-access</key>
  <array>
    <string>com.gotrustid.SoftFIDO2</string>
  </array>
  ```



### Roles in the project and required Entitlements 

#### Driver (SoftFIDO2)

+ Entitilements - **Need to apply to Apple**

```xml
	<key>com.apple.developer.driverkit</key>
	<true/>
	<key>com.apple.developer.driverkit.family.hid.device</key>
	<true/>
	<key>com.apple.developer.driverkit.family.hid.eventservice</key>
	<true/>
	<key>com.apple.developer.driverkit.family.hid.virtual.device</key>
	<true/>
	<key>com.apple.developer.driverkit.transport.hid</key>
	<true/>
    <key>com.apple.developer.driverkit.transport.usb</key>
				:
```

#### Manager (FidoDriverManager)：Activate or Deactivate Driver

+ Setup XCode > Build Phases > Embed System Extensions : `com.gotrustid.SoftFIDO2.dext`
+ Entitilements - 不需申請

```xml
		<!-- Permission to activate or deactivate system extensions. -->
    <key>com.apple.developer.system-extension.install</key>
    <true/>
```

#### Client：Use Driver

+ Entitilements - **Need to apply to Apple**

```xml
    <key>com.apple.developer.driverkit.userclient-access</key>
    <array>
      <string>com.gotrustid.SoftFIDO2</string>
    </array>
```

+ ℹ️ [xnu-7195.50.7.100.1的IOKitKeys.h](https://github.com/clemensg/xnu/blob/master/iokit/IOKit/IOKitKeys.h) 中找到一個未公開的 entitlement for driver，可以不需要上述 entitlement
  + `com.apple.developer.driverkit.allow-any-userclient-access`  => 加到 SoftFIDO2

### How to Debug

+ [Disable SIP (System Integrity Protection)](https://developer.apple.com/documentation/security/disabling_and_enabling_system_integrity_protection?language=objc)：關閉Code-Signing, Notarization Check
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

#### Update SoftFIDO2.DEXT

+ `$ systemextensionsctl reset` ：Reset Old SoftFIDO2 driver
+ (Sometimes need) Reboot your macOS
+ Rebuild SoftFIDO2
+ Run SoftFido2App - Activate Driver
+ Run Test App - 送資料給 SoftFIDO2 Driver
+ Show Log

```bash
$ log show --predicate 'sender == "sysextd" or sender CONTAINS[c] "Fido" or sender CONTAINS[c] "HID"' --info --debug --last 2m
```





----

### IOKit / DriverKit  API 對應

主要差異在 UserClient 部份

frameReceived



----

### 重要參考

+ [VirtualHIDDevice-Development](https://github.com/pqrs-org/Karabiner-DriverKit-VirtualHIDDevice/blob/master/DEVELOPMENT.md)

+ [Debugging and Testing System Extensions](https://developer.apple.com/documentation/driverkit/debugging_and_testing_system_extensions)