# miu-ftd Example Guide

## What is the miu-ftd Example?

The `miu-ftd` example serves as the reference implementation for an OpenThread FTD (Full Thread Device) within the Mesh It Up (MIU) SDK. It integrates Sub-GHz connectivity for robust and automatic mesh networking capabilities. This example demonstrates how to configure core network parameters, utilize OpenThread CLI commands, and establish application-level communication between devices.

---

## Configuration Options

The behavior of the `miu-ftd` example is influenced by several build-time configuration options, typically found in the project's `sdk_config.h` or a similar configuration file.

* **`CONFIG_APP_TASK_CENTRAL_ENABLE`**:
    When enabled (`=1`), this option configures the device to facilitate the MIU Network Management functionality. Specifically, only a device with **GPIO 22 physically grounded** will be allowed to assume the Thread Leader role. This is crucial for managing the network centrally.
    * *For detailed information on MIU Network Management, refer to the [Network Management Guide](../Network-management-guid.md).*

* **`CONFIG_APP_TASK_CONTROL_CMD_ENABLE`**:
    This option must be enabled (`=1`) to activate the application-level command parsing functionality. Many of the MIU Network Management features and application-specific commands (like LED control) rely on this to interpret incoming UDP payloads.

---

## Network Parameter Settings

You can find the the default network configuration in `app_task.c` inside the `otdatasetInit` function. The example uses the following `AppNetworkConfig` structure to define the initial Thread network parameters:

```c
AppNetworkConfig netconfig = {
    .networkName = "Rafael Miu",
    .extPanId = {0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00},
    .networkKey = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff},
    .meshLocalPrefix = {0xfd, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00},
    .pskc = {0x74, 0x68, 0x72, 0x65, 0x61, 0x64, 0x6a, 0x70,
              0x61, 0x6b, 0x65, 0x74, 0x65, 0x73, 0x74, 0x00},
    .channel = 1,
    .panId = 0xabcd
};
```

If this is your first time running the device, or if you wish to reset all stored network parameters, it is highly recommended to reset the flash memory using the following command before promoting the device to a leader:

```bash
ot factoryreset
```

Otherwise, the device will retain previously saved network parameters from flash, which might interfere with new network formation.

---

## Sub-GHz Frequency & Data Rate Configuration

The MIU SDK leverages the Rafael RT58x's Sub-GHz capabilities, allowing flexible configuration of frequency bands and data rates. These are defined at **compile-time** using specific configuration macros, typically found in `sdk_config.h` or the project's `CMakeLists.txt`.

* **Frequency Band (e.g., `CONFIG_SUBG_FREQUENCY_BAND_915`)**:
    These macros determine the operating Sub-GHz frequency band for the device. Options include:
    * `CONFIG_SUBG_FREQUENCY_BAND_915` (915 MHz)
    * `CONFIG_SUBG_FREQUENCY_BAND_868` (868 MHz)
    * `CONFIG_SUBG_FREQUENCY_BAND_470` (470 MHz)
    * `CONFIG_SUBG_FREQUENCY_BAND_433` (433 MHz)
    By default, `HOSAL_RF_BAND_SUBG_915M` is often selected.

* **Data Rate (e.g., `CONFIG_SUBG_DATA_RATE_FSK_300K`)**:
    These macros select the physical layer data rate for Sub-GHz communication. Options include:
    * `CONFIG_SUBG_DATA_RATE_FSK_300K` (300 kbps FSK)
    * `CONFIG_SUBG_DATA_RATE_FSK_200K` (200 kbps FSK)
    * `CONFIG_SUBG_DATA_RATE_FSK_100K` (100 kbps FSK)
    * `CONFIG_SUBG_DATA_RATE_FSK_50K` (50 kbps FSK)
    * `CONFIG_SUBG_DATA_RATE_OQPSK_25K` (25 kbps OQPSK)
    The default is often `HOSAL_RF_PHY_DATA_RATE_300K`.

**Important:** For devices to communicate within the same mesh network, they **must be configured with the same frequency band and data rate** during compilation.

-----

## Getting Started: Forming a Mesh Network (Two Devices)

This section demonstrates how to set up a basic Mesh network using two `miu-ftd` devices. Ensure both devices have the `miu-ftd.bin` firmware flashed (refer to the [Quick Start Guide](../Quick-start.md) if needed).

**Conditional Setup based on `CONFIG_APP_TASK_CENTRAL_ENABLE`:**

* **If `CONFIG_APP_TASK_CENTRAL_ENABLE` is NOT enabled (or set to `0`):**
    You can manually promote any `miu-ftd` device to a Leader using the `ot state leader` command.
* **If `CONFIG_APP_TASK_CENTRAL_ENABLE` IS enabled (set to `1`):**
    Due to the Network Management functionality, only the device with **GPIO 22 physically grounded** will be able to successfully become the Leader. The other device(s) will automatically attempt to join the network.

### Device 1: Becoming the Leader

Upon booting Device 1, you will first see the default configuration output in the UART terminal:

```
Mesh It Up FTD
Band             : SubG_915M
Data Rate        : 300K
Active Timestamp : 1
Channel          : 1
Wake-up Channel  : 0
Ext PAN ID       : 000db80000000000
Mesh Local Prefix: fd00:0db8:0000:0000::/64
Network Key      : 00112233445566778899aabbccddeeff
Network Name     : Rafael Miu
Link Mode        : 1, 1, 1
PAN ID           : 0xabcd
Extaddr          : 5600000000000000
UDP PORT         : 0x162e
```

Then, check the device's current Thread network role. It should initially be `detached`:

```
Current role     : detached
```

To initiate the mesh network, use the following OpenThread CLI command to promote Device 1 to a **Leader**:

```bash
ot state leader
```

**Important consideration for `CONFIG_APP_TASK_CENTRAL_ENABLE = 1`:**
If this configuration is enabled, ensure Device 1 is the one with **GPIO 22 grounded** to allow it to become the Leader. Attempting to promote a non-grounded device to a Leader will likely fail or revert its role.

Once successfully promoted, you will see output similar to this, confirming its new role and network addresses:

```
Current role       : leader
Rloc16             : 4000
Extend Address     : 5600000000000000
RLOC IPv6 Address  : fd00:db8:0:0:0:ff:fe00:4000
Mesh IPv6 Address  : fd00:db8:0:0:5600:0:0:0
local IPv6 Address : fe80:0:0:0:5400:0:0:0
```

### Device 2: Joining the Mesh Network

Booting Device 2 will display the same initial configuration and detached role as Device 1:

```
Mesh It Up FTD
Band             : SubG_915M
Data Rate        : 300K
Active Timestamp : 1
Channel          : 1
Wake-up Channel  : 0
Ext PAN ID       : 000db80000000000
Mesh Local Prefix: fd00:0db8:0000:0000::/64
Network Key      : 00112233445566778899aabbccddeeff
Network Name     : Rafael Miu
Link Mode        : 1, 1, 1
PAN ID           : 0xabcd
Extaddr          : 0200000000000000
UDP PORT         : 0x162e
```

Initially, its role will also be `detached`:

```
Current role     : detached
```

Device 2 will automatically discover and attempt to join the mesh network established by Device 1 (the Leader). After successfully joining, its role will change, typically to a **Child** (and then potentially to a Router if the conditions are met, as FTDs can be Routers). You will observe output similar to:

```
Current role       : child
Rloc16             : 4002
Extend Address     : 0200000000000000
RLOC IPv6 Address  : fd00:db8:0:0:0:ff:fe00:4002
Mesh IPv6 Address  : fd00:db8:0:0:200:0:0:0
local IPv6 Address : fe80:0:0:0:0:0:0:0
```

At this point, you have successfully formed a two-device Thread Mesh network!

-----

## UDP Communication and Application Control Commands

The `miu-ftd` example includes basic UDP communication capabilities, allowing devices to send and receive data, including application-specific control commands. The MIU SDK provides specialized CLI commands to interact with these application-level features.

### 1. Sending Raw UDP Data (`app udp send`)

You can send arbitrary hexadecimal or string data as a UDP payload to a specific IPv6 address. This is useful for testing raw data transfer.

* **Syntax:**
    ```bash
    app udp send <ipv6> -x <hex data>
    app udp send <ipv6> -c <string data>
    ```
* **Example: Sending raw hexadecimal data:**
    Send `123456` as hexadecimal data to Device 2's Mesh IPv6 Address:
    ```bash
    app udp send fd00:db8:0:0:200:0:0:0 -x 123456
    ```

### 2. Sending Application Control Commands (`app ctrl`)

The app ctrl CLI command is available only when CONFIG_APP_TASK_CONTROL_CMD_ENABLE is enabled.
It allows structured application-level interactions using predefined control commands (defined in the command_id_t enumeration).

* **Syntax:**
    ```bash
    app ctrl <ipv6> <command id> [data]
    ```
    * `<ipv6>`: The target IPv6 address (e.g., Mesh Local EID of Device 2).
    * `<command id>`: <command id>: The hexadecimal ID of the control command (e.g., `0x03` for LED Toggle. see command_id_t definition).
    * `[data]`: Optional additional data for the command (e.g., `times` for `CMD_ID_LED_FLASH`).

* **Example: Triggering LED actions:**
    Send an LED Toggle command (ID `0x03`) to Device 2's Mesh IPv6 Address:
    ```bash
    app ctrl fd00:db8:0:0:200:0:0:0 0x03
    ```

### Other Application Commands (`app help`)

To see other basic application-level CLI commands, type `app help`:

```bash
app help
```

Output:

```
app udp send <ipv6> -x <hex data>
app udp send <ipv6> -c <string data>
app udp port
app led <on/off/toggle/flash>
app ctrl <ipv6> <cmd id> [data]
```

-----

## Setting Network Parameters via CLI

Beyond the default configuration, you can dynamically adjust various Thread network parameters using the OpenThread CLI.

```bash
ot dataset panid 0x1234
ot dataset commit active
```

These settings will be stored in flash memory and automatically loaded after a reboot. You can verify the changes using `ot dataset active`.

> 📚 **For more advanced dataset usage:**
> Refer to the full OpenThread [dataset CLI documentation](../../../../components/network/mesh-it-up/miu_openthread/src/cli/README_DATASET.md) for additional commands like `networkkey`, `channel`, `extpanid`, `masterkey`, etc.

> ❗ **Reminder:**
> When using OpenThread [commands](../../../../components/network/mesh-it-up/miu_openthread/src/cli#readme), remember to prefix them with `ot`.
> For example: `ot state`, `ot dataset`, `ot factoryreset`.

-----

## Next Steps

Now that you've successfully created a basic Mesh network with `miu-ftd` devices, you can explore more advanced functionalities:

* **Explore other examples:** Learn to run `miu-mtd` or `miu-sniffer` to understand different MIU example.
    * [`miu-mtd Example Guide`](miu-mtd-guide.md)
    * [`miu-sniffer Example Guide`](miu-sniffer-guide.md)
* **Learn about MIU Services:** Dive into Rafael's proprietary Network Management and OTA features.
    * [`Network Management Guide`](../Network-management-guid.md)
    * [`OTA Guide`](../OTA-guid.md)
* **Utilize MIU Tools:** Discover how to use additional tools, such as the Topology Tool.
    * [`Topology Tool Guide`](../Topology-guid.md)