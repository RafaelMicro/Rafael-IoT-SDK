# Examples Descript

**Recommendation:** Beginners should start with Hello World or Peripheral examples (UART / GPIO).

---

## Bluetooth® Low Energy (BLE)

Suitable for BLE devices, mobile connectivity, Mesh applications, and production testing scenarios.

### BLE Central

* **Central Data Rate** `bluetooth-le/central/central-data-rate`  
  ▶ BLE Central device for testing data transfer performance and throughput

* **Central TRSP** `bluetooth-le/central/central-trsp`  
  ▶ General-purpose BLE transfer protocol example, suitable for communication with mobile phones or other BLE devices

### BLE Mesh

* **Mesh Gateway** `bluetooth-le/mesh/gateway`  
  ▶ BLE Mesh Gateway example, suitable for smart lighting and building control systems

* **Mesh Lightness TRSP** `bluetooth-le/mesh/lightness-trsp`  
  ▶ Mesh Light control and data transfer demonstration

### Multi-link

* **Multi 1C1P** `bluetooth-le/multi-link/multi-1c1p`  
  ▶ One-to-many BLE connection management demonstration

* **Power Management** `bluetooth-le/multi-link/power-management`  
  ▶ Low-power management in multi-connection scenarios

### BLE Other Applications

* **AT Command** `bluetooth-le/others/at-command`  
  ▶ BLE AT command control, suitable for modular products or testing tools

* **iBeacon** `bluetooth-le/others/ibeacon`  
  ▶ iBeacon broadcast example, applicable for location services and marketing push notifications

### BLE Peripheral

* **Peripheral Data Rate** – High-speed data transfer
* **HOGP** – Human Interface Device (Keyboard / Mouse)
* **HRS** – Heart Rate Measurement (Health Device)
* **TRSP / TRSP FOTA** – General-purpose data transfer and OTA updates

**Path:**
```
bluetooth-le/peripheral/periph-*
```

---

## Zigbee Applications

Applicable for smart home, sensor networks, and building automation.

* **Door Lock** – Smart door lock
* **Door Sensor** – Door/window sensor
* **Gateway Module** – Zigbee Gateway
* **Lighting App** – Smart lighting
* **Smart Plug** – Smart plug
* **Switch / Wall Switch** – Wall switch
* **Temperature / Illuminance / PIR Sensor** – Environmental sensors

**Path:**
```
zigbee/<application-name>
```

---

## Sub-GHz (Sub-G)

Suitable for long-range, low-power, IoT sensor networks.

### Mesh-It-Up (MIU)

* **FTD / MTD / Sniffer**  
  ▶ Sub-G Mesh Network and packet analysis

### Other Sub-G Examples

* **SubG Sample** ▶ Basic Sub-G communication demonstration
* **SubG TRX** ▶ Sub-G transmit/receive testing
* **Wake-on-Radio** ▶ Ultra-low power wake-up application

**Path:**
```
sub-g/*
```

---

## Peripheral / Driver Examples

These examples demonstrate MCU peripheral functions and are ideal for:

* Driver development reference
* Function verification
* Educational documentation

### Common Peripherals

* GPIO / UART / I2C / I2S / SPI (QSPI)
* DMA (Polling / Interrupt)
* Timer / Slow Timer / RTC
* PWM
* Flash / OTP
* Watchdog (WDT)
* Comparator 
* SADC
  
### Low Power & Power Management

* Sleep / Deep Sleep / Power Down
* GPIO / RTC Wake-up
* AUX / BOD Comparator

### Crypto / Security

* AES / ECC / ECDSA / HMAC / HKDF / DRBG
* SM2 / SM3 / SM4 (RT584 Serial)

**Path:**
```
peripheral/<module>/<example>
```

---

## Bootloader & System

* **Bootloader App** `bootloader-app/bootloader-app`  
  ▶ Firmware update and system boot process reference

* **RF Lab Test Tool** `rf-labtest-tool`  
  ▶ RF testing and production line verification tool

* **Application Template** `template/application`  
  ▶ Quick-start template for new projects

* **Hello World** `helloworld/helloworld`  
  ▶ Most basic SDK build and execution example

---

## Recommended Learning Path (For New Customers)

1. **Hello World**
2. **Peripheral** → UART / GPIO
3. **BLE Peripheral** or **Zigbee Device**
4. **Low Power**
