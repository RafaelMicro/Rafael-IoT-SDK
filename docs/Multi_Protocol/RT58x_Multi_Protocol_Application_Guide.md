# RT58x Zigbee/BLE Multi-Protocol Application Guide

## Introduction

A multi-protocol application runs two protocol stacks concurrently on a single RT58x SoC: the
Zigbee stack over IEEE 802.15.4 and the Bluetooth LE host over BLE, sharing one 2.4 GHz radio.
This lets a product keep its Zigbee functionality while offering a BLE channel for provisioning,
diagnostics, serial pass-through and firmware update.

This guide describes the architecture and the application development flow using
[`examples/zigbee-ble/zb-ble-lighting-app`](../../examples/zigbee-ble/zb-ble-lighting-app/), which
combines:

* a **Zigbee dimmable light** (Basic, Identify, Groups, Scenes, On/Off, Level Control, plus an OTA
  Upgrade client), and
* a **BLE peripheral** exposing the Transparent Service (TRSP) for serial pass-through and the FOTA
  service for firmware update over BLE.

Single-protocol development is covered by
[Zigbee Application Note](../Zigbee/zigbee_application_note.md) and
[RT58x BLE SDK Application Guide](../BLE/RT58x_BLE_SDK_Application_Guide.md); this guide focuses on
what is specific to running both at once.

## Table of Contents

1. [System Architecture](#1-system-architecture)
2. [Application Development](#2-application-development)
3. [BLE Transparent Service (TRSP)](#3-ble-transparent-service-trsp)
4. [Running the Demo](#4-running-the-demo)
5. [Firmware Update (OTA)](#5-firmware-update-ota)
6. [References](#references)

---

## 1. System Architecture

### 1.1 Multi-Protocol RF Mode

Both stacks share one radio, so the RF subsystem must be started in multi-protocol mode and the
multi-protocol RF firmware must be linked into the image. Three things have to agree:

| Where | Setting |
|---|---|
| `src/main.c` | `hosal_rf_init(HOSAL_RF_MODE_MULTI_PROTOCOL)` |
| `default-*.config` | `CONFIG_RF_FW_INCLUDE_MULTI_2P4G=y`, `CONFIG_RF_MCU_CONST_LOAD_SUPPORTED=y` |
| `CMakeLists.txt` | `sdk_add_compile_options(-DSET_SYS_CLK=1)` |

`HOSAL_RF_MODE_MULTI_PROTOCOL` loads the 2.4 GHz multi-protocol RF firmware, which time-shares the
radio between the BLE link layer and the 802.15.4 MAC. A single-protocol build (for example
`CONFIG_RF_FW_INCLUDE_PCI=y` with `HOSAL_RF_MODE_RUCI_CMD`) cannot serve both stacks — the second
stack to start will fail to transmit.

The BLE host and its services also have to be enabled in the configuration:

```
CONFIG_BUILD_COMPONENT_BLUETOOTH=y
CONFIG_BUILD_COMPONENT_BLUETOOTH_LE_HOST=y
CONFIG_BLUETOOTH_LE_SERVICE_GAP=y
CONFIG_BLUETOOTH_LE_SERVICE_GATT=y
CONFIG_BLUETOOTH_LE_SERVICE_DIS=y
CONFIG_BLUETOOTH_LE_SERVICE_TRSP=y
CONFIG_BLUETOOTH_LE_SERVICE_FOTA=y
CONFIG_BLE_SUPPORT_NUM_CONN_MAX=1
CONFIG_BUILD_COMPONENT_ZIGBEE=y
```

### 1.2 Task Architecture

```
  +---------------------------- User Application ----------------------------+
  |                                                                          |
  |   app_main_loop()  "app-zigbee"          ble_trsp_task()  "app-ble"      |
  |        ^                                       ^                         |
  |        | task notification                     | g_app_msg_q (40 msgs)   |
  |        | + g_zb_app_evt_var bitmask            |                         |
  |        |                                       |                         |
  |   zboss_signal_handler()                  ble_evt_handler()              |
  |   zigbee_zcl_msg_handler()                ble_svcs_trsps_evt_handler()   |
  |                                           ble_svcs_fotas_evt_handler()   |
  +--------|-------------------------------------------|---------------------+
           |                                           |
  +--------v----------------------+     +--------------v---------------------+
  |  zbStackTask  "zb-thread"     |     |  BLE host task                     |
  |  Zigbee 3.0 & network stack   |     |  HCI TX task                       |
  |                               |     |  BLE host stack + GATT services    |
  +--------|----------------------+     +--------------|---------------------+
           |                                           |
           |  802.15.4 frames                          |  HCI commands / ACL
           |                                           |
  +--------v-------------------------------------------v---------------------+
  |  __rf_proc  "hoasl-rf"                                                   |
  |  Multi-protocol 2.4 GHz RF firmware  (802.15.4 MAC + BLE link layer)     |
  +--------|-----------------------------------------------------------------+
           |
  +--------v-----------------------------------------------------------------+
  |  HOSAL peripheral drivers (GPIO, UART, PWM, Flash, WDT, LPM)             |
  +--------|-----------------------------------------------------------------+
           |
  +--------v-----------------------------------------------------------------+
  |  RT58x hardware platform                                                 |
  +--------------------------------------------------------------------------+
```

Each block is a separate FreeRTOS task. The tasks are created by `zigbee_app_init()`
(`app-zigbee`), `zbStart()` (`zb-thread`), `ble_trsp_app_init()` (`app-ble`, plus the BLE host and
HCI TX tasks through `ble_host_stack_init()`) and `hosal_rf_init()` (`hoasl-rf`); the task
parameters live in those calls and in
[`rtos/config/rt582/FreeRTOSConfig.h`](../../examples/zigbee-ble/zb-ble-lighting-app/rtos/config/rt582/FreeRTOSConfig.h).

The two application tasks are deliberately independent: neither blocks on the other, so a slow
Zigbee operation (network steering, flash write) cannot stall the BLE connection and vice versa.
Both stacks run at a higher priority than either application task, so protocol timing is preserved
even when the application is busy.

### 1.3 Concurrency Rules

Because two application tasks now exist alongside two stacks, three rules must be followed.

**1. Wrap every ZBOSS call in `ZB_THREAD_SAFE()`.** The Zigbee stack runs in `zbStackTask`, so any
ZBOSS API invoked from `app-zigbee`, from `app-ble` or from a timer callback must take the stack
lock (`zbLock()` / `zbUnlock()`):

```c
/* From the BLE task: a TRSP command triggers a Zigbee factory reset. */
ZB_THREAD_SAFE(zigbee_do_factory_reset();)

/* From the application task: read network parameters after joining. */
ZB_THREAD_SAFE(g_panid          = zb_get_pan_id();
               g_short_addr     = zb_get_short_address();
               g_joined_channel = zb_get_current_channel();)
```

Use `ZB_THREAD_SAFE_RET(ret, ...)` when the call returns a value. Code already running in stack
context — `zigbee_zcl_msg_handler()` and `zboss_signal_handler()` — holds the lock and must not
take it again. Both macros are defined in
[`components/network/zigbee/zboss_port/include/zigbee_platform.h`](../../components/network/zigbee/zboss_port/include/zigbee_platform.h).

**2. Keep the two event mechanisms separate.** The Zigbee side uses a task notification plus an
event bitmask; the BLE side uses a FreeRTOS queue guarded by counting semaphores. They are not
interchangeable:

| | Zigbee application task | BLE application task |
|---|---|---|
| Signal | `ZIGBEE_APP_NOTIFY(ebit)` / `ZIGBEE_APP_NOTIFY_ISR(ebit)` | `ble_app_request_set(host_id, request, from_isr)` |
| Carrier | `g_zb_app_evt_var` bitmask + `xTaskNotifyGive()` | `g_app_msg_q` queue of `app_queue_t` |
| Consume | `ZIGBEE_APP_GET_NOTIFY(sevent)` | `xQueueReceive(g_app_msg_q, ...)` |
| Events | `ZB_APP_EVENT_INIT`, `ZB_APP_EVENT_NOT_JOINED`, `ZB_APP_EVENT_JOINED` | `APP_REQUEST_ADV_START`, `APP_REQUEST_TRSPS_DATA_SEND`, `APP_REQUEST_FOTA_TIMER_EXPIRY` |

**3. Respect interrupt context.** `ZIGBEE_APP_NOTIFY_ISR()` and
`ble_app_request_set(..., from_isr = true)` are the ISR-safe variants; the UART RX callback and the
WDT ISR use them (or only raise a flag) and never call a blocking API. The WDT ISR in `main.c` is a
worked example: it sets `g_wdt_timeout_warn` and lets `app_main_loop()` print the message, because
`log_error()` takes a UART semaphore that cannot be taken from an ISR.

### 1.4 File Structure

```
examples/zigbee-ble/zb-ble-lighting-app/
├── CMakeLists.txt
├── Kconfig
├── default-RT582-EVB.config
├── default-RT583-EVB.config
├── default-RT584H-EVB.config
├── default-RT584HA4-EVB.config
├── default-RT584L-EVB.config
├── include
│   ├── ble_app.h                 BLE application queue / request / link-state types
│   ├── ble_profile.h             Service-info structure per link, MAX_NUM_CONN_* limits
│   ├── ble_trsp_app.h            ble_trsp_app_init() — the BLE subsystem entry point
│   ├── device_api.h              PWM light control interface
│   ├── zigbee_api.h              Zigbee events, scene/startup databases, attribute accessors
│   └── zigbee_zcl_msg_handler.h  ZCL message handler prototype
├── rtos
│   ├── CMakeLists.txt
│   └── config
│       ├── rt582/FreeRTOSConfig.h
│       └── rt584/FreeRTOSConfig.h
└── src
    ├── main.c                    Entry point, start-up sequence, Zigbee app loop, WDT
    ├── ble_trsp_app.c            BLE task, host callbacks, TRSP + FOTA handling, TRSP UART
    ├── ble_profile_app.c         Per-link profile data instance
    ├── ble_profile_def.c         Service combination and attribute-database mapping
    ├── zigbee_api.c              Network start-up, ZBOSS signal handler, persistence
    ├── zcl_construction.c        Attributes, clusters, endpoints, OTA client attributes
    ├── zigbee_zcl_msg_handler.c  ZCL command handling (On/Off, Level, Scenes, Groups, …)
    └── device_api.c              PWM light output
```

The Zigbee-side files play the same roles as in a single-protocol Zigbee example; the four
`ble_*` files and the BLE portion of `main.c` are what the dual-protocol integration adds.

### 1.5 Build Configuration

* **`CMakeLists.txt`** — lists the sources (including the three `ble_*.c` files), enables the BLE
  compile options, sets the EnhancedFlashDataset region per chip, selects the Zigbee role macros,
  and ends with `sdk_set_main_file()` + `setup_project(zb-ble-lighting-app)`:

  ```cmake
  sdk_add_compile_options(-DBLE_SUPPORT_NUM_CONN_MAX=${CONFIG_BLE_SUPPORT_NUM_CONN_MAX}
                          -DBLE_PAWR_SELECT=${CONFIG_SUPPORT_PAWR})
  sdk_add_compile_options(-DSET_SYS_CLK=1)

  if (CONFIG_HOSAL_SOC_IDLE_SLEEP)
      add_definitions(-DZB_ED_ROLE -DZB_USE_SLEEP)
  endif()
  ```

  `CONFIG_HOSAL_SOC_IDLE_SLEEP` therefore decides at compile time whether the build is a mains-powered
  router or a sleepy end device; it changes the Zigbee role, the low-power configuration and the
  watchdog strategy together. The shipped configurations leave it unset, i.e. **router**.

* **EnhancedFlashDataset region** — set per chip in `CMakeLists.txt`:

  | Chip | `EFD_START_ADDR` | `ENV_AREA_SIZE` |
  |---|---|---|
  | RT581 / RT582 | `0xF0000` | `0x4000` |
  | RT583 | `0x1E0000` | `0x10000` |
  | RT584H / RT584L | `0x101E0000` | `0x10000` |
  | RT584HA4 | `0x103E0000` | `0x10000` |

* **`Kconfig`** — the application's own menu. This example exposes the TRSP operation UART:

  | Option | Default |
  |---|---|
  | `CONFIG_APP_TRSP_OPERATION_UART_PORT` | 1 |
  | `CONFIG_APP_TRSP_OPERATION_UART_TX_PIN` | 28 |
  | `CONFIG_APP_TRSP_OPERATION_UART_RX_PIN` | 29 |
  | `CONFIG_APP_TRSP_OPERATION_UART_BAUDRATE` | 115200 |

  Supported baud rates are 115200, 38400, 57600, 500000, 1000000 and 2000000; any other value stops
  the build with an `#error`.

* **`default-<CHIP>-EVB.config`** — one pre-generated configuration per supported chip (RT582,
  RT583, RT584H, RT584HA4, RT584L). This file is the only input the CMake tree reads. Beyond the
  multi-protocol settings in §1.1 it also selects `CONFIG_ZIGBEE_BLE_EXAMPLE=y`,
  `CONFIG_ZIGBEE_BLE_LIGHTING_APP=y`, `CONFIG_APP_WDT_ENABLE=y`,
  `CONFIG_BUILD_COMPONENT_ENHANCED_FLASH_DATASET=y` and the stdio UART
  (`CONFIG_UART_STDIO_PORT=0`, TX GPIO17, RX GPIO16, 115200).

---

## 2. Application Development

### 2.1 Application Flow

```
main()
 ├─ uart_stdio_init() / vHeapRegionsInt() / hosal_flash_init()
 ├─ enhanced_flash_dataset_init() ──► check_reboot_count()
 ├─ hosal_rf_init(HOSAL_RF_MODE_MULTI_PROTOCOL)      ─► starts "hoasl-rf"
 ├─ hosal_lpm_init()
 ├─ zigbee_app_init()   ─► creates "app-zigbee", pwm_ctl_init()
 ├─ zbStart()           ─► creates "zb-thread"
 ├─ ble_trsp_app_init() ─► queues/semaphores, ble_host_stack_init(), TRSP UART,
 │                         FOTA timer, creates "app-ble"
 └─ vTaskStartScheduler()

 "app-zigbee"                              "app-ble"
   register device context                   ble_init()
   install endpoint handlers                   host callbacks, PHY, address, MTU,
   ZIGBEE_APP_NOTIFY(ZB_APP_EVENT_INIT)        profile init, ble_fota_init()
        │                                    APP_REQUEST_ADV_START
        v                                         │
   wait for notification  <── zboss_signal_handler v
        │                     zigbee_zcl_msg_handler   wait on g_app_msg_q
        v                                              │
   INIT      → zigbee_app_nwk_start()                   ├─ QUEUE_TYPE_APP_REQ  → ble_app_main()
   NOT_JOINED→ bdb_start_top_level_commissioning()      └─ QUEUE_TYPE_OTHERS   → ble_evt_handler()
   JOINED    → restore scene/startup state,                                      service handlers
               start the Zigbee OTA client
```

### 2.2 Initialization

```c
int main(void) {
    uart_stdio_init();
    vHeapRegionsInt();
    hosal_flash_init();
    log_info("%s version : %s", CONFIG_BUILD_PROJECT, CONFIG_PROJECT_VERSION);

    enhanced_flash_dataset_init();
    check_reboot_count();
    hosal_rf_init(HOSAL_RF_MODE_MULTI_PROTOCOL);
    hosal_lpm_init();
#ifdef ZB_USE_SLEEP
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_SLEEP);
    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_GPIO);
#endif

    zigbee_app_init();
    zbStart();
    ble_trsp_app_init();
    vTaskStartScheduler();
    while(1) {;}
}
```

The order matters:

1. **stdio, heap and flash first** — everything after this logs and uses the heap.
2. **`enhanced_flash_dataset_init()` then `check_reboot_count()`** — the persistent dataset must be
   available before the reboot counter is read (see §2.10).
3. **`hosal_rf_init(HOSAL_RF_MODE_MULTI_PROTOCOL)` before either stack** — both stacks attach to
   the RF subsystem, so it has to exist first.
4. **`zigbee_app_init()` → `zbStart()`** — `zigbee_app_init()` performs the OTA bank check, creates
   the `app-zigbee` task and initialises the PWM output; `zbStart()` creates the stack task and its
   lock.
5. **`ble_trsp_app_init()` last** — it creates the BLE queues and semaphores, starts the BLE host
   stack, initialises the TRSP UART and the FOTA timer, and creates the `app-ble` task.

Note that `vTaskStartScheduler()` is what actually starts all of these; nothing runs until then, so
the two stacks come up in parallel rather than in sequence.

### 2.3 Zigbee Application Loop

`app_main_loop()` in `src/main.c` is the `app-zigbee` task. Before entering its loop it registers
the endpoint definitions with the stack and installs the ZCL handler for every endpoint:

```c
ZB_THREAD_SAFE(
    ZB_AF_REGISTER_DEVICE_CTX(&simple_desc_light_ctx);
    for (int i = 0; i < simple_desc_light_ctx.ep_count; i++) {
        ZB_AF_SET_ENDPOINT_HANDLER(
            simple_desc_light_ctx.ep_desc_list[i]->ep_id,
            zigbee_zcl_msg_handler);
    }
)
ZIGBEE_APP_NOTIFY(ZB_APP_EVENT_INIT);
```

The loop then waits for a task notification, collects the pending event bits and dispatches them:

| Event | Action |
|---|---|
| `ZB_APP_EVENT_INIT` | `zigbee_app_nwk_start(ZIGBEE_CHANNEL_ALL_MASK(), 32, 0)`; if the reboot counter asked for it, `zigbee_do_factory_reset()` |
| `ZB_APP_EVENT_NOT_JOINED` | `bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING)` |
| `ZB_APP_EVENT_JOINED` | `scene_db_check()`, `startup_db_check()`, `set_startup_status()`, store the protocol marker, read and log PAN ID / short address / channel |

`zigbee_app_nwk_start()` also decides the device's role. For a router build it calls
`zb_set_network_router_role(channel_mask)` and `zb_set_max_children(32)`; when `ZB_USE_SLEEP` is
defined it instead configures a sleepy end device (`zb_set_network_ed_role()`,
`ED_AGING_TIMEOUT_64MIN`, `zb_set_rx_on_when_idle(0)`, a 1000 ms keep-alive). The IEEE address comes
from the OTP MAC address, falling back to the flash unique ID when the OTP field is blank.

### 2.4 Zigbee Stack Signal Handler

`zboss_signal_handler()` in `src/zigbee_api.c` runs in stack context and converts ZBOSS signals into
application events:

| Signal | Handling |
|---|---|
| `ZB_ZDO_SIGNAL_SKIP_STARTUP` | `zboss_start_continue()` |
| `ZB_BDB_SIGNAL_DEVICE_FIRST_START`, `ZB_BDB_SIGNAL_STEERING`, `ZB_BDB_SIGNAL_DEVICE_REBOOT` | on failure notify `ZB_APP_EVENT_NOT_JOINED`; on success notify `ZB_APP_EVENT_JOINED` and schedule the Zigbee OTA client 2 s later |
| `ZB_COMMON_SIGNAL_CAN_SLEEP` | `zb_sleep_now()` (sleep builds only) |
| `ZB_ZDO_SIGNAL_LEAVE` | when `leave_type == ZB_NWK_LEAVE_TYPE_RESET`: reset the persistent dataset, erase the FOTA bank-information sector, then `sys_software_reset()` |

The OTA client is started from the join path rather than at boot:

```c
ZIGBEE_APP_NOTIFY(ZB_APP_EVENT_JOINED);
ZB_SCHEDULE_APP_ALARM(zb_zcl_ota_upgrade_init_client, param,
                      ZB_MILLISECONDS_TO_BEACON_INTERVAL(2 * 1000));
```

### 2.5 ZCL Message Handling

`zigbee_zcl_msg_handler()` in `src/zigbee_zcl_msg_handler.c` receives every ZCL command addressed to
the application endpoint and implements the light behaviour: attribute reads and writes, Basic
reset, Identify, Groups (add / remove / remove-all, including add-if-identifying), the full Scenes
command set (add, view, remove, remove-all, store, recall, get-membership, enhanced add/view, copy),
On/Off (on, off, toggle, off-with-effect, on-with-recall-global-scene, on-with-timed-off) and Level
Control (move, step, move-to-level, with and without On/Off). Dimming ramps and timed-off behaviour
are driven by software timers, and the resulting brightness is applied through
`pwm_ctl_set_level()`.

Handler code runs in stack context, so it must **not** wrap its ZBOSS calls in `ZB_THREAD_SAFE()`.

### 2.6 Endpoint and Cluster Definition

`src/zcl_construction.c` declares the attributes, clusters and endpoints. Two endpoints are defined:

| Endpoint | Device ID | Clusters | Role |
|---|---|---|---|
| 1 (`LIGHT_EP`) | `ZB_HA_DIMMABLE_LIGHT_DEVICE_ID` (0x0101) | Basic, Identify, Groups, Scenes, On/Off, Level Control | Server |
| 10 | `ZB_HA_DIMMABLE_LIGHT_DEVICE_ID` (0x0101) | OTA Upgrade | Client |

Basic cluster identity is `RafaelMicro` / `Light0001`, with date code and software build ID
`20220121`. The OTA Upgrade client attributes (`upgrade_server`, `file_offset`, `file_version`,
`downloaded_file_ver`, `image_status`, `manufacturer`, `image_type`, …) are declared here and reached
from the application through the accessors in `zigbee_api.h` (`z_get_file_version()`,
`z_set_file_version()`, `z_set_OTA_status()`, …). The default `file_version` is `0x01010101`.

### 2.7 BLE Application Task

`ble_trsp_app_init()` builds the BLE side and creates the `app-ble` task:

```c
g_app_msg_q  = xQueueCreate(APP_QUEUE_SIZE, sizeof(app_queue_t));
semaphore_cb  = xSemaphoreCreateCounting(BLE_APP_CB_QUEUE_SIZE, BLE_APP_CB_QUEUE_SIZE);
semaphore_isr = xSemaphoreCreateCounting(APP_ISR_QUEUE_SIZE, APP_ISR_QUEUE_SIZE);
semaphore_app = xSemaphoreCreateCounting(APP_REQ_QUEUE_SIZE, APP_REQ_QUEUE_SIZE);
...
ble_host_stack_init(&ble_task_level);
app_uart_init();
g_fota_timer = xTimerCreate("fota-tmr", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, fota_timer_handler);
xTaskCreate(ble_trsp_task, "app-ble", BLE_TRSP_TASK_STACK_SIZE, NULL, BLE_TRSP_TASK_PRIORITY, NULL);
```

`ble_task_level` is a `ble_task_priority_t` (declared in `ble_api.h`) carrying the priorities for
the BLE host and HCI TX tasks, which `ble_host_stack_init()` creates.

One queue carries every kind of BLE work item, and a counting semaphore per producer class reserves
capacity so that no producer can starve the others:

| Producer | Semaphore | Slots |
|---|---|---|
| BLE host callbacks | `semaphore_cb` | `BLE_APP_CB_QUEUE_SIZE` = 32 |
| ISR-generated requests (UART RX) | `semaphore_isr` | `APP_ISR_QUEUE_SIZE` = 2 |
| Task-generated requests | `semaphore_app` | `APP_REQ_QUEUE_SIZE` = 6 |

The queue depth is the sum of the three (40). The task takes a semaphore before enqueuing and the
consumer returns it after handling the item, so a burst of host events can never fill the queue and
block an ISR from posting.

`ble_trsp_task()` runs `ble_init()` once, requests advertising, and then dispatches queue items:

```c
switch (p_app_q.param_type) {
case QUEUE_TYPE_APP_REQ:
    if (p_app_q.from_isr) { xSemaphoreGive(semaphore_isr); }
    else                  { xSemaphoreGive(semaphore_app); }
    ble_app_main(&p_app_q.param.app_req);
    break;

case QUEUE_TYPE_OTHERS:
    switch (p_app_q.param.pt_tlv->type) {
    case APP_GENERAL_EVENT:                          /* GAP / ADV / connection events  */
        ble_evt_handler((ble_evt_param_t *)p_app_q.param.pt_tlv->value);
        break;
    case APP_SERVICE_EVENT:                          /* dispatched to the service      */
        ...att_db_link[host_id].p_server_db[cb_index]->att_handler(p_svcs_param);
        break;
    case APP_L2CAP_DATA_EVENT:
    default:
        break;
    }
    vPortFree(p_app_q.param.pt_tlv);
    xSemaphoreGive(semaphore_cb);
    break;
}
```

### 2.8 BLE Host Callbacks and Event Handling

Three callbacks move host events into the application queue; each allocates a `ble_tlv_t`, copies
the event in and posts it, releasing the semaphore and freeing the buffer on failure:

| Callback | Registered for | Payload |
|---|---|---|
| `ble_app_event_cb()` | `APP_GENERAL_EVENT` | `ble_evt_param_t` |
| `ble_service_data_cb()` | `APP_SERVICE_EVENT` | `ble_evt_att_param_t` |
| `ble_l2cap_data_cb()` | `APP_L2CAP_DATA_EVENT` | `ble_l2cap_evt_param_t` |

```c
ble_host_callback_set(APP_GENERAL_EVENT,    ble_app_event_cb);
ble_host_callback_set(APP_SERVICE_EVENT,    ble_service_data_cb);
ble_host_callback_set(APP_L2CAP_DATA_EVENT, ble_l2cap_data_cb);
```

`ble_evt_handler()` then processes the general events in task context:

| Event | Handling |
|---|---|
| `BLE_ADV_EVT_SET_ENABLE` | update the link state to `STATE_ADVERTISING` / `STATE_STANDBY` |
| `BLE_GAP_EVT_CONN_COMPLETE` | set `STATE_CONNECTED`, log the host ID |
| `BLE_GAP_EVT_CONN_PARAM_UPDATE` | log the update |
| `BLE_GAP_EVT_PHY_READ`, `BLE_GAP_EVT_PHY_UPDATE` | log the TX/RX PHY |
| `BLE_ATT_GATT_EVT_MTU_EXCHANGE` | store the negotiated MTU in `g_trsp_mtu` (used for TRSP segmentation) |
| `BLE_ATT_GATT_EVT_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH`, `BLE_ATT_GATT_EVT_DATA_LENGTH_CHANGE` | log the result |
| `BLE_GAP_EVT_DISCONN_COMPLETE` | `ble_fota_disconnect()`, set `STATE_STANDBY`, restart advertising |

Restarting advertising on disconnection means the device is always reachable over BLE without any
user action.

### 2.9 BLE Application Requests

`ble_app_request_set()` posts a request to the BLE task from either task or interrupt context, and
`app_peripheral_handler()` executes it:

| Request | Handling |
|---|---|
| `APP_REQUEST_ADV_START` | reset the GATT/TRSP/FOTA service data, `ble_cmd_default_mtu_size_set()`, `adv_init()`, `adv_enable()`, reset `g_trsp_mtu` to the minimum |
| `APP_REQUEST_TRSPS_DATA_SEND` | send the pending UART buffer as a notification (or indication, depending on the CCCD); on failure set `g_data_send_pending` and re-queue |
| `APP_REQUEST_FOTA_TIMER_EXPIRY` | `ble_fota_timerexpiry_handler(host_id)` |

Advertising uses `ADV_TYPE_ADV_IND` with a 160-unit (100 ms) interval on all channels, an
advertising payload carrying only the flags, and the device name `LIGHTING_TRSP` in the scan
response.

### 2.10 BLE Initialization Sequence and HCI Pacing

`ble_init()` runs once at the start of `ble_trsp_task()`:

1. register the three host callbacks,
2. `ble_cmd_phy_controller_init()`,
3. `ble_cmd_read_unique_code()` — on success set the device address and identity resolving key from
   the MP sector; otherwise fall back to the static random address in `DEVICE_ADDR`,
4. `ble_cmd_resolvable_address_init()`,
5. `ble_cmd_suggest_data_len_set(BLE_GATT_DATA_LENGTH_MAX)`,
6. `server_profile_init(APP_TRSP_P_HOST_ID)` — GAP, GATT, DIS, TRSPS and FOTAS,
7. `ble_fota_fw_buffer_flash_check()` and `ble_fota_init()`.

Each HCI-heavy step is separated by `ble_hci_cmd_step_delay()` (20 ms) and
`ble_wait_hci_cmd_queue_ready()`, which waits until the HCI command and common queues have spare
capacity:

```c
static bool ble_wait_hci_cmd_queue_ready(TickType_t timeout_ticks) {
    TickType_t start_ticks = xTaskGetTickCount();

    for (;;) {
        if ((g_hci_tx_cmd_handle != NULL) && (g_hci_common_handle != NULL) &&
            (uxQueueSpacesAvailable(g_hci_tx_cmd_handle) > 4U) &&
            (uxQueueSpacesAvailable(g_hci_common_handle) > 5U)) {
            return true;
        }
        if ((xTaskGetTickCount() - start_ticks) >= timeout_ticks) {
            return false;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

This pacing matters specifically in a multi-protocol build: the radio is shared, so HCI commands
drain more slowly than in a BLE-only application, and issuing initialization commands back to back
can overflow the HCI queues. Keep this pacing when adding further initialization steps.

### 2.11 Persistence, Recovery and Watchdog

**Persistent data.** EnhancedFlashDataset stores four keys:

| Key | Content |
|---|---|
| `reboot` | reboot counter used by the recovery mechanism below |
| `scenedb` | `scene_db_t` — global scene, 16-entry scene table, 16-entry group table |
| `startupdb` | `startup_entry_t` — last On/Off state, last level, StartUpOnOff, StartUpCurrentLevel |
| `prot` | active protocol marker, written after joining |

`scene_db_check()` and `startup_db_check()` create a blank record when a key is missing;
`set_startup_status()` restores the configured power-on behaviour and drives the PWM accordingly.

**Reboot-count recovery.** `check_reboot_count()` increments the `reboot` key at boot, waits 500 ms
and then clears it. A reboot that completes normally therefore leaves the counter at zero, while
five power cycles in quick succession leave it at five and set `reset_to_default`, which triggers
`zigbee_do_factory_reset()` when `ZB_APP_EVENT_INIT` is handled. This gives a user a way to factory
reset a device that has no button.

**Watchdog.** With `CONFIG_APP_WDT_ENABLE=y` the application starts the WDT with an 8 s timeout and
a 500 ms warning interrupt (prescale 32, i.e. 1 MHz). The kick strategy follows the build type:

* **Router (no `ZB_USE_SLEEP`)** — the counter runs continuously, so `app_main_loop()` waits on its
  notification with a 1 s timeout and kicks the WDT on every iteration.
* **Sleepy end device (`ZB_USE_SLEEP`)** — the WDT is frozen while asleep and the task blocks
  indefinitely, so the kick comes from `vApplicationIdleHook()`: reaching idle proves the system is
  healthy, and this is independent of whether the application task was notified. A task that spins
  without yielding never lets the idle task run, so a genuine hang still triggers a reset.

`WDT_TIMEOUT_MS` must exceed the longest contiguous *awake* processing burst — network steering,
flash erase/write, OTA block handling — not the sleep period. `hosal_wdt_reset_event_get()` is
checked at start-up so a previous watchdog reset is reported even if the hang prevented the warning
from being printed.

---

## 3. BLE Transparent Service (TRSP)

### 3.1 Service and Characteristic UUIDs

The Transparent Service is a Rafael proprietary service that emulates a serial port over BLE. See
[RT58x BLE SDK Service & Profile Guide](../BLE/RT58x_BLE_SDK_Service_Profile_Guide.md) for how such
a service is defined; the implementation is in
[`components/network/bluetooth/ble-host/ble-service/trsps/`](../../components/network/bluetooth/ble-host/ble-service/trsps/).

| Service | UUID | Description |
|---|---|---|
| TRSP Service | `00112233445566778899AABBCCDDEEFF` | Serial data transferred via BLE |

| Characteristic | UUID | Properties | Description |
|---|---|---|---|
| `UDATR01` | `101112131415161718191A1B1C1D1E1F` | Read | Static data the client can read |
| `UDATNI01` | `303132333435363738393A3B3C3D3E3F` | Notify / Indicate | Device-to-client data |
| `UDATRW01` | `505152535455565758595A5B5C5D5E5F` | Read / Write / Write-without-response | Client-to-device data |

### 3.2 Profile Composition

`include/ble_profile.h` declares the per-link service information and the connection limits
(`MAX_NUM_CONN_GAPS`, `_GATTS`, `_DIS`, `_TRSPS`, `_FOTAS`, all 1), and `src/ble_profile_def.c`
combines the services into the attribute database for link 0:

```c
const ble_att_param_t *const att_service_comb00[] = {
    &ATT_NULL_INVALID,
    ATT_GAPS_SERVICE,
    ATT_GATTS_SERVICE,
    ATT_DIS_SERVICE,
    ATT_TRSPS_SERVICE,
    ATT_FOTAS_SERVICE,
};
```

The example is a single-link peripheral (`CONFIG_BLE_SUPPORT_NUM_CONN_MAX=1`), and
`src/ble_profile_app.c` provides the one profile-data instance with
`gap_role = BLE_GAP_ROLE_PERIPHERAL`. `server_profile_init()` initialises each service in turn and
registers the two application handlers — `ble_svcs_trsps_evt_handler` and
`ble_svcs_fotas_evt_handler`.

### 3.3 BLE to UART Data Path

Data written to `UDATRW01` is first offered to the in-band command parser (§3.5) and otherwise
forwarded to the TRSP UART:

```c
static void ble_svcs_trsps_evt_handler(ble_evt_att_param_t *p_param) {
    if (p_param->gatt_role != BLE_GATT_ROLE_SERVER) {
        return;
    }

    switch (p_param->event) {
    case BLESERVICE_TRSPS_UDATRW01_WRITE_EVENT:
    case BLESERVICE_TRSPS_UDATRW01_WRITE_WITHOUT_RSP_EVENT:
        if (p_param->length != 0U) {
            if (handle_trsp_command(p_param->data, p_param->length)) {
                break;
            }
            (void)hosal_uart_send(&trsp_uart_dev, p_param->data, p_param->length);
        }
        break;

    case BLESERVICE_TRSPS_UDATRW01_READ_EVENT:
        /* answer a read request with a fixed string */
        ...
        break;
    }
}
```

### 3.4 UART to BLE Data Path

The TRSP UART runs in interrupt RX mode. `uart_data_handler()` accumulates characters and flushes a
buffer when a line ends (`\n` or `\r`) or when the buffer reaches `g_trsp_mtu - 3` bytes — the
largest payload that fits in one notification for the negotiated MTU:

```
UART1 RX ISR
   uart0_rx_callback() ─► uart_data_handler(ch)
        │  on '\n'/'\r' or (index >= g_trsp_mtu - 3)
        v
   trsp_data_send_from_isr(buffer, length)
        │  copy into g_rx_buffer[g_uart_index] (UART_BUFF_DEPTH = 2)
        v
   ble_app_request_set(APP_TRSP_P_HOST_ID, APP_REQUEST_TRSPS_DATA_SEND, true)
        │
        v
   "app-ble": app_peripheral_handler()
        │  ble_svcs_trsps_server_send(... hdl_udatni01 ...)
        │  notification if the CCCD selects notify, indication if it selects indicate
        v
   BLE client
```

Two buffers are used in rotation, and `g_data_send_pending` provides back-pressure: while a send is
outstanding, new ISR data is dropped rather than overwriting a buffer that has not been transmitted.
If the send fails the request is re-queued.

### 3.5 In-Band Commands

`handle_trsp_command()` inspects the received TRSP payload before it is forwarded to the UART, which
lets a BLE client control the Zigbee side without a separate GATT service. The example implements
one command:

```c
static bool handle_trsp_command(const uint8_t *data, uint16_t length) {
    static const uint8_t leave_cmd[] = "Leave";

    if ((length == (sizeof(leave_cmd) - 1U)) &&
        (memcmp(data, leave_cmd, sizeof(leave_cmd) - 1U) == 0)) {
        printf("TRSP command: Leave -> factory reset\n");
        ZB_THREAD_SAFE(zigbee_do_factory_reset();)
        return true;
    }

    return false;
}
```

Returning `true` consumes the payload so it is not echoed to the UART. To add a command, extend this
function — and remember that it runs in the `app-ble` task, so every ZBOSS call needs
`ZB_THREAD_SAFE()` (§1.3).

---

## 4. Running the Demo

### 4.1 Prerequisites

* **One RT58x EVB** flashed with this example. Supported targets: RT582, RT583, RT584H, RT584HA4 and
  RT584L EVB.
* **A Zigbee network to join** — any Zigbee 3.0 coordinator with permit-join enabled.
* **A BLE client** — a phone app or PC tool that can browse GATT services and write to a
  characteristic; the FOTA update additionally needs the Rafael mobile application.
* **Serial connections:**

  | Purpose | UART | Pins | Settings |
  |---|---|---|---|
  | Debug console (log output) | UART0 | TX GPIO17, RX GPIO16 | 115200 8N1 |
  | TRSP operation UART | UART1 | TX GPIO28, RX GPIO29 | 115200 8N1 (configurable, §1.5) |

  The debug console is enough to follow the demo. Connect the TRSP UART to a second terminal only
  when you want to exercise the pass-through in both directions.

### 4.2 Build and Flash

```bash
cmake -DCUSTOM_CONFIG_DIR="examples/zigbee-ble/zb-ble-lighting-app/default-RT582-EVB.config" \
      -S . -B build_zb_ble -G Ninja
cmake --build build_zb_ble -j16
```

Make sure `arm-none-eabi-gcc`, `cmake`, and `ninja` are on the PATH. See the
[Setup guide](../SDK_Setup/sdk_setup.md). CMake sets the generator when it first creates the
build directory. To change the generator later, delete the build directory. Run the cmake command
again.

The build writes the image to the build directory, as `zb-ble-lighting-app_<CHIP>.elf/.bin/.hex`.
See the [Setup guide](../SDK_Setup/sdk_setup.md) for the toolchain setup and the Rafael extension.
See [Build Examples](../SDK_Setup/sdk_build_examples.md) for the flashing procedure.

To change the configuration, run `python3 -m menuconfig` from the repository root and save the
result over the example's `.config` file.

### 4.3 Joining a Zigbee Network

After reset the device logs its version and MAC address, starts both stacks and begins network
steering. On success the console reports the network parameters:

```
zb-ble-lighting-app version : <version>
15p4 MAC Address : XX:XX:XX:XX:XX:XX:XX:XX
ZigBee APP init
BLE advertising
ZigBee APP joined
startup status: onoff = 0, level = 1
Protocol: Zigbee
PAN ID: 1213, Short Addr: 4E8D, Channel: 12
```

The two application tasks log independently, so the exact interleaving of the Zigbee and BLE lines
varies from boot to boot.

If no network accepts the device, `ZB_APP_EVENT_NOT_JOINED` is raised and
`bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING)` retries. Enable permit-join on the
coordinator and reset the device to retry immediately. Network parameters persist, so after a power
cycle the device rejoins its previous network without commissioning.

Note that the BLE peripheral is advertising throughout — it does not wait for the Zigbee side to
join, and joining does not interrupt advertising.

### 4.4 Controlling the Light over Zigbee

With the device joined, the light responds to standard ZCL commands on endpoint 1:

| Cluster | Behaviour |
|---|---|
| On/Off | On, Off and Toggle switch the PWM output between the current level and 0; off-with-effect and on-with-timed-off are supported |
| Level Control | Move, Step and Move-to-Level dim the PWM output, with and without the On/Off coupling; the ramp is timer-driven |
| Identify | Blinks the output at 50 % every 200 ms for the duration of `IdentifyTime`, then restores the previous state |
| Groups | Up to 16 group memberships, stored in `scenedb` |
| Scenes | Up to 16 scenes storing On/Off state, level and transition time, stored in `scenedb` |

Power-on behaviour follows the `StartUpOnOff` and `StartUpCurrentLevel` attributes, which are
restored from `startupdb` when the device joins.

### 4.5 Connecting over BLE

The device advertises as **`LIGHTING_TRSP`** with a 100 ms advertising interval; the complete local
name is in the scan response. Connect with any BLE client and you should see the console report the
connection and the negotiated MTU:

```
BLE advertising
BLE connected, ID=0
BLE MTU=247
BLE conn updated, ID=0
```

The GATT database exposes GAP, GATT, Device Information, TRSP (§3.1) and FOTA. On disconnection the
device logs the reason and restarts advertising automatically.

### 4.6 Transparent Data Transfer

**BLE to UART:** write a payload to the `UDATRW01` characteristic
(`505152535455565758595A5B5C5D5E5F`), using either a write or a write-without-response. Unless it
matches an in-band command, the bytes appear on the TRSP UART (UART1, 115200 8N1).

**UART to BLE:** enable notifications on `UDATNI01`
(`303132333435363738393A3B3C3D3E3F`), then type a line into the TRSP UART terminal. The line is sent
as a notification when you press Enter, or earlier if it reaches `MTU - 3` bytes. Without the CCCD
enabled for notification or indication, nothing is sent.

### 4.7 Factory Reset over BLE

Writing the string `Leave` to `UDATRW01` resets the Zigbee side to factory-new:

```
TRSP command: Leave -> factory reset
Device do factory reset
```

The sequence is `zigbee_do_factory_reset()` → `zb_bdb_reset_via_local_action()` →
`ZB_ZDO_SIGNAL_LEAVE` with `leave_type == ZB_NWK_LEAVE_TYPE_RESET`, which resets the persistent
dataset to defaults, erases the FOTA bank-information sector, waits 300 ms and calls
`sys_software_reset()`. Because the device reboots, the BLE connection is terminated as well; after
the reboot it advertises again and starts network steering from scratch.

The same factory reset can be reached without a BLE client by power-cycling the device five times in
quick succession (§2.11).

---

## 5. Firmware Update (OTA)

### 5.1 Overview

This example carries two independent firmware-update paths:

| | BLE FOTA | Zigbee OTA Upgrade |
|---|---|---|
| Transport | BLE connection, FOTA GATT service | Zigbee network, OTA Upgrade cluster |
| Peer | Phone app / PC tool | Zigbee OTA server (coordinator or gateway) |
| Application side | `ble_trsp_app.c` (`ble_fota_*`) | Endpoint 10 OTA client, `zcl_construction.c` |
| Prerequisite | BLE connection established | Device joined to a Zigbee network |
| Documentation | [RT58x BLE SDK OTA Guide](../BLE/RT58x_BLE_SDK_OTA_Guide.md) | §5.3 below |

Both write the new image into the same dual-bank flash layout and are applied by the bootloader on
the next reset, so **only one update may be in progress at a time**.

### 5.2 Firmware Update over BLE (FOTA)

The complete BLE FOTA procedure — the dual-bank flash layout, building the BLE `.bin` with system
information, producing the FOTA bin file with the PC tool, running the update from the mobile app,
and the FOTA command/data protocol — is documented in
**[RT58x BLE SDK OTA Guide](../BLE/RT58x_BLE_SDK_OTA_Guide.md)**. That procedure applies to this
example unchanged; this section only describes how the example wires the FOTA service up.

**Service.** `ATT_FOTAS_SERVICE` is part of the attribute database (§3.2) and is initialised at the
end of `server_profile_init()`:

```c
return ble_svcs_fotas_init(host_id, BLE_GATT_ROLE_SERVER,
                           &p_profile_info->svcs_info_fotas,
                           ble_svcs_fotas_evt_handler);
```

**Initialization.** `ble_init()` finishes with `ble_fota_fw_buffer_flash_check()` — which validates
the update bank before any transfer starts — followed by `ble_fota_init()`.

**Command and data handling.** The service handler forwards both characteristics to the FOTA state
machine and (re)starts the timer that drives it:

```c
static void ble_svcs_fotas_evt_handler(ble_evt_att_param_t *p_param) {
    if (p_param->gatt_role != BLE_GATT_ROLE_SERVER) {
        return;
    }

    switch (p_param->event) {
    case BLESERVICE_FOTAS_DATA_WRITE_WITHOUT_RSP_EVENT:
        ble_fota_data(p_param->host_id, p_param->length, p_param->data);
        (void)fota_sw_timer_start();
        break;

    case BLESERVICE_FOTAS_COMMAND_WRITE_EVENT:
        ble_fota_cmd(p_param->host_id, p_param->length, p_param->data);
        (void)fota_sw_timer_start();
        break;
    }
}
```

**Timer.** `g_fota_timer` is a 1 s auto-reloading FreeRTOS timer created in `ble_trsp_app_init()`.
Its callback runs only while the link is connected; when `ble_fota_timertick()` returns `EXPIRED` it
posts `APP_REQUEST_FOTA_TIMER_EXPIRY` so that `ble_fota_timerexpiry_handler()` runs in task context
rather than in the timer service task.

**Disconnection.** `ble_fota_disconnect()` is called from the disconnect handler so an interrupted
transfer is abandoned cleanly and the next connection can start over.

**Result reporting.** After the bootloader has applied an image, `zb_app_ota_status_chk()` (called
from `zigbee_app_init()` at start-up) reads the bank information, logs `OTA success` when
`fotabank_ready == FOTA_IMAGE_READY` and `fota_result == FOTA_RESULT_SUCCESS`, and erases the sector
so the result is reported once.

### 5.3 Firmware Update over Zigbee (OTA Upgrade Cluster)

Endpoint 10 carries an OTA Upgrade **client**, so the device can also be updated by a Zigbee OTA
server. The client is not started at boot but two seconds after the device joins a network, from
`zboss_signal_handler()`:

```c
ZB_SCHEDULE_APP_ALARM(zb_zcl_ota_upgrade_init_client, param,
                      ZB_MILLISECONDS_TO_BEACON_INTERVAL(2 * 1000));
```

The client attributes are declared in `zcl_construction.c` and can be read or updated from the
application through `zigbee_api.h`:

| Accessor | Attribute |
|---|---|
| `z_get_upgrade_server_addr()` | `UpgradeServerID` |
| `z_get_file_version()` / `z_set_file_version()` | `CurrentFileVersion` (default `0x01010101`) |
| `z_set_OTA_status()` | `ImageUpgradeStatus` |
| `z_set_OTA_downloaded_file_version()` | `DownloadedFileVersion` |
| `z_set_OTA_downloaded_file_offset()` | `FileOffset` |

The current file version is logged at start-up by `zb_app_ota_status_chk()`, which also performs the
bank-information check described in §5.2 — the same mechanism serves both update paths.

### 5.4 Notes and Limitations

* **One update at a time.** Both paths target the same update bank. Do not start a Zigbee OTA
  download while a BLE FOTA transfer is in progress, or vice versa.
* **Avoid updating during commissioning.** While the Zigbee stack is performing network steering or
  rejoining, the radio is busy scanning and the shared-radio duty cycle available to BLE drops
  sharply. Start a BLE FOTA transfer only once the device has joined (or has stopped retrying), and
  expect a slower transfer if commissioning starts mid-update.
* **Watchdog headroom.** The WDT timeout must exceed the longest single burst of image handling —
  writing a block to flash while the stacks are active. If you shorten `WDT_TIMEOUT_MS` or add work
  to the FOTA path, re-check this (§2.11).
* **The reboot counter is not update-aware.** Five consecutive resets trigger a Zigbee factory
  reset, which erases the network credentials but not the firmware. If a new image resets
  repeatedly, expect the device to come back factory-new.
* **Bank addresses are chip-specific.** `FOTA_UPDATE_BANK_INFO_ADDRESS`,
  `FOTA_UPDATE_BUFFER_FW_ADDRESS`, `SIZE_OF_FOTA_BANK` and `APP_START_ADDRESS` are defined per chip
  in
  [`components/common/fota/Include/fota_define.h`](../../components/common/fota/Include/fota_define.h);
  see also [Flash Layout Overview](../SDK_Overview/Flash_Layout_Overview.md).

---

## References

* [RT58x BLE SDK OTA Guide](../BLE/RT58x_BLE_SDK_OTA_Guide.md) — the complete BLE FOTA procedure
* [RT58x BLE SDK Service & Profile Guide](../BLE/RT58x_BLE_SDK_Service_Profile_Guide.md) — creating and combining GATT services
* [RT58x BLE SDK Application Guide](../BLE/RT58x_BLE_SDK_Application_Guide.md) — the single-protocol BLE examples, including `periph-trsp-fota`
* [Zigbee Application Note](../Zigbee/zigbee_application_note.md) — the single-protocol Zigbee examples and software flow
* [Setup guide](../SDK_Setup/sdk_setup.md): toolchain, build, flash, UART and JLink debug
* [Build Examples](../SDK_Setup/sdk_build_examples.md) — building from the VS Code Rafael extension
* [Flash Layout Overview](../SDK_Overview/Flash_Layout_Overview.md) — flash regions and FOTA banks
* [`examples/zigbee-ble/zb-ble-lighting-app`](../../examples/zigbee-ble/zb-ble-lighting-app/) — the example this guide describes
