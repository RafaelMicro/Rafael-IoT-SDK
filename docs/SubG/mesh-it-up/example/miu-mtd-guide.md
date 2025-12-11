# miu-mtd Example Guide

## What is the miu-mtd Example?

The `miu-mtd` example demonstrates a Minimal Thread Device (MTD) implementation in the Mesh It Up (MIU) SDK.
It is designed for low-power end devices that join an existing Thread network via the provisioning (commissioning) process.
Compared with the Full Thread Device (FTD), it focuses on energy efficiency and simplified network participation, while maintaining compatibility with MIU Network Management and application-level command handling.

---

## Configuration Options

The `miu-mtd` example behavior is controlled by several build-time options:

* **`CONFIG_APP_TASK_CENTRAL_ENABLE`**:
    When enabled (`=1`), the device can be managed through MIU Network Management.
    Only when it is turned on will the registration information be sent to the leader.

* **`CONFIG_APP_TASK_CONTROL_CMD_ENABLE`**:
    Enables application-level control commands (e.g., LED control, custom command packets).
    When disabled, the device only supports basic Thread join and UDP data exchange.

* **`CONFIG_HOSAL_SOC_IDLE_SLEEP = y`**:
    Enables MCU idle-sleep capability. The MTD will automatically enter low-power sleep mode when no active task is running.

* **`CONFIG_HOSAL_SOC_SLEEP_TIMER_ID = 4`**:
    Defines the hardware timer used to maintain timing accuracy during sleep and wake-up cycles.

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

## Network Join Flow

Unlike the FTD, which can form a network with default parameters when flash is empty,
the MTD must actively perform a join process to obtain the Thread credentials (Network Key, PAN ID, Channel, etc.) from a Provisioner (Leader/Router).

Provisioning Sequence Overview

The MTD acts as a Joiner, while the FTD or Leader acts as a Provisioner.

| Step	| Joiner (MTD) Action	| Provisioner (Leader/Router) Action |
| :--- | :--- | :--- |
| 1	| Power on and scan available channels	| Waits for join requests |
| 2	| Send JOIN REQUEST	| Responds with JOIN RESPONSE (Key, PAN ID) if provisioning window is open |
| 3	| Receive JOIN RESPONSE	| Stores received parameters and starts standard MLE Attach |
| 4	| Join successful	| Closes provisioning window or times out | 


##  Process Diagram

### Figure 1: Flow Overview
  <p align="center">
    <img src="../../../picture/miu_mtd_join_flow.png" alt="Flow Overview" width="600"/>
  </p>

The Joiner repeatedly scans channels and sends join requests until it receives a valid Join Response from the Provisioner.
If all channels are tried without success, it waits for a random backoff period before retrying.

---
##  Power Saving Behavior

The MTD example prioritizes low power operation.
When idle, the system enters deep sleep through hosal_soc_sleep() while maintaining accurate time using CONFIG_HOSAL_SOC_SLEEP_TIMER_ID.

After wake-up:

* System time is compensated automatically.

* Pending events such as join retries or network messages resume seamlessly.


## Getting Started: Joining a Network (Two Devices)

### Device 1: Leader/Provisioner (using miu-ftd)

1. Flash miu-ftd.bin and start it as the Leader (ot state leader).

2. Open the provisioning window by command(app provisioner <start/stop> <time (s)> ) or automatically when the network starts.

Device 2 — Joiner (using miu-mtd)

1. Flash miu-mtd.bin.

2. Power on — device starts scanning channels.

3. It will send JOIN REQUEST frames.

4. When the Provisioner window is open, it receives JOIN RESPONSE (Key, PAN ID).

5. Once network parameters are received, it attaches to the Thread network.

CLI output example:

```
Mesh It Up MTD
Band             : SubG_915M
Data Rate        : 300K
Active Timestamp : 1
Channel          : 1
Ext PAN ID       : 000db80000000000
Mesh Local Prefix: fd00:0db8:0000:0000::/64
Network Key      : 00112233445566778899aabbccddeeff
Network Name     : Rafael Miu
Link Mode        : 1, 0, 0
PAN ID           : 0xabcd
Extaddr          : 5600000000000000
UDP PORT         : 0x162e
```

At this point, you have successfully formed a two-device Thread Mesh network!

-----

## UDP Communication and Application & Control Commands

The `miu-mtd` example includes basic UDP communication capabilities, allowing devices to send and receive data, including application-specific control commands. The MIU SDK provides specialized CLI commands to interact with these application-level features.

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
    * [`miu-ftd Example Guide`](miu-ftd-guide.md)
    * [`miu-sniffer Example Guide`](miu-sniffer-guide.md)
* **Learn about MIU Services:** Dive into Rafael's proprietary Network Management and OTA features.
    * [`Network Management Guide`](../Network-management-guid.md)
    * [`OTA Guide`](../OTA-guid.md)
* **Utilize MIU Tools:** Discover how to use additional tools, such as the Topology Tool.
    * [`Topology Tool Guide`](../Topology-guid.md)