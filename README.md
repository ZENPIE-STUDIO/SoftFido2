# SoftFido2

The FIDO Authenticator is developed using DriverKit Frameworks, in order to port SoftU2F to macOS Big Sur.

### Know Issues

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

  