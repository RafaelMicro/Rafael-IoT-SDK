# miu-mtd Example Guide

## 1. What is the miu-mtd Example?

The 'miu-mtd' example demonstrates a Minimal Thread Device (MTD) implementation. It is specifically optimized for battery-powered devices that remain in a low-power sleep state most of the time. The device periodically wakes up to "poll" its parent (FTD) for messages, ensuring extreme energy efficiency while staying part of the mesh network.

---

## 2. Configuration Options

The `miu-mtd` example behavior is controlled by several build-time options:

* **`CONFIG_HOSAL_SOC_IDLE_SLEEP = y`**:
    Enables MCU idle-sleep capability. The MTD will automatically enter low-power sleep mode when no active task is running.

* **`CONFIG_HOSAL_SOC_SLEEP_TIMER_ID = 4`**:
    Defines the hardware timer used to maintain timing accuracy during sleep and wake-up cycles.

* **`CONFIG_APP_TASK_OTA_ENABLE`**:
    When enabled (`=1`), this configuration activates the OTA service and task.
    * **Impact:** Enables the device to receive and process over-the-air firmware updates.

---

## 3. Application Structure (app_task.c Deep Dive)

### 3.1 Task Synchronization and Safety
The application logic runs within the app_task. For MTD, the Notification mechanism is the primary driver for power management.

* **Task Notification (ulTaskNotifyTake):**  
The task remains in a blocked state. While blocked, the SoC enters its low-power sleep mode (Idle-Sleep). It only wakes up when the radio receives data, a timer expires, or a local interrupt (like a button press) occurs.

* **Thread Safety (OT_THREAD_SAFE):** 
Critical Rule: Always wrap OpenThread API calls in OT_THREAD_SAFE. This is vital on MTD to ensure the stack is protected when the device transitions between sleep and active modes.

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

### 3.2 Application Layer Services
The `app_task.c` integrates and utilizes the following MIU services (enabled via configuration flags):
* **Over-the-Air (OTA) (app_ota.h)**: Manages firmware updates.
* **LED Control (app_led.h)**: Manages application-specific LED behaviors.
* **UDP Control (app_udp.h)**: Manages application-specific UDP behaviors.
* **UART Control (app_uart.h)**: Manages application-specific UART behaviors.

---

## 4. No CLI Interface

The `miu-mtd` example does **not** expose a CLI interface. All behavior is driven by the dataset stored in flash at compile/flash time. There are no runtime commands to enter — the device boots, reads its configuration, and immediately begins network operations autonomously.

To change network parameters (e.g. channel, PAN ID, network key), update the values in `app_task.c` and reflash the firmware.

---

## 5. Sub-GHz Frequency & Data Rate Configuration

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

## 6. Network Attach Flow

The MTD does not accept CLI input for network control. On boot, it reads the Thread dataset stored in flash (network name, PAN ID, network key, channel, etc.) and immediately begins scanning for a parent FTD to attach to. No manual commands are needed.

> On first boot (or after a clean flash), the MTD will use the compiled-in default dataset and start scanning. Ensure at least one FTD (Leader/Router) using the same dataset is already running on the network before powering on the MTD.

On boot, the following configuration is printed to the UART terminal:

```
Mesh It Up MTD
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

The device starts in `detached` state and will continuously attempt to find and join the network:

```
Current role     : detached
```

**Successful Network Join (Attach Completed)**

Once the MTD has successfully attached to a parent FTD, you will observe output similar to the following:

```
Current role       : child
Rloc16             : 4002
Extend Address     : 0200000000000000
RLOC IPv6 Address  : fd00:db8:0:0:0:ff:fe00:4002
Mesh IPv6 Address  : fd00:db8:0:0:200:0:0:0
local IPv6 Address : fe80:0:0:0:0:0:0:0
```

If the device does not transition to `child` within about 1 minute, verify that the FTD parent is running and that both devices are compiled with the same frequency band, data rate, and network parameters.


##  7. Power Saving Behavior

The MTD example prioritizes low power operation.
When idle, the system enters deep sleep through hosal_soc_sleep() while maintaining accurate time using CONFIG_HOSAL_SOC_SLEEP_TIMER_ID.

After wake-up:

* System time is compensated automatically.

* Pending events such as join retries or network messages resume seamlessly.

## 8. Changing Network Parameters

Because the MTD has no CLI, network parameters cannot be changed at runtime. To use a different network configuration:

1. Update the `AppNetworkConfig` struct in `app_task.c` with the desired values (channel, PAN ID, network key, etc.).
2. Rebuild and reflash the firmware.

-----

## 9. Next Steps

Now that you've successfully created a basic Mesh network with `miu-mtd` devices, you can explore more advanced functionalities:

* **Explore other examples:** Learn to run `miu-ftd` or `miu-sniffer` to understand different MIU example.
    * [`miu-ftd Example Guide`](miu-ftd-guide.md)
    * [`miu-sniffer Example Guide`](miu-sniffer-guide.md)
* **Learn about MIU Services:** Dive into Rafael's proprietary OTA features.
    * [`OTA Guide`](../OTA-guide.md)