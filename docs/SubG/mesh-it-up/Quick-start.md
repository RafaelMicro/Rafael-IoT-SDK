# Mesh It Up (MIU) Quick Start

This document guides you through the essential steps to quickly set up your development environment, compile and flash a basic MIU example, and verify its successful operation.

---

## 1. Prerequisites

Before you begin, ensure you have the following hardware:

* MIU Development Board: At least one Rafael RT58x series development board (e.g., an RT58X development board) for initial setup verification.

* USB Cable: To connect your development board to your PC.

* J-Link or CMSIS-DAP Debugger: Required for program flashing and debugging, especially when using the VS Code Extension.
 
### Figure 1: RT58X Development EVK Board
  <p align="center">
    <img src="../../picture/evk.png" alt="RT58X Development EVK Board" width="600"/>
  </p>


## 2. Development Environment Setup
All necessary steps for setting up your development environment, including software installation, SDK acquisition, and environment variable configuration, are detailed in our dedicated setup guide.

Please refer to the [`Rafael-IoT-SDK Setup Guide`](../../SDK_Setup/) for comprehensive instructions.

This guide covers:

* Installing required software (e.g., Toolchain, Python, Git).

* Obtaining the Rafael-IoT-SDK SDK source code.

* Configuring necessary environment variables.


## 3. Configure Frequency Band

The Sub-GHz frequency band is a **compile-time** setting. Before building, edit the `.config` file that matches your board:

**File:** `examples/sub-g/mesh-it-up/<example>/default-<board>.config`

Set the desired band to `=y` and mark the others as `is not set`:

```
CONFIG_SUBG_FREQUENCY_BAND_915=y
# CONFIG_SUBG_FREQUENCY_BAND_868 is not set
# CONFIG_SUBG_FREQUENCY_BAND_470 is not set
# CONFIG_SUBG_FREQUENCY_BAND_433 is not set
```

> **Note:** Ensure the selected frequency band complies with your local radio regulations.

For the full channel-to-frequency mapping of each band, see [`Frequency Band Reference`](frequency-band-reference.md).


## 4. First Build and Flash (miu-ftd)
Once your development environment is set up as per the [`Rafael-IoT-SDK Setup Guide`](../../SDK_Setup/), you can proceed to build and flash your first MIU FTD example. Our goal is to simply run the miu-ftd example on a single board to confirm the setup is working.

### 4.1 Select and Build miu-ftd Example:
### Figure 2: RT58X miu build ftd user space
  <p align="center">
    <img src="picture/miu-build-ftd-user-space.jpg" alt="RT58X miu build ftd user space" width="600"/>
  </p>

* Using the Rafael VS Code Extension (typically found in the sidebar or status bar), select the miu-ftd example and your target RT58x board.

* Click the extension's "Build" button.

* Verify that the compilation completes successfully in the VS Code TERMINAL panel. The compiled .bin file (e.g., miu-ftd.bin) will be located in the example's build folder.

### 4.2 Flash miu-ftd Example to Board:

* Ensure your J-Link or CMSIS-DAP debugger is correctly connected to your development board.

* In the Rafael MIU VS Code Extension, click the "Download" button for the miu-ftd example.

* Monitor the flashing process in the VS Code TERMINAL panel.

### 4.3 Connect UART Debug Tool:

* Connect your development board's debug UART interface to your PC using the USB to UART cable.

* Open a terminal program like Tera Term or PuTTY. Configure the correct COM Port and baud rate (e.g., 115200 bps).

### 4.4 Observe Boot Messages:

* After the board reboots (or manually reset it), you should see MIU boot messages and Thread network status output in your UART terminal.

Example Output:

```
Mesh It Up FTD
Band               : SubG_915M
Data Rate          : 300K
Active Timestamp   : 1
Channel            : 1
Wake-up Channel    : 0
Ext PAN ID         : 000db80000000000
Mesh Local Prefix  : fd00:0db8:0000:0000::/64
Network Key        : 00112233445566778899aabbccddeeff
Network Name       : Rafael Miu
Link Mode          : 1, 1, 1
PAN ID             : 0xabcd
Extaddr            : 5600000000000000
UDP PORT           : 0x162e
```

### 4.5 Verification: If you observe similar output, congratulations! Your development environment is correctly set up, and your first MIU example is running successfully.


## 5. Standard Operating Procedure (SOP) for Network Setup

This section guides you through the process of establishing a **Leader-Router-Child** network from scratch using three EVK boards.

### 5.1 Device Preparation and Flashing

Prepare 3 EVK development boards (assumed as EVK #1, #2, #3).

| Device | Firmware to Flash | Network Role | Notes |
| :--- | :--- | :--- | :--- |
| **EVK #1** | `miu-ftd.bin` | Leader | Automatically promotes to Leader after power-on. |
| **EVK #2** | `miu-ftd.bin` | Router/Child | Automatically attaches to the existing network. |
| **EVK #3** | `miu-mtd.bin` | Child | Automatically attaches to the existing network. |

### 5.2 Network Formation and Initialization

1.  **Factory Reset:** To ensure a clean start, it is recommended to factory reset the FTD devices (EVK #1 and EVK #2) before forming the network. On each FTD CLI terminal, run:
    ```bash
    factoryreset
    ```
    > **Note:** EVK #3 (MTD) has no CLI interface and cannot run `factoryreset`. To reset its stored credentials, perform a clean reflash of the firmware.
### 5.3 Leader Initialization (EVK #1)
1.  **Power On Leader (EVK #1):**
    When no existing Thread network is detected, EVK #1 will automatically form a new network and promote itself to Leader. This may take up to 1 minute.
2.  **Verify Leader State:**
    * In the CLI terminal for EVK #1, input:
        ```bash
        state
        ```
    * **Expected Output:** `leader`
    * If the device is still `detached` after 1 minute, you can manually force it:
        ```bash
        state leader
        ```
### 5.4 Router and MTD Automatic Attach

1.  **Power On Router (EVK #2) and MTD (EVK #3).**
2.  Both devices will automatically scan and join the network established by EVK #1 using default parameters.
3.  **Verify States:**
    * In the CLI terminal for EVK #2, input `state`. **Expected Output:** `router`
    * EVK #3 (MTD) has no CLI. Verify attachment by observing the UART boot log — it will print `Current role : child` once it joins.

---

## 6. Network Functionality Verification

We will verify application-layer connectivity using UDP and Ping commands from the Leader.

1.  **Get MTD's Mesh Local EID:**
    * EVK #3 (MTD) has no CLI. Its Mesh Local EID is printed in the UART boot log after attaching:
        ```
        Mesh IPv6 Address  : fd00:db8:0:0:200:0:0:0
        ```
    * Use this address as the target in the following commands.

2.  **Send UDP Command from Leader:**
    * Return to the Leader (EVK #1) CLI terminal and input the `app udp` command.
    * **Command Format:** `app udp send <Target EID/RLOC IPv6> -x <Hex Data>`
    * **Example:**
        ```bash
        app udp send fd00:db8:0:0:a0b4:c3d2:5e67:890a -x 123456
        ```
    * **Expected Result:** The MTD (EVK #3) should show 
        ```bash
        123456
        ```
3.  **Send ping Command from Leader:**
    * Return to the Leader (EVK #1) CLI terminal and input the `ping` command.
    * **Command Format:** `ping <Target EID/RLOC IPv6> <Size> <Count>`
    * **Example:**
        ```bash
        ping fd00:db8:0:0:a0b4:c3d2:5e67:890a 64 1
        ```
    * **Expected Result:**
        ```bash
        72 bytes from fd00:db8:0:0:a0b4:c3d2:5e67:890a: icmp_seq=1 hlim=64 time=39ms
        1 packets transmitted, 1 packets received. Packet loss = 0.0%. Round-trip min/avg/max = 39/39.0/39 ms.
        ```

---

## 7. Further Steps

You have successfully built and verified the MIU Mesh network. Please refer to the following guides for code details and advanced features:

* **Code and Feature Deep Dive:**
    * [`miu-ftd Example Guide`](example/miu-ftd-guide.md)
    * [`miu-mtd Example Guide`](example/miu-mtd-guide.md)
* **Diagnostics and Debugging:**
    * [`miu-sniffer Example Guide`](example/miu-sniffer-guide.md)
* **Advanced Features (To be completed):**
    * [`OTA Guide`](OTA-guide.md)