# miu-ftd Example Guide

## 1. What is the miu-ftd Example?

The `miu-ftd` example serves as the reference implementation for an **OpenThread FTD (Full Thread Device)** within the Mesh It Up (MIU) SDK. It integrates Sub-GHz connectivity for robust and automatic mesh networking capabilities. This example demonstrates how to configure core network parameters, utilize OpenThread CLI commands, and establish application-level communication between devices.

---

## 2.Configuration Options

The behavior of the `miu-ftd` example is influenced by several build-time configuration options, typically found in the project's `sdk_config.h` or a similar configuration file.

* **`CONFIG_APP_TASK_OTA_ENABLE`**:
    When enabled (`=1`), this configuration activates the OTA service and task.
    * **Impact:** Enables the device to receive and process over-the-air firmware updates.

---

## 3. Application Structure and Core Functions (app_task.c Deep Dive)

The `app_task.c` manages the main application lifecycle using FreeRTOS tasks.

### 3.1 Task Synchronization and Safety
The application logic runs within the app_task. To ensure system stability, it employs a Notification mechanism and strict Thread-Safe wrappers.
* **Task Notification (ulTaskNotifyTake):**     
    The task remains in a blocked state until notified, reducing CPU usage.
* **Thread Safety (OT_THREAD_SAFE):**    
    Critical Rule: Always wrap OpenThread API calls in OT_THREAD_SAFE to prevent race conditions with the radio task.

Core Loop Example:
```c
void app_task(void) {
    while (true) {
        otInstance* instance = NULL;
        
        /* Always wrap OpenThread API calls in OT_THREAD_SAFE 
           to prevent race conditions with the radio task. */
        OT_THREAD_SAFE(instance = otrGetInstance();)
        
        if (instance) {
            /* Application logic starts here... */
        }
        
        // Wait for next event/notification
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
    }
}
```

* **`ot_stateChangeCallback`**: Triggered when the device's Thread role changes. It now directly notifies the app_task to handle state updates.

### 3.2 Application Services
The `app_task.c` integrates and utilizes the following MIU services (enabled via configuration flags):
* **OTA (app_ota.h)**: Manages firmware updates.
* **LED Control (app_led.h)**: Manages application-specific LED behaviors.
* **UDP Service (app_udp.h)**: Manages application-specific UDP behaviors.
* **UART Control (app_uart.h)**: Manages application-specific UART behaviors.

---

## 4. Common OpenThread and MIU CLI Commands

The device supports both the standard OpenThread CLI and custom MIU application commands, all accessible via the serial port.

### 4.1 Standard OpenThread Commands
The device supports standard OpenThread commands and custom MIU application commands.

| Command | Purpose | Example |
| :--- | :--- | :--- |
| `state` | Returns the device's current Thread role (e.g., leader, router). | `state` |
| `dataset` | Manages the Active or Pending Operational Dataset (network parameters). | `dataset panid 0x1234` |
| `ipaddr` | Displays various IPv6 addresses (Mesh Local, Link Local, etc.). | `ipaddr mleid` |
| `ping` | Sends an ICMPv6 Echo Request (ping) to another node. | `ping fd00::xxxx` |
| `factoryreset` | Erases all Thread network credentials stored in flash memory. | `factoryreset` |

### 4.2 MIU Application Commands (Proprietary)
The proprietary commands are available under the `app` namespace, implemented in `app_task.c` via the `_cli_cmd_miu_app` function. These commands are essential for leveraging the MIU proprietary services.

| Command | Purpose |
| :--- | :--- |
| `app udp send <Target IPv6> -x <Hex data>` | Sends a udp hex data to a target node. |
| `app udp send <Target IPv6> -c <String data>` | Sends a udp string data to a target node. |
| `app udp port` | Displays the current udp port. |
| `app led <on/off/toggle/flash>` | Control Led 0 behaviors. |
| `app mem` | Check dynamic memory alloc information. |

---

## 5. Network Parameter Settings

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
factoryreset
```

---

## 6. Sub-GHz Frequency & Data Rate Configuration

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

## 7. Getting Started: Forming a Mesh Network (Two Devices)

This section demonstrates how to set up a basic Mesh network using two `miu-ftd` devices. Ensure both devices have the `miu-ftd.bin` firmware flashed (refer to the [Quick Start Guide](../Quick-start.md) if needed).

### 7.1 Device 1: Becoming the Leader

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
state leader
```

### 7.2 Device 2: Joining the Mesh Network

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

Then, check the device's current Thread network role. It should initially be `detached`:

```
Current role     : detached
```

**Successful Network Join (Attach Completed)**

Device 2 will automatically scan and attach to Device 1's network. This typically completes within about 1 minute. If the device remains `detached` after 1 minute, verify that both devices are compiled with the same frequency band, data rate, and network parameters.

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

## 8. UDP Communication and Application Control Commands

The `miu-ftd` example includes basic UDP communication capabilities, allowing devices to send and receive data, including application-specific control commands. The MIU SDK provides specialized CLI commands to interact with these application-level features.

### 8.1 Sending Raw UDP Data (`app udp send`)

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

### 8.2 Other Application Commands (`app help`)

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
app mem
```

-----

## 9. Setting Network Parameters via CLI

Beyond the default configuration, you can dynamically adjust various Thread network parameters using the OpenThread CLI.

```bash
dataset panid 0x1234
dataset commit active
```

These settings will be stored in flash memory and automatically loaded after a reboot. You can verify the changes using `dataset active`.

> 📚 **For more advanced dataset usage:**
> Refer to the full OpenThread [dataset CLI documentation](../../../../components/network/mesh-it-up/miu_openthread/src/cli/README_DATASET.md) for additional commands like `networkkey`, `channel`, `panid`, etc.

> ❗ **Reminder:**
> For a full list of available OpenThread CLI commands, refer to the [OpenThread CLI README](../../../../components/network/mesh-it-up/miu_openthread/src/cli#readme).
> Common examples: `state`, `dataset`, `factoryreset`.

-----

## 10. Next Steps

Now that you've successfully created a basic Mesh network with `miu-ftd` devices, you can explore more advanced functionalities:

* **Explore other examples:** Learn to run `miu-mtd` or `miu-sniffer` to understand different MIU example.
    * [`miu-mtd Example Guide`](miu-mtd-guide.md)
    * [`miu-sniffer Example Guide`](miu-sniffer-guide.md)
* **Learn about MIU Services:** Dive into Rafael's proprietary OTA features.
    * [`OTA Guide`](../OTA-guide.md)