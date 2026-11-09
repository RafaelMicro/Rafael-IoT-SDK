# RT58x BLE SDK User Guide

## Introduction

This document is a reference for developing BLE applications using the Rafael RT58x BLE SDK.

---

## Table of Contents

1. [BLE SDK Architecture](#1-ble-sdk-architecture)
   1. [BLE SDK Folder Hierarchy](#11-ble-sdk-folder-hierarchy)
      1. [BLE Sources](#111-ble-sources)
      2. [BLE Demo Projects](#112-ble-demo-projects)
   2. [BLE System Architecture](#12-ble-system-architecture)
      1. [BLE Task Definition and Priority](#121-ble-task-definition-and-priority)
      2. [BLE Task Architecture](#122-ble-task-architecture)
      3. [BLE Event Handling Flow](#123-ble-event-handling-flow)
2. [BLE Application Development](#2-ble-application-development)
   1. [BLE Application Flow](#21-ble-application-flow)
   2. [BLE Initialization](#22-ble-initialization)
   3. [BLE Main Loop](#23-ble-main-loop)
   4. [BLE Event Handler](#24-ble-event-handler)
   5. [BLE Service Event Handle (Optional)](#25-ble-service-event-handle-optional)
3. [BLE Profile Implementation](#3-ble-profile-implementation)
   1. [Step 1 & 2: Implement Services and Profiles](#31-step-1--2-implement-services-and-profiles)
   2. [Step 3: Implement User Data for BLE Profiles](#32-step-3-implement-user-data-for-ble-profiles)
4. [BLE SDK APIs](#4-ble-sdk-apis)
   1. [BLE Common APIs](#41-ble-common-apis)
   2. [BLE Advertising APIs](#42-ble-advertising-apis)
   3. [BLE Scan APIs](#43-ble-scan-apis)
   4. [BLE GAP APIs](#44-ble-gap-apis)
   5. [BLE ATT and GATT APIs](#45-ble-att-and-gatt-apis)
   6. [BLE Security APIs](#46-ble-security-apis)
   7. [BLE Privacy APIs](#47-ble-privacy-apis)
   8. [BLE Connect CTE APIs](#48-ble-connect-cte-apis)
   9. [BLE Extended & Periodic Advertising APIs](#49-ble-extended--periodic-advertising-apis)
   10. [BLE L2CAP APIs](#410-ble-l2cap-apis)

---

## 1. BLE SDK Architecture

BLE upper layers — the BLE application and BLE Host — communicate with the BLE Controller via
HCI commands in the HCI layer:

```
 ┌─────────────────────────────┐
 │      BLE Application        │   examples/bluetooth-le/**
 ├─────────────────────────────┤
 │  GAP / L2CAP / ATT / GATT   │   BLE Host core (prebuilt library)
 │  / SMP (BLE Host)           │   components/network/bluetooth/ble-host/ble-cmd-api
 ├─────────────────────────────┤
 │       HCI (cmd / event)     │   components/network/bluetooth/ble-host/hci
 ├─────────────────────────────┤
 │       BLE Controller        │   RF/Link-Layer, platform HAL (hosal_rf.c)
 └─────────────────────────────┘
```

GAP, L2CAP, ATT, GATT and SMP are built as a per-chip-family static library
(e.g. `libCM33_ble_host.a` for RT584H/RT584HA4/RT584L/RF1301, `libCM3_ble_host.a` for
RT581/RT582/RT583). The BLE Host core is delivered as this prebuilt library — its source is not
part of the released SDK tree, so treat any host-core internals mentioned later in this document
(task names, priorities, etc.) as background/prebuilt-library behavior rather than as a folder you
can browse.

The BLE command API layer (`ble-cmd-api`) and the per-profile services (`ble-service`) are built as
separate static libraries (`libble_cmd_api.a`, `libble_service.a`) and linked into the application.
These folders are the actual API surface applications should develop against (`ble_cmd_*` /
`ble_svcs_*`, described in this document).

This section introduces the Rafael RT58x BLE SDK folder hierarchy and the FreeRTOS task hierarchy
used by BLE applications.

### 1.1 BLE SDK Folder Hierarchy

#### 1.1.1 BLE Sources

BLE stack source lives under
[`components/network/bluetooth/ble-host/`](../../components/network/bluetooth/ble-host/):

```
components/network/bluetooth/
├── ble-host/
│   ├── ble-cmd-api/            BLE Command API & Definitions
│   │   ├── include/            one header per API group:
│   │   │   ble_common.h, ble_advertising.h, ble_scan.h, ble_gap.h, ble_att_gatt.h,
│   │   │   ble_security_manager.h, ble_privacy.h, ble_connect_cte.h,
│   │   │   ble_extended_advertising.h, ble_l2cap.h, ble_event.h, ble_api.h,
│   │   │   ble_host_ref.h, ble_host_cmd.h, ble_hci_error.h, ble_uuid.h
│   │   └── src/
│   │       ble_cmd_common.c, ble_cmd_advertising.c, ble_cmd_scan.c, ble_cmd_gap.c,
│   │       ble_cmd_att_gatt.c, ble_cmd_security_manager.c, ble_cmd_privacy.c,
│   │       ble_cmd_connect_cte.c, ble_cmd_extended_advertising.c, ble_cmd_l2cap.c,
│   │       ble_host_ref.c
│   ├── hci/                    HCI command/event layer
│   │   ├── include/  ble_hci.h, ble_hci_event.h, hci_cmd*.h
│   │   └── src/       hci_cmd_*.c, hci_event.c
│   ├── ble-service/             one subfolder per profile
│   │   ├── common/, gaps/, gatts/, dis/, trsps/, bas/, bls/, hrs/, hids/, ots/,
│   │   └── esls/, fota/, matter/, mesh_prov/, mesh_proxy/
│   │       each profile: include/ble_service_<name>.h + src/ble_service_<name>.c
│   ├── fota/                    BLE FOTA (firmware-over-the-air) support
│   └── uecc/                    micro-ecc crypto library (used by LE Secure Connections)
├── ble-mesh/                    BLE Mesh stack — separate component, not covered by this guide
└── hci_bridge/                  HCI-over-transport bridge (AT-command / dongle style demos)
```

Prebuilt libraries are generated per chip family via `sdk_generate_library(...)` in
[`components/network/bluetooth/ble-host/CMakeLists.txt`](../../components/network/bluetooth/ble-host/CMakeLists.txt).

#### 1.1.2 BLE Demo Projects

BLE demo/example projects live under
[`examples/bluetooth-le/`](../../examples/bluetooth-le/), organized into 5 categories:

```
examples/bluetooth-le/
├── central/
│   ├── central-trsp/            TRSP central demo
│   ├── central-data-rate/       Throughput/data-rate test central
│   └── central-esl-ap/          Electronic Shelf Label Access Point
├── peripheral/
│   ├── periph-trsp/              TRSP peripheral demo
│   ├── periph-trsp-fota/         TRSP + FOTA combined demo
│   ├── periph-data-rate/         Throughput/data-rate test peripheral
│   ├── periph-hrs/               Heart Rate Service
│   ├── periph-hogp/              HID over GATT
│   ├── periph-esl/               Electronic Shelf Label peripheral
│   └── periph-iot-test/
├── multi-link/
│   ├── multi-1c1p/               1 Central + 1 Peripheral
│   └── power-management/         Low-power multi-link demo
├── mesh/
│   ├── gateway/                  BLE Mesh ↔ gateway bridge
│   └── lightness-trsp/           BLE Mesh lightness model + TRSP
└── others/
    ├── at-command/               AT-command / HCI-bridge style control
    └── ibeacon/
```

Each demo project follows this internal layout (using `periph-trsp` as the canonical example):

```
periph-trsp/
├── CMakeLists.txt, Kconfig
├── default-RT581-EVB.config, default-RT582-EVB.config, ... (one per supported chip/EVB)
├── ble-app-profile/
│   ├── include/ble_profile.h
│   └── src/ble_profile_app.c, ble_profile_def.c
├── periph-trsp/                  (app-specific folder, named after the project itself)
│   ├── include/ble_app.h
│   └── src/main.c
├── rtos/                         FreeRTOS config/CMakeLists for this project
└── build/                        ninja/cmake output (.elf/.bin/.hex/.map)
```

Build-time knobs (e.g. `BLE_SUPPORT_NUM_CONN_MAX`) are Kconfig options
(`CONFIG_BLE_SUPPORT_NUM_CONN_MAX=1` in `default-<CHIP>-EVB.config`) passed to the compiler via
`sdk_add_compile_options(...)` in the project's `CMakeLists.txt`.

More details on building and running each demo are in
`RT58x_BLE_SDK_Application_Guide.md` (see `docs/BLE/RT58x_BLE_SDK_Application_Guide.md`).

### 1.2 BLE System Architecture

The BLE SDK creates several FreeRTOS tasks for handling RF/HCI processing, BLE host processing,
command transport, and the application itself.

#### 1.2.1 BLE Task Definition and Priority

| Task | Created in | Notes |
|---|---|---|
| `"hoasl-rf"` (RF/controller ISR-driven proc task) | `components/platform/hosal/<chip>_hosal/Src/hosal_rf.c` (`hosal_rf_init()` → `xTaskCreate(__rf_proc, "hoasl-rf", ..., E_TASK_PRIORITY_HOSAL, ...)`) | Lives in the platform HAL layer. Handles RF/Link-Layer interrupts and feeds HCI events up to the host. |
| `"HOST_SUBSYSTEM"` | Internal BLE Host core, inside `ble_host_subsystem_init()` (prebuilt library) | Priority passed in as `p_priority->ble_host_level`. |
| `"Host_TX"` | Internal BLE Host core, inside `task_host_tx_init()` (called from `ble_host_subsystem_init()`; prebuilt library) | Priority passed in as `p_priority->hci_tx_level`. Transports command/ACL data from host to HCI. |
| task named `"main"`, running `app_main_task()` | The demo's own `main.c`, e.g. `examples/bluetooth-le/peripheral/periph-trsp/periph-trsp/src/main.c:1401` (`xTaskCreate(app_main_entry, "main", ..., E_TASK_PRIORITY_APP, NULL)`) | Every demo's `main()` creates one task named `"main"`; the BLE-specific loop is `app_main_task()`, invoked from `app_main_entry()`. Handles both application requests and BLE general/service/L2CAP events, dispatched through the same message queue — `ble_app_event_cb()` / `ble_service_data_cb()` / `ble_l2cap_data_cb()` are host callbacks that push a TLV message onto the same queue (`g_app_msg_q`) the app task reads. |

Priorities for the two BLE-internal tasks are supplied by the application as a
`ble_task_priority_t` (`ble_host_level`, `hci_tx_level`) passed into `ble_host_stack_init()`:

```c
// examples/bluetooth-le/peripheral/periph-trsp/periph-trsp/src/main.c
ble_task_priority_t ble_task_level;
ble_task_level.ble_host_level = configMAX_PRIORITIES - 7;
ble_task_level.hci_tx_level   = configMAX_PRIORITIES - 6;
ble_host_stack_init(&ble_task_level);
```

(`ble_task_priority_t` is defined in
[`ble-host/ble-cmd-api/include/ble_api.h`](../../components/network/bluetooth/ble-host/ble-cmd-api/include/ble_api.h).)

##### 1.2.1.1 RF / HCI processing (`hoasl-rf`)

Handles RF/Link-Layer interrupts, HCI commands, ACL data and events at the platform HAL level.

##### 1.2.1.2 BLE Host Task (`HOST_SUBSYSTEM`)

BLE Host sub-system: GAP/L2CAP/ATT/GATT/SMP processing (`ble_host_subsystem_handle`).

##### 1.2.1.3 BLE cmd transport Task (`Host_TX`)

Transports application command/ACL data from the host to the HCI layer (`task_tx_handle`).

##### 1.2.1.4 BLE Application Task (`"main"` running `app_main_task()`)

Handles both application requests (`APP_REQUEST_*`) and BLE general/service/L2CAP events, dispatched
through the same message queue — see [2.3](#23-ble-main-loop).

#### 1.2.2 BLE Task Architecture

Receive path — RF/HCI events flow up to the application:

```
 hosal_rf.c ("hoasl-rf")
     │  RF/Link-Layer ISR → HCI events
     ▼
 "HOST_SUBSYSTEM" task
 (BLE Host core, prebuilt)
     │  GAP/L2CAP/ATT/GATT/SMP processing
     ▼
 ble_host_callback_set() callbacks
 (ble_app_event_cb / ble_service_data_cb / ble_l2cap_data_cb)
     │  push ble_tlv_t onto g_app_msg_q
     ▼
 app_main_task()  (task "main")
     • QUEUE_TYPE_APP_REQ    → ble_app_main() → app_<role>_handler()
     • QUEUE_TYPE_OTHERS/APP_GENERAL_EVENT → ble_evt_handler()
     • QUEUE_TYPE_OTHERS/APP_SERVICE_EVENT → per-service att_handler() via att_db_link[]
     • QUEUE_TYPE_OTHERS/APP_L2CAP_DATA_EVENT
```

Send path — the application issues a command, which flows back down to the RF controller:

```
 app_main_task()  (task "main")
     │  app_<role>_handler() calls a command API, e.g. ble_cmd_adv_enable()
     ▼
 ble-cmd-api  (ble_cmd_*.c, e.g. ble_cmd_advertising.c)
     │  builds the HCI command and calls an hci_cmd_*() function, e.g. hci_le_set_adv_enable_cmd()
     ▼
 HCI layer (ble-host/hci, e.g. hci_cmd_advertising.c)
     │  host_send_cmd_to_hci() posts onto the HCI common queue (g_hci_common_handle)
     ▼
 "Host_TX" task
 (BLE Host core, prebuilt)
     │  drains the queue, hands the command/ACL data to the controller
     ▼
 hosal_rf.c ("hoasl-rf")  →  RF/Link-Layer  →  over the air
```

#### 1.2.3 BLE Event Handling Flow

The RF/HCI processing task handles BLE events from the BLE controller (ISR-driven, registered in
the platform HAL). The `HOST_SUBSYSTEM` task processes the event, performs any related host-side
setting, and — if the event needs to reach the application — invokes the registered callback
(`ble_host_callback_set(APP_GENERAL_EVENT / APP_SERVICE_EVENT / APP_L2CAP_DATA_EVENT, ...)`).
Those callbacks enqueue a TLV-wrapped copy of the event onto the application's own queue
(`g_app_msg_q`), which `app_main_task()` drains in its `for (;;) { xQueueReceive(...); }` loop.

The application-level event opcode is defined in `ble_event.h`
(`components/network/bluetooth/ble-host/ble-cmd-api/include/ble_event.h`), and dispatch to a
specific service's handler is done through the `att_db_link[]` table generated in
`ble_profile_def.c` (see [3.2](#32-step-3-implement-user-data-for-ble-profiles)).

---

## 2. BLE Application Development

This section introduces the BLE application flow implemented in the Rafael RT58x BLE SDK, using
the `periph-trsp` (BLE TRSP Peripheral) demo,
[`examples/bluetooth-le/peripheral/periph-trsp/periph-trsp/src/main.c`](../../examples/bluetooth-le/peripheral/periph-trsp/periph-trsp/src/main.c),
as the concrete example.

### 2.1 BLE Application Flow

```
main()
 └─ xTaskCreate(app_main_entry, "main", ..., E_TASK_PRIORITY_APP, NULL)
      └─ app_main_entry()
           ├─ hosal_lpm_init() / hosal_rf_init(HOSAL_RF_MODE_BLE_CONTROLLER)
           ├─ app_init()                      // queues/semaphores + ble_host_stack_init()
           └─ app_main_task()
                ├─ ble_init()                 // once: callbacks, controller, address, profile init
                ├─ ble_app_request_set(ADV_START)
                └─ for (;;) xQueueReceive(g_app_msg_q, ...)
                       ├─ QUEUE_TYPE_APP_REQ    → ble_app_main() → app_peripheral_handler()
                       └─ QUEUE_TYPE_OTHERS     → ble_evt_handler() / service att_handler()
```

BLE controller and BLE application initialization are implemented in `ble_init()`, issued once
from `app_main_task()`.

Application behavior is implemented in `ble_app_main()`, invoked whenever `app_main_task()`
receives an `APP_REQUEST` from the queue; issue `ble_app_request_set()` to send the application
request to the BLE application task.

If the application task receives a `QUEUE_TYPE_APP_REQ` message it calls `ble_app_main()`; a
`QUEUE_TYPE_OTHERS` message tagged `APP_GENERAL_EVENT` goes to `ble_evt_handler()`, and one tagged
`APP_SERVICE_EVENT` is routed to the registered per-service handler through `att_db_link[]`. A
third tag, `APP_L2CAP_DATA_EVENT`, is reserved for L2CAP data but is not yet handled in the
`periph-trsp` demo.

### 2.2 BLE Initialization

RT58x BLE general initialization, as implemented in `ble_init()` (`main.c:1210`):

1. Register the application's event/service/L2CAP data callbacks with
   `ble_host_callback_set(APP_GENERAL_EVENT | APP_SERVICE_EVENT | APP_L2CAP_DATA_EVENT, ...)`.
2. Initialize the BLE controller (PHY and Link Layer) via `ble_cmd_phy_controller_init()`.
3. Set the device address — from OTP unique code (`ble_cmd_read_unique_code()` +
   `ble_cmd_write_identity_resolving_key()`) if available, else a fixed `DEVICE_ADDR`.
4. Initialize the resolvable address and preferred data length.
5. Initialize BLE services/profiles via `server_profile_init()`. (Optional)

```c
static ble_err_t ble_init(void)
{
    ble_err_t status;
    ble_unique_code_format_t unique_code_param;
    ble_gap_addr_t device_addr_param;

    status = BLE_ERR_OK;
    do
    {
        status = ble_host_callback_set(APP_GENERAL_EVENT, ble_app_event_cb);
        if (status != BLE_ERR_OK) break;

        status = ble_host_callback_set(APP_SERVICE_EVENT, ble_service_data_cb);
        if (status != BLE_ERR_OK) break;

        status = ble_host_callback_set(APP_L2CAP_DATA_EVENT, ble_l2cap_data_cb);
        if (status != BLE_ERR_OK) break;

        status = ble_cmd_phy_controller_init();
        if (status != BLE_ERR_OK) break;

        status = ble_cmd_read_unique_code(&unique_code_param);
        if (status == BLE_ERR_OK)
        {
            device_addr_param.addr_type = unique_code_param.addr_type;
            memcpy(&device_addr_param.addr, &unique_code_param.ble_addr, 6);
            status = ble_cmd_device_addr_set((ble_gap_addr_t *)&device_addr_param);
            if (status != BLE_ERR_OK) break;
            status = ble_cmd_write_identity_resolving_key((ble_sm_irk_param_t *)&unique_code_param.ble_irk[0]);
            if (status != BLE_ERR_OK) break;
        }
        else
        {
            status = ble_cmd_device_addr_set((ble_gap_addr_t *)&DEVICE_ADDR);
            if (status != BLE_ERR_OK) break;
        }

        status = ble_cmd_resolvable_address_init();
        if (status != BLE_ERR_OK) break;

        status = ble_cmd_suggest_data_len_set(BLE_GATT_DATA_LENGTH_MAX);
        if (status != BLE_ERR_OK) break;

        // BLE profile init --> only 1 link (host id = 0)
        status = server_profile_init(APP_TRSP_P_HOST_ID);
        if (status != BLE_ERR_OK) break;
    } while (0);

    return status;
}
```

### 2.3 BLE Main Loop

`app_main_task()` (in `main.c`) runs the main application loop and issues
`ble_app_request_set()` to send an application request to itself for later processing.

Application request definitions (customizable per application), in `main.c`:

```c
typedef enum
{
    APP_REQUEST_IDLE = 0x00,
    APP_REQUEST_ADV_START,
    APP_REQUEST_TRSPS_DATA_SEND,
} app_request_t;
```

`ble_app_request_set()` — call with `request` to control application flow. If called from an ISR,
set `from_isr` to `true`; otherwise `false`. It uses a counting semaphore (`semaphore_app` /
`semaphore_isr`) to bound how many outstanding requests can be queued:

```c
static bool ble_app_request_set(uint8_t host_id, app_request_t request, bool from_isr)
{
    app_queue_t p_app_q;

    p_app_q.event = 0; // from BLE
    p_app_q.param_type = QUEUE_TYPE_APP_REQ;
    p_app_q.param.app_req.host_id = host_id;
    p_app_q.param.app_req.app_req = request;

    if (from_isr == false)
    {
        if (xSemaphoreTake(semaphore_app, 0) == pdTRUE)
        {
            p_app_q.from_isr = false;
            if (xQueueSendToBack(g_app_msg_q, &p_app_q, 0) != pdTRUE)
            {
                xSemaphoreGive(semaphore_app);
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        BaseType_t context_switch = pdFALSE;
        if (xSemaphoreTakeFromISR(semaphore_isr, &context_switch) == pdTRUE)
        {
            p_app_q.from_isr = true;
            if (xQueueSendToBackFromISR(g_app_msg_q, &p_app_q, &context_switch) != pdTRUE)
            {
                xSemaphoreGiveFromISR(semaphore_isr, &context_switch);
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}
```

`ble_app_main()` dispatches to the role-specific handler — this application flow can be
implemented for peripheral and central roles with 0 to *n* connection links, where *n* is less
than or equal to `BLE_SUPPORT_NUM_CONN_MAX` (Kconfig `CONFIG_BLE_SUPPORT_NUM_CONN_MAX`). Link
information is defined in `ble_profile_def.c`, `ble_profile_app.c` and `ble_profile.h` (see
[Chapter 3](#3-ble-profile-implementation)):

```c
static void app_peripheral_handler(app_req_param_t *p_param)
{
    ble_err_t status;
    uint8_t host_id;
    ble_info_link0_t *p_profile_info;

    host_id = p_param->host_id;
    p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    switch (p_param->app_req)
    {
    case APP_REQUEST_ADV_START:
        // ... service data init, set MTU, adv_init(), adv_enable()
        break;
    case APP_REQUEST_TRSPS_DATA_SEND:
        // ... ble_svcs_trsps_server_send()
        break;
    default:
        break;
    }
}

static void ble_app_main(app_req_param_t *p_param)
{
    // Link - Peripheral
    app_peripheral_handler(p_param);
}
```

### 2.4 BLE Event Handler

`ble_evt_handler()` dispatches on event type; each case is split out into a small named handler
function to keep the dispatcher itself short:

```c
static void ble_evt_handler(ble_evt_param_t *p_param)
{
    switch (p_param->event)
    {
    case BLE_ADV_EVT_SET_ENABLE:
        handle_adv_set_enable(&p_param->event_param.ble_evt_adv.param.evt_set_adv_enable);
        break;
    case BLE_GAP_EVT_CONN_COMPLETE:
        handle_conn_complete(&p_param->event_param.ble_evt_gap.param.evt_conn_complete);
        break;
    case BLE_GAP_EVT_CONN_PARAM_UPDATE:
        handle_conn_param_update(&p_param->event_param.ble_evt_gap.param.evt_conn_param_update);
        break;
    case BLE_GAP_EVT_PHY_READ:
    case BLE_GAP_EVT_PHY_UPDATE:
        handle_phy_update(&p_param->event_param.ble_evt_gap.param.evt_phy);
        break;
    case BLE_ATT_GATT_EVT_MTU_EXCHANGE:
        handle_mtu_exchange(&p_param->event_param.ble_evt_att_gatt.param.ble_evt_mtu);
        break;
    case BLE_ATT_GATT_EVT_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH:
        handle_write_suggested_default_data_length(&p_param->event_param.ble_evt_att_gatt.param.ble_evt_suggest_data_length_set);
        break;
    case BLE_ATT_GATT_EVT_DATA_LENGTH_CHANGE:
        handle_data_length_change(&p_param->event_param.ble_evt_att_gatt.param.ble_evt_data_length_change);
        break;
    case BLE_GAP_EVT_DISCONN_COMPLETE:
        handle_disconn_complete(&p_param->event_param.ble_evt_gap.param.evt_disconn_complete);
        break;
    default:
        break;
    }
}
```

Event definitions are defined in `ble_event.h`
(`components/network/bluetooth/ble-host/ble-cmd-api/include/ble_event.h`).

### 2.5 BLE Service Event Handle (Optional)

Implement a BLE service handler if the application supports a BLE service that needs to process
service events. Register the handler through the service's own `_init()` call, e.g.
`ble_svcs_trsps_init(host_id, role, &info, ble_svcs_trsps_evt_handler)`. In the handler, retrieve
the event's GAP role, handle number, and data via `p_param`:

```c
static void ble_svcs_trsps_evt_handler(ble_evt_att_param_t *p_param)
{
    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        switch (p_param->event)
        {
        case BLESERVICE_TRSPS_UDATRW01_WRITE_EVENT:
        case BLESERVICE_TRSPS_UDATRW01_WRITE_WITHOUT_RSP_EVENT:
            // handle client write
            break;
        case BLESERVICE_TRSPS_UDATRW01_READ_EVENT:
            // handle client read, respond with ble_svcs_data_send(TYPE_BLE_GATT_READ_RSP, ...)
            break;
        default:
            break;
        }
    }
}

static ble_err_t server_profile_init(uint8_t host_id)
{
    ble_err_t status = BLE_ERR_OK;
    ble_info_link0_t *p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    ble_app_link_info[host_id].state = STATE_STANDBY;
    do
    {
        status = ble_svcs_gaps_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_gaps), NULL);
        if (status != BLE_ERR_OK) break;
        status = ble_svcs_gaps_device_name_set((uint8_t *)DEVICE_NAME_STR, strlen(DEVICE_NAME_STR));
        if (status != BLE_ERR_OK) break;
        status = ble_svcs_gatts_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_gatts), NULL);
        if (status != BLE_ERR_OK) break;
        status = ble_svcs_dis_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_dis), NULL);
        if (status != BLE_ERR_OK) break;
        status = ble_svcs_trsps_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_trsps), ble_svcs_trsps_evt_handler);
        if (status != BLE_ERR_OK) break;
    } while (0);

    return status;
}
```

---

## 3. BLE Profile Implementation

There are three steps to implement a BLE profile in an application:

- **Step 1**: Implement BLE services (`ble_service_xxx.c` / `ble_service_xxx.h`).
- **Step 2**: Implement the BLE profile definition (`ble_profile_def.c`).
- **Step 3**: Implement user data for the BLE profile (`ble_profile_app.c` / `ble_profile.h`).

### 3.1 Step 1 & 2: Implement Services and Profiles

Available BLE services under
[`components/network/bluetooth/ble-host/ble-service/`](../../components/network/bluetooth/ble-host/ble-service/):

| Service folder | Description |
|---|---|
| `common/` | Shared service infrastructure/types |
| `gaps/` | Generic Access Profile Service |
| `gatts/` | Generic Attribute Profile Service |
| `dis/` | Device Information Service |
| `trsps/` | Transparent Service Profile |
| `bas/` | Battery Service |
| `bls/` | Blood Pressure/Body composition-style Service (see header for exact profile) |
| `hrs/` | Heart Rate Service |
| `hids/` | HID over GATT Service |
| `ots/` | Object Transfer Service |
| `esls/` | Electronic Shelf Label Service |
| `fota/` | BLE FOTA service (`ble_service_fotas.*`) |
| `matter/` | Matter-over-BLE commissioning support |
| `mesh_prov/`, `mesh_proxy/` | BLE Mesh Provisioning / Proxy services |

For the instructions to create a custom BLE service and profile, refer to
`RT58x_BLE_SDK_Service_Profile_Guide.md`
(`docs/BLE/RT58x_BLE_SDK_Service_Profile_Guide.md`).

### 3.2 Step 3: Implement User Data for BLE Profiles

Implement profile-related definitions for the application in `ble_profile_app.c` / `ble_profile.h`
(under each demo's own `ble-app-profile/` folder, e.g.
`examples/bluetooth-le/peripheral/periph-trsp/ble-app-profile/`). Define the minimum number of
connection links for each service, define the data structure for each link, and implement
`link0_svcs_handles_get()` if the BLE GATT role is `BLE_GATT_ROLE_CLIENT`.

#### 3.2.1 Step 3.1: Max. Number of Connection Links per Service

`periph-trsp` demo (TRSP Peripheral) supports one link (host id 0), with GAP/GATT/DIS/TRSPS all
active on it, defined in `ble_profile.h`:

```c
/** Define the maximum number of BLE GAPS link. */
#define MAX_NUM_CONN_GAPS      1
/** Define the maximum number of BLE GATTS link. */
#define MAX_NUM_CONN_GATTS     1
/** Define the maximum number of BLE DIS link. */
#define MAX_NUM_CONN_DIS       1
/** Define the maximum number of BLE TRSPS link. */
#define MAX_NUM_CONN_TRSPS     1
```

(A multi-role/multi-link demo — e.g. `multi-1c1p` — defines these per role.)

#### 3.2.2 Step 3.2: Define User Data and Data Structures for Each Link

`ble_profile.h`:

```c
/** BLE Application Link 0 Profile Attribute Information Structure.*/
typedef struct ble_info_link0_s
{
    ble_svcs_gaps_info_t      svcs_info_gaps;      /**< GAPS information (server). */
    ble_svcs_gatts_info_t     svcs_info_gatts;     /**< GATTS information (server). */
    ble_svcs_dis_info_t       svcs_info_dis;       /**< DIS information (server). */
    ble_svcs_trsps_info_t     svcs_info_trsps;     /**< TRSPS information (server). */
} ble_info_link0_t;

typedef struct ble_app_link_info_s
{
    ble_gap_role_t     gap_role;          /**< GAP role. */
    uint8_t            state;             /**< Current state. */
    void               *profile_info;     /**< Link profile information. */
} ble_app_link_info_t;

/** Extern maximum Number of Host Connection Link Definition. */
extern const uint8_t                              max_num_conn_host;
/** Extern BLE Connection Links Definition. */
extern const ble_att_role_by_id_t                 att_db_link[];
/** Extern BLE Connection Link Parameter Table Definition. */
extern const ble_att_db_mapping_by_id_t           att_db_mapping[];
/** Extern BLE Connection Link Mapping Size Definition. */
extern const ble_att_db_mapping_by_id_size_t      att_db_mapping_size[];
/** Extern BLE application links information which is ordered by host id. */
extern ble_app_link_info_t                        ble_app_link_info[];
```

`ble_profile_def.c`:

```c
const ble_att_param_t *const att_service_comb00[] =
{
    &ATT_NULL_INVALID,       //mandatory, don't remove it.
    ATT_GAPS_SERVICE,
    ATT_GATTS_SERVICE,
    ATT_DIS_SERVICE,
    ATT_TRSPS_SERVICE
};

const ble_att_role_by_id_t att_db_link[] =
{
    // Link 0
    { ((const ble_att_param_t **)0), att_service_comb00 },  // Client Profile, Server Profile
};

ble_att_handle_param_t att_hdl_para_links00[SIZE_ARRAY_ROW(att_service_comb00)]; // Link 0 Server

const ble_att_db_mapping_by_id_t att_db_mapping[] =
{
    // Link 0
    { ((ble_att_handle_param_t *)0), att_hdl_para_links00 },  // Client, Server Link Parameter
};

const ble_att_db_mapping_by_id_size_t att_db_mapping_size[] =
{
    // Link 0
    { 0, SIZE_ARRAY_ROW(att_service_comb00) },  // Client, Server Link Mapping Size
};

const uint8_t max_num_conn_host = (SIZE_ARRAY_ROW(att_db_mapping_size));
```

`ble_profile_app.c`:

```c
/* link0 information */
ble_info_link0_t   ble_info_link0;

/* BLE application links information which is ordered by host id. */
ble_app_link_info_t ble_app_link_info[BLE_SUPPORT_NUM_CONN_MAX] =
{
    // Link 0, host id = 0
    {
        .gap_role = BLE_GAP_ROLE_PERIPHERAL,
        .state = 0x00,
        .profile_info = (void *) &ble_info_link0,
    },
};
```

#### 3.2.3 Step 3.3: Implement Profile-Related Functions

Implement customized functions for the BLE profile in `ble_profile_app.c`. For a **central**
role, implement `link0_svcs_handles_get()` after receiving a `BLE_ATT_GATT_EVT_DB_PARSE_COMPLETE`
event when the GATT role is `BLE_GATT_ROLE_CLIENT` (see the `central-trsp` demo). Its purpose is to
let the application confirm that the attribute handles for every service defined in the central
project's profile have actually been retrieved from the connected peer (GATT database discovery)
before the application starts using those handles for reads/writes.

```c
/** Get BLE link0 Service All Handles */
ble_err_t link0_svcs_handles_get(uint8_t host_id)
{
    ble_err_t status;
    ble_info_link0_t *p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    status = BLE_ERR_OK;
    do
    {
        // Get GAPS handles
        status = ble_svcs_gaps_handles_get(host_id, BLE_GATT_ROLE_CLIENT, (void *)&p_profile_info->svcs_info_gaps);
        if (status != BLE_ERR_OK) break;

        // Get GATTS handles
        status = ble_svcs_gatts_handles_get(host_id, BLE_GATT_ROLE_CLIENT, (void *)&p_profile_info->svcs_info_gatts);
        if (status != BLE_ERR_OK) break;

        // Get DIS handles
        status = ble_svcs_dis_handles_get(host_id, BLE_GATT_ROLE_CLIENT, (void *)&p_profile_info->svcs_info_dis);
        if (status != BLE_ERR_OK) break;

        // Get TRSPS handles
        status = ble_svcs_trsps_handles_get(host_id, BLE_GATT_ROLE_CLIENT, (void *)&p_profile_info->svcs_info_trsps);
        if (status != BLE_ERR_OK) break;
    } while (0);

    return status;
}
```

Each `ble_svcs_<name>_handles_get()` call fills in the handle table for one service from
`p_profile_info`; if any call fails, the corresponding service's handles were not found on the
peer's GATT database, and `status` reports which one broke out of the `do { } while (0)` chain.

---

## 4. BLE SDK APIs

Rafael RT58x BLE SDK command APIs are declared across the headers in
[`components/network/bluetooth/ble-host/ble-cmd-api/include/`](../../components/network/bluetooth/ble-host/ble-cmd-api/include/),
organized into 10 groups: Common, Advertising, Scan, GAP, ATT/GATT, Security, Privacy, Connect
CTE, Extended & Periodic Advertising, and L2CAP.

### 4.1 BLE Common APIs

Header: `ble_common.h`.

| Function | Description | BLE Event |
|---|---|---|
| `ble_err_t ble_cmd_phy_controller_init(void)` | Set PHY controller initialization. | — |
| `ble_err_t ble_cmd_read_unique_code(ble_unique_code_format_t *p_param)` | Read the BLE unique code burned into OTP via MP Tool. | — |
| `ble_err_t ble_cmd_read_filter_accept_list_size(void)` | Read filter accept list size. | `BLE_COMMON_EVT_READ_FILTER_ACCEPT_LIST_SIZE` |
| `ble_err_t ble_cmd_clear_filter_accept_list(void)` | Clear the filter accept list. | `BLE_COMMON_EVT_CLEAR_FILTER_ACCEPT_LIST` |
| `ble_err_t ble_cmd_add_device_to_accept_list(ble_filter_accept_list_t *p_accept_list)` | Add a device to the filter accept list. | `BLE_COMMON_EVT_ADD_FILTER_ACCEPT_LIST` |
| `ble_err_t ble_cmd_remove_device_from_accept_list(ble_filter_accept_list_t *p_accept_list)` | Remove a device from the filter accept list. | `BLE_COMMON_EVT_REMOVE_FILTER_ACCEPT_LIST` |
| `ble_err_t ble_cmd_antenna_info_read(void)` | Read antenna information. | `BLE_COMMON_EVT_READ_ANTENNA_INFO` |

### 4.2 BLE Advertising APIs

Header: `ble_advertising.h`.

| Function | Description | BLE Event |
|---|---|---|
| `ble_err_t ble_cmd_adv_data_set(ble_adv_data_param_t *p_param)` | Set BLE advertising data. | — |
| `ble_err_t ble_cmd_adv_scan_rsp_set(ble_adv_data_param_t *p_param)` | Set BLE scan response data. | — |
| `ble_err_t ble_cmd_adv_param_set(ble_adv_param_t *p_param)` | Set BLE advertising parameters. Min interval ≤ Max interval, within `ADV_INTERVAL_MIN`/`ADV_INTERVAL_MAX`. | — |
| `ble_err_t ble_cmd_adv_enable(uint8_t host_id)` | Start advertising. `host_id` = link's host ID, or `BLE_HOSTID_RESERVED` for non-connectable/scannable advertising. | `BLE_ADV_EVT_SET_ENABLE` |
| `ble_err_t ble_cmd_adv_disable(void)` | Stop advertising. | `BLE_ADV_EVT_SET_ENABLE` |

### 4.3 BLE Scan APIs

Header: `ble_scan.h`.

| Function | Description | BLE Event |
|---|---|---|
| `ble_err_t ble_cmd_scan_param_set(ble_scan_param_t *p_param)` | Set scan parameters. Scan window ≤ scan interval. | — |
| `ble_err_t ble_cmd_scan_enable(void)` | Start scanning. Note: for multi-connection scanning, keep the current connection interval > 10ms × number of links. | `BLE_SCAN_EVT_SET_ENABLE`, `BLE_SCAN_EVT_ADV_REPORT` |
| `ble_err_t ble_cmd_scan_disable(void)` | Stop scanning. | `BLE_SCAN_EVT_SET_ENABLE` |
| `ble_err_t ble_cmd_scan_report_adv_data_parsing(ble_evt_scan_adv_report_t *p_rpt_data, ble_gap_ad_type_t ad_type, uint8_t *p_data, uint8_t *p_data_length)` | Parse scanned advertising data by AD type. | — |

### 4.4 BLE GAP APIs

Header: `ble_gap.h`.

| Function | Description | BLE Event |
|---|---|---|
| `ble_err_t ble_cmd_device_addr_get(ble_gap_addr_t *p_addr)` | Get local device BLE address and type. Address is little-endian, length `BLE_ADDR_LEN`. | — |
| `ble_err_t ble_cmd_device_addr_set(ble_gap_addr_t *p_addr)` | Set local device BLE address and type. | — |
| `ble_err_t ble_cmd_conn_create(ble_gap_create_conn_param_t *p_param)` | Create a connection. | `BLE_GAP_EVT_CONN_COMPLETE` |
| `ble_err_t ble_cmd_extend_conn_create_v2(ble_gap_extend_create_conn_v2_param_t *p_param)` | Create a connection using extended/multi-PHY (1M/2M/Coded) parameters. | `BLE_GAP_EVT_CONN_COMPLETE` |
| `ble_err_t ble_cmd_conn_create_cancel(void)` | Cancel an in-progress connection creation. | `BLE_GAP_EVT_CONN_CANCEL` |
| `ble_err_t ble_cmd_conn_param_update(ble_gap_conn_param_update_param_t *p_param)` | Update connection parameters. Note: for multi-link, keep interval > 10ms × number of links. | `BLE_GAP_EVT_CONN_PARAM_UPDATE` |
| `ble_err_t ble_cmd_conn_terminate(uint8_t host_id)` | Terminate a connection link. | `BLE_GAP_EVT_DISCONN_COMPLETE` |
| `ble_err_t ble_cmd_phy_update(ble_gap_phy_update_param_t *p_param)` | Set PHY update. | `BLE_GAP_EVT_PHY_UPDATE` |
| `ble_err_t ble_cmd_phy_read(uint8_t host_id)` | Read RF PHY. | `BLE_GAP_EVT_PHY_READ` |
| `ble_err_t ble_cmd_default_phy_set(ble_gap_default_phy_set_param_t *p_param)` | Set the default preferred PHY used for future connections. | — |
| `ble_err_t ble_cmd_rssi_read(uint8_t host_id)` | Read RSSI. | `BLE_GAP_EVT_RSSI_READ` |
| `ble_err_t ble_cmd_host_ch_classif_set(ble_gap_host_ch_classif_t *p_param)` | Set host channel classification. | — |
| `ble_err_t ble_cmd_channel_map_read(uint8_t host_id)` | Read channel map. | `BLE_GAP_EVT_READ_CHANNEL_MAP` |
| `ble_err_t ble_cmd_resolvable_address_init(void)` | Initialize resolvable address. | — |
| `ble_err_t ble_cmd_regenerate_resolvable_address(uint8_t host_id, uint8_t en_new_irk)` | Regenerate resolvable address; optionally generate a new IRK. | — |

### 4.5 BLE ATT and GATT APIs

Header: `ble_att_gatt.h`.

| Function | Description | Role | BLE Event |
|---|---|---|---|
| `ble_err_t ble_cmd_suggest_data_len_set(uint16_t tx_octets)` | Set preferred data length (`BLE_GATT_DATA_LENGTH_MIN`..`_MAX`). | any | — |
| `ble_err_t ble_cmd_default_mtu_size_set(uint8_t host_id, uint16_t mtu)` | Set preferred MTU size (`BLE_GATT_ATT_MTU_MIN`..`_MAX`). | any | — |
| `ble_err_t ble_cmd_mtu_size_update(uint8_t host_id, uint16_t mtu)` | ATT_MTU exchange request. | Client only | `BLE_ATT_GATT_EVT_MTU_EXCHANGE` |
| `ble_err_t ble_cmd_data_len_update(uint8_t host_id, uint16_t tx_octets)` | Data length update. | any | `BLE_ATT_GATT_EVT_DATA_LENGTH_CHANGE` |
| `ble_err_t ble_cmd_mtu_size_get(uint8_t host_id, uint16_t *mtu)` | Get GATT MTU size. | any | — |
| `ble_err_t ble_cmd_gatt_read_rsp(ble_gatt_data_param_t *p_param)` | GATT Read Response. | Server only | — |
| `ble_err_t ble_cmd_gatt_read_by_type_rsp(ble_gatt_data_param_t *p_param)` | GATT Read By Type Response. | Server only | — |
| `ble_err_t ble_cmd_gatt_read_blob_rsp(ble_gatt_data_param_t *p_param)` | GATT Read Blob Response. | Server only | — |
| `ble_err_t ble_cmd_gatt_error_rsp(ble_gatt_err_rsp_param_t *p_param)` | GATT Error Response. | Server only | — |
| `ble_err_t ble_cmd_gatt_notification(ble_gatt_data_param_t *p_param)` | Notify a Characteristic Value without ack. | Server only | — |
| `ble_err_t ble_cmd_gatt_indication(ble_gatt_data_param_t *p_param)` | Indicate a Characteristic Value with ack. | Server only | — |
| `ble_err_t ble_cmd_gatt_write_req(ble_gatt_data_param_t *p_param)` | GATT Write Request (acked). | Client only | — |
| `ble_err_t ble_cmd_gatt_write_cmd(ble_gatt_data_param_t *p_param)` | GATT Write Command (unacked). | Client only | — |
| `ble_err_t ble_cmd_gatt_read_req(ble_gatt_read_req_param_t *p_param)` | GATT Read Request by known handle. | Client only | — |
| `ble_err_t ble_cmd_gatt_read_blob_req(ble_gatt_read_blob_req_param_t *p_param)` | GATT Read Blob Request (offset read) by known handle. | Client only | — |
| `ble_err_t ble_cmd_gatt_att_handle_mapping_get(ble_gatt_handle_table_param_t *p_param)` | Get the attribute handle mapping table for a link. | any | — |

### 4.6 BLE Security APIs

Header: `ble_security_manager.h`.

| Function | Description | BLE Event |
|---|---|---|
| `ble_err_t ble_cmd_security_request_set(uint8_t host_id)` | Send a security request. | — |
| `ble_err_t ble_cmd_passkey_set(ble_sm_passkey_param_t *p_param)` | Set the pairing passkey value. | — |
| `ble_err_t ble_cmd_numeric_comp_result_set(ble_sm_numeric_comp_result_param_t *p_param)` | Set the result of a Numeric Comparison (LESC "Just Works"/Numeric Comparison pairing). | — |
| `ble_err_t ble_cmd_io_capability_set(ble_sm_io_cap_param_t *p_param)` | Set IO capabilities. | — |
| `ble_err_t ble_cmd_bonding_flag_set(ble_sm_bonding_flag_param_t *p_param)` | Set bonding flags. | — |
| `ble_err_t ble_cmd_cccd_restore(uint8_t host_id)` | Restore CCCD for a bonded link. | — |
| `ble_err_t ble_cmd_bonding_space_init(void)` | Initialize the bonding storage space. | — |
| `ble_err_t ble_cmd_write_identity_resolving_key(ble_sm_irk_param_t *p_param)` | Write the identity resolving key (IRK). | — |
| `ble_err_t ble_cmd_lesc_init(void)` | Initialize LE Secure Connections. | — |

### 4.7 BLE Privacy APIs

Header: `ble_privacy.h`.

| Function | Description |
|---|---|
| `ble_err_t ble_cmd_privacy_enable(ble_set_privacy_cfg_t *p_param)` | Enable BLE privacy. |
| `ble_err_t ble_cmd_privacy_disable(void)` | Disable BLE privacy. |

### 4.8 BLE Connect CTE APIs

Header: `ble_connect_cte.h`.

| Function | Description | BLE Event |
|---|---|---|
| `ble_err_t ble_cmd_connection_cte_receive_parameters_set(ble_connection_cte_rx_param_t *p_param)` | Set connection CTE receiver parameters. | `BLE_CTE_EVT_SET_CONN_CTE_RX_PARAM` |
| `ble_err_t ble_cmd_connection_cte_transmit_parameters_set(ble_connection_cte_tx_param_t *p_param)` | Set connection CTE transmit parameters. | `BLE_CTE_EVT_SET_CONN_CTE_TX_PARAM` |
| `ble_err_t ble_cmd_connection_cte_request_enable(ble_connection_cte_req_enable_t *p_param)` | Enable connection CTE request. | `BLE_CTE_EVT_SET_CONN_CTE_REQ` |
| `ble_err_t ble_cmd_connection_cte_response_enable(ble_connection_cte_rsp_enable_t *p_param)` | Enable connection CTE response. | `BLE_CTE_EVT_SET_CONN_CTE_RSP` |

Also delivered via the same connection-CTE event path: `BLE_CTE_EVT_IQ_REPORT` and
`BLE_CTE_EVT_CTE_REQ_FAILED` events.

### 4.9 BLE Extended & Periodic Advertising APIs

Header: `ble_extended_advertising.h` (impl. `ble_cmd_extended_advertising.c`) — the SDK's
Bluetooth 5 Extended/Periodic Advertising surface.

| Function | Description | BLE Event |
|---|---|---|
| `ble_err_t ble_cmd_extended_adv_parameter_set(ble_extended_adv_param_set_t *p_param)` | Set extended advertising parameters (adv handle, event properties, primary/secondary PHY, SID, etc.). | `BLE_EADV_EVT_SET_EXTENDED_ADV_PARAM` |
| `ble_err_t ble_cmd_extended_adv_data_set(ble_extended_adv_data_set_t *p_param)` | Set extended advertising data for an adv handle. | `BLE_EADV_EVT_SET_EXTENDED_ADV_DATA` |
| `ble_err_t ble_cmd_extended_adv_enable_set(uint8_t host_id, ble_extended_adv_enable_set_t *p_param)` | Enable/disable an extended advertising set. | — |
| `ble_err_t ble_cmd_adv_set_random_addr_set(ble_adv_set_random_address_set_t *p_param)` | Set the random address used by an extended advertising set. | `BLE_EADV_EVT_SET_ADV_SET_RANDOM_ADDR` |
| `ble_err_t ble_cmd_periodic_adv_parameter_set(ble_periodic_adv_param_set_param_t *p_param)` | Set periodic advertising parameters (interval, properties, subevent config for PAwR). | — |
| `ble_err_t ble_cmd_periodic_adv_data_set(ble_periodic_adv_data_set_param_t *p_param)` | Set periodic advertising data. | — |
| `ble_err_t ble_cmd_periodic_adv_enable_set(ble_periodic_adv_enable_set_param_t *p_param)` | Enable periodic advertising. | — |
| `ble_err_t ble_cmd_periodic_adv_disable_set(ble_periodic_adv_disable_set_param_t *p_param)` | Disable periodic advertising. | — |
| `ble_err_t ble_cmd_periodic_adv_rsp_data_set(ble_periodic_adv_rsp_data_set_param_t *p_param)` | Set response data for a Periodic Advertising with Responses (PAwR) subevent. | — |
| `ble_err_t ble_cmd_periodic_adv_subevent_data_set(ble_periodic_adv_subevent_data_set_param_t *p_param)` | Set PAwR subevent data. | — |
| `ble_err_t ble_cmd_periodic_adv_set_info_transfer_set(ble_periodic_adv_set_info_transfer_set_param_t *p_param)` | Transfer periodic-adv set info to a connected peer. | — |
| `ble_err_t ble_cmd_periodic_adv_receive_enable(uint16_t sync_handle)` | Enable reception of a periodic advertising train. | — |
| `ble_err_t ble_cmd_periodic_adv_receive_disable(uint16_t sync_handle)` | Disable reception of a periodic advertising train. | — |
| `ble_err_t ble_cmd_periodic_advertising_sync_terminate(ble_periodic_advertising_sync_terminate_param_t *p_param)` | Terminate periodic advertising sync. | — |
| `ble_err_t ble_cmd_periodic_sync_subevent_set(ble_periodic_sync_subevent_param_t *p_param)` | Select which PAwR subevents to synchronize to. | — |
| `ble_err_t ble_cmd_periodic_adv_sync_transfer_param_set(ble_periodic_adv_sync_transfer_param_t *p_param)` | Set periodic advertising sync transfer parameters for a link. | `BLE_PADV_EVT_SET_PADV_SYNC_TRANSFER_PARAM` |
| `ble_err_t ble_cmd_default_periodic_adv_sync_transfer_param_set(ble_default_periodic_adv_sync_transfer_param_t *p_param)` | Set the default periodic advertising sync transfer parameters. | `BLE_PADV_EVT_SET_DEFAULT_PADV_SYNC_TRANSFER_PARAM` |

> Isochronous Channels (CIS/BIS) are only present as bitmask/event-code stubs
> (`ble_common.h`, `ble_hci_event.h`) — there is no dedicated `ble_cmd_iso*` command group yet, so
> full ISO channel support should not be claimed in application design docs.

### 4.10 BLE L2CAP APIs

Header: `ble_l2cap.h` (impl. `ble_cmd_l2cap.c`) — L2CAP connection-oriented channel support. Data
is delivered to the app via `ble_l2cap_data_cb()` / `APP_L2CAP_DATA_EVENT`, described in
[2.1](#21-ble-application-flow).

| Function | Description |
|---|---|
| `ble_err_t ble_cmd_l2cap_chan_connect(ble_l2cap_chan_connect_t *p_param)` | Open an L2CAP connection-oriented channel. |
| `ble_err_t ble_cmd_l2cap_data_send(ble_l2cap_data_send_t *p_param)` | Send data over an L2CAP channel. |

