# zb-ble-lighting-app

## Introduction

This example demonstrates **running Zigbee and BLE communication simultaneously**.

- Its **Zigbee lighting functionality** is identical to [`examples/zigbee/lighting-app`](../../zigbee/lighting-app) (the same endpoints, clusters, On/Off, Level Control, etc.).
- On top of that, it **also integrates** the functionality of the [`examples/bluetooth-le/peripheral/periph-trsp-fota`](../../bluetooth-le/peripheral/periph-trsp-fota) BLE example (BLE TRSP + FOTA).

As a result, while the Zigbee lighting application is running, the user can also use the **OTA capability provided by `periph-trsp-fota` over BLE to update the device firmware**.

> This example can also serve as a reference template for porting other Zigbee examples into **dual-protocol Zigbee + BLE** applications: by comparing this project against `zigbee/lighting-app` and `bluetooth-le/peripheral/periph-trsp-fota`, the changes required for the integration become clear.

## Main Features

- **Zigbee Lighting**: the same lighting-device behavior as `zigbee/lighting-app`.
- **BLE TRSP (Transparent Serial Port)**: a data pass-through service over a BLE connection.
- **BLE FOTA / OTA**: firmware over-the-air update of the device via BLE.
- **Zigbee and BLE coexistence**: the radio is switched to multi-protocol mode so that both protocol stacks run simultaneously on the same SoC.

## Usage (OTA Update Flow Overview)

1. After flashing this example, the device starts the Zigbee lighting application and the BLE peripheral at the same time.
2. On the Zigbee side, join the network and control the light in the same way as `zigbee/lighting-app`.
3. On the BLE side, the device advertises with the name defined by `DEVICE_NAME` in `ble_trsp_app.c`; after connecting with a BLE tool, the firmware can be updated over-the-air (OTA) through the FOTA service.
