# miu-ftd Example Guide

## 1. What is the miu-ftd Example?

The `miu-ftd` example serves as the reference implementation for an **OpenThread FTD (Full Thread Device)** within the Mesh It Up (MIU) SDK. It integrates Sub-GHz connectivity for robust and automatic mesh networking capabilities. This example demonstrates how to configure core network parameters, utilize OpenThread CLI commands, and establish application-level communication between devices.

---

## 2.Configuration Options

The behavior of the `miu-ftd` example is influenced by several build-time configuration options, typically found in the project's `sdk_config.h` or a similar configuration file.

* **`CONFIG_APP_TASK_CENTRAL_ENABLE`**:
    When enabled (`=1` - Default), this configuration activates the core logic for the **MIU Network Management Central Role**, which is essential for proprietary MIU services.
    * **Impact when enabled (`=1` - Default):**
        1. **Leader Enforcement:** The device will only assume the Thread Leader role if this configuration is enabled **AND** **GPIO 22 is physically grounded** (Low Level).
        2. **Network Management Registration:** Enables sending network registration packets to the Leader.
        3. **Topology Tool Support:** Necessary for the Leader to gather and report network topology information.
    * **Impact when disabled (`=0`):**
        1. **MIU Services Disabled:** All proprietary MIU services, including the **Topology Tool** and specific centralized management functions, are disabled.
        2. **Standard OpenThread Join:** The device will revert to standard OpenThread behavior for joining/forming a network, ignoring the GPIO 22 mechanism.
    * *For detailed information on MIU Network Management, including the proprietary joining process, refer to the [Network Management Guide](../Network-management-guide.md).*

* **`CONFIG_APP_TASK_CONTROL_CMD_ENABLE`**:
    This option must be enabled (`=1` - Default) to activate the application-level command parsing functionality.
    * **Impact when enabled (`=1`):**
        1. **Control Command Handler:** Enables the task that processes incoming UDP packets based on the `app ctrl` command format.
        2. **Remote Application Control:** Essential for features like the remote **LED Toggle** command demonstrated in the Quick Start Guide.
    * **Impact when disabled (`=0`):**
        1. **`app ctrl` Commands Disabled:** The device will not respond to application-layer control commands (including those used by the Topology Tool or custom applications). It will only support standard raw UDP data exchange.

* **`CONFIG_APP_TASK_OTA_ENABLE`**:
    When enabled (`=1`), this configuration activates the OTA service and task.
    * **Impact:** Enables the device to receive and process over-the-air firmware updates.

---

## 3. Application Structure and Core Functions (app_task.c Deep Dive)

The `app_task.c` file implements the core application logic, utilizing FreeRTOS for task management and integrating the various MIU proprietary features on top of OpenThread.

### 3.1 Application Tasks and Event Handling
The main application logic runs within a FreeRTOS task (`app_task`). It uses a **Queue** (`appEventQueue`) and a **Semaphore** (`appSemHandle`) for thread-safe communication and event processing.

* **`ot_stateChangeCallback`**: This is a critical function registered with the OpenThread stack. When the device's Thread state changes (e.g., to Leader, Router, or Child), this callback is triggered.
    * It logs the device's new role and its key IPv6 addresses (RLOC, Mesh Local EID).
    * It sends an application event (`APP_EVENT_CHANGE_ROLE`) to the main application queue to update application-specific behavior, such as LED indication.

### 3.2 Application Layer Services
The `app_task.c` integrates and utilizes the following MIU services (enabled via configuration flags):
* **Network Management (app_net_mgm.h)**: Centralized network orchestration.
* **Application Control Commands (app_control_cmd.h / app_udp.h)**: Handles incoming UDP packets and parses them into executable control commands (e.g., LED Toggle).
* **Over-the-Air (OTA) (app_ota.h)**: Manages firmware updates.
* **LED Control (app_led.h)**: Manages application-specific LED behaviors.

---

## 4. LED Indicator and GPIO Pinout

The application uses an application-specific LED to visually indicate the device's network state, which is handled by the `app_led.h` module and triggered by the role change in `ot_stateChangeCallback`.

| Network State | Leader LED Behavior | Other LED Behavior |
| :--- | :--- | :--- |
| **Out-of-Network** | All LED Off | LED 0 Blinking |
| **Network Joined** | All LED Blinking | LED 1 Blinking|

> 📌 **GPIO Pinout:** The application LED is physically connected to **[LED 0: gpio20 or gpio15, LED 1: gpio21 or gpio14]**. Developers should refer to the device schematic or `app_led.h` for the specific EVK used.

---

## 5. Common OpenThread and MIU CLI Commands

The device supports both the standard OpenThread CLI and custom MIU application commands, all accessible via the serial port.

### 5.1 Standard OpenThread Commands
All standard OpenThread commands must be prefixed with `ot`.

| Command | Purpose | Example |
| :--- | :--- | :--- |
| `ot state` | Returns the device's current Thread role (e.g., leader, router). | `ot state` |
| `ot dataset` | Manages the Active or Pending Operational Dataset (network parameters). | `ot dataset panid 0x1234` |
| `ot ipaddr` | Displays various IPv6 addresses (Mesh Local, Link Local, etc.). | `ot ipaddr mleid` |
| `ot ping` | Sends an ICMPv6 Echo Request (ping) to another node. | `ot ping fd00::xxxx` |
| `ot factoryreset` | Erases all Thread network credentials stored in flash memory. | `ot factoryreset` |

### 5.2 MIU Application Commands (Proprietary)
The proprietary commands are available under the `app` namespace, implemented in `app_task.c` via the `_cli_cmd_miu_app` function. These commands are essential for leveraging the MIU proprietary services.

| Command | Purpose | Configuration Dependency |
| :--- | :--- | :--- |
| `app ctrl <Target EID> <CMD ID>` | Sends a remote application control command to a target node (e.g., LED toggle, custom commands). | `CONFIG_APP_TASK_CONTROL_CMD_ENABLE` |
| `app node list` | **(Leader Only)** Displays the current list of registered nodes and their network information. | `CONFIG_APP_TASK_CENTRAL_ENABLE` |
| `app node num` | **(Leader Only)** Displays the number of registered nodes in the network. | `CONFIG_APP_TASK_CENTRAL_ENABLE` |
| `app node kick <Target EID> [Time]` | **(Leader Only)** Instructs a node to leave the network for a specified duration (default 30 seconds). | `CONFIG_APP_TASK_CENTRAL_ENABLE` |

> 🔗 **Network Management Commands:** The `app node` commands directly utilize the MIU Network Management service. For detailed packet structure and protocol flow, refer to the [`Network Management Guide`](../Network-management-guide.md).

---

## 6. Network Parameter Settings

You can find the default network configuration in `app_task.c` inside the `otdatasetInit` function. The example uses the following `AppNetworkConfig` structure to define the initial Thread network parameters:

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

If `CONFIG_APP_TASK_CENTRAL_ENABLE` is enabled, the leader will use the default `AppNetworkConfig` settings. If not, the settings in flash will be used. Other devices will enter the join state, waiting to obtain network settings. If network settings are already available in flash, they will attach directly. 

If `CONFIG_APP_TASK_CENTRAL_ENABLE` is disabled and no network settings are available in flash, the **device will use** the default settings.

---

## 7. Sub-GHz Frequency & Data Rate Configuration

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

## 8. Getting Started: Forming a Mesh Network (Two Devices)

This section demonstrates how to set up a basic Mesh network using two `miu-ftd` devices. Ensure both devices have the `miu-ftd.bin` firmware flashed (refer to the [Quick Start Guide](../Quick-start.md) if needed).

**Conditional Setup based on `CONFIG_APP_TASK_CENTRAL_ENABLE`:**

* **If `CONFIG_APP_TASK_CENTRAL_ENABLE` is NOT enabled (or set to `0`):**
    You can manually promote any `miu-ftd` device to a Leader using the `ot state leader` command.
* **If `CONFIG_APP_TASK_CENTRAL_ENABLE` IS enabled (set to `1`):**
    Due to the Network Management functionality, only the device with **GPIO 22 physically grounded** will be able to successfully become the Leader. The other device(s) will automatically attempt to join the network.

### 8.1 Device 1: Becoming the Leader

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

### 8.2 Device 2: Joining the Mesh Network

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

Network Joining Process (When no settings are stored in flash)
If no network settings are stored in flash memory (i.e., this is the device's first boot or settings have been cleared), Device 2 will execute the Joiner process to try and join the existing Thread network.

#### 8.2.1 Initial Role: The device's initial role will be disabled or a similar non-Thread role.

```
Current role     : disabled
```

#### 8.2.2 Scanning and Join Request: The device begins scanning channels and attempts to find and join a network (i.e., sending Join Requests).

```
[Network] >> Join Request (channel: 1)
[Network] >> Join Request (channel: 2)
[Network] >> Join Request (channel: 3)
[Network] >> Join Request (channel: 4)
[Network] >> Join Request (channel: 5)
[Network] >> Join Request (channel: 6)
[Network] >> Join Request (channel: 7)
[Network] >> Join Request (channel: 8)
[Network] >> Join Request (channel: 9)
[Network] >> Join Request (channel: 10)
```

#### 8.2.3 Receiving Network Credentials: The device waits for the Provisioning Join Window to be opened by the Leader/Commissioner to receive network credentials.

```
[Network] << Join Response [9E7C081DFE7D7BAE]
join panid: abcd
join key:
[join key]: 0000-000F: 00 11 22 33 44 55 66 77  88 99 AA BB CC DD EE FF
```

#### 8.2.4 **Transition to** `detached`: After successfully receiving and storing the network credentials, the device starts the **Attach** process to connect to the network. Its role transitions to `detached`, indicating it possesses network information but has not yet established a link with a parent node.

```
Current role     : detached
```

#### 8.2.5 **Successful Join (Attach)**: Upon a successful attach, the device's role will transition, typically becoming a **Child** (and potentially a Router later, as FTDs are Router-Eligible).

**Successful Network Join (Attach Completed)**

Once Device 2 has successfully joined the network, you will observe output similar to the following:

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

## 9. UDP Communication and Application Control Commands

The `miu-ftd` example includes basic UDP communication capabilities, allowing devices to send and receive data, including application-specific control commands. The MIU SDK provides specialized CLI commands to interact with these application-level features.

### 9.1 Sending Raw UDP Data (`app udp send`)

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

### 9.2 Sending Application Control Commands (`app ctrl`)

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

### 9.3 Other Application Commands (`app help`)

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

## 10. Setting Network Parameters via CLI

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

## 11. Next Steps

Now that you've successfully created a basic Mesh network with `miu-ftd` devices, you can explore more advanced functionalities:

* **Explore other examples:** Learn to run `miu-mtd` or `miu-sniffer` to understand different MIU example.
    * [`miu-mtd Example Guide`](miu-mtd-guide.md)
    * [`miu-sniffer Example Guide`](miu-sniffer-guide.md)
* **Learn about MIU Services:** Dive into Rafael's proprietary Network Management and OTA features.
    * [`Network Management Guide`](../Network-management-guide.md)
    * [`OTA Guide`](../OTA-guide.md)
* **Utilize MIU Tools:** Discover how to use additional tools, such as the Topology Tool.
    * [`Topology Tool Guide`](../Topology-Tool-guide.md)