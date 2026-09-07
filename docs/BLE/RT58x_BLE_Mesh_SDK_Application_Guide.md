# RT58x BLE Mesh SDK Application Guide

## Introduction

The RT58x family is built around an ARM Cortex-M CPU capable of running multiple wireless
protocols concurrently, including Bluetooth LE, Bluetooth Mesh, Zigbee, 802.15.4, and 2.4GHz
proprietary links.

This document covers the BLE Mesh example demos under
[`examples/bluetooth-le/mesh/`](../../examples/bluetooth-le/mesh/): the `lightness-trsp` server
demo (a mesh node with a lightness element and a vendor transparent-data model) and the
`gateway` demo (a provisioner/network manager driven either by a debug-console CLI or by a
binary UART command set). The gateway's UART wire protocol is documented separately in
[`RT58x_BLE_Mesh_Gateway_Command_Manual.md`](RT58x_BLE_Mesh_Gateway_Command_Manual.md).

## 1. lightness-trsp — Mesh Server Demo

Path: [`examples/bluetooth-le/mesh/lightness-trsp/`](../../examples/bluetooth-le/mesh/lightness-trsp/).
Advertised device name: `BLE_MESH` (`lightness-trsp/src/main.c`, `DEVICE_NAME`).

The demo node exposes **two elements** — confirmed via
`ble-mesh-element/src/ble_mesh_element_def.c`:

- **Element 0** — 8 models:
  - Light Lightness Server
  - Light Lightness Setup Server
  - Generic Level Server (`is_extended = TRUE`, extends the Light Lightness model, sharing state
    through `mmdl_light_lightness_ex_cb`)
  - Generic OnOff Server (`is_extended = TRUE`, same extension mechanism)
  - Scene Server
  - Scene Setup Server
  - Vendor "Rafael TRSP" Server
  - Vendor "Rafael TRSP" Client
- **Element 1** — 3 models: a standalone Generic OnOff Server (not extended), Scene Server, and
  Scene Setup Server, controlling a second, independent on/off output.

### 1.1 Lightness Model

Element 0's Light Lightness server is extended by Generic OnOff and Generic Level servers —
setting either extended model's state invokes the lightness model's extension callback
(`mmdl_light_lightness_ex_cb`, `ble_mesh_element_def.c:75,96`) so the three models share one
underlying state.

### 1.2 Vendor Model ("Rafael TRSP")

Defined in `components/network/bluetooth/ble-mesh/model/vendor/include/mmdl_trsp_common.h`.
Company ID is `0x0864`. The model ID macro packs the model number into the high 16 bits and the
company ID into the low 16 bits:

```c
#define BLE_COMPANY_ID                      0x0864
#define MMDL_RAFAEL_TRSP_MDL_ID_DEF(id_2B)   ((id_2B)*0x00010000 | (BLE_COMPANY_ID))

#define MMDL_RAFAEL_TRSP_SR_MDL_ID   MMDL_RAFAEL_TRSP_MDL_ID_DEF(0x0831)   // 0x08310864
#define MMDL_RAFAEL_TRSP_CL_MDL_ID   MMDL_RAFAEL_TRSP_MDL_ID_DEF(0x0832)   // 0x08320864
```

- "Rafael TRSP Server" model ID: **`0x08310864`**
- "Rafael TRSP Client" model ID: **`0x08320864`**

Three vendor opcodes, transmitted as 3 wire octets (opcode byte, then company ID low byte, then
company ID high byte — standard SIG vendor-opcode encoding):

| Message | Wire opcode | Notes |
|---|---|---|
| Rafael TRSP Status | `C0 64 08` | Sent automatically by the server in reply to a Set |
| Rafael TRSP Set | `C1 64 08` | Acknowledged — triggers an automatic Status loopback |
| Rafael TRSP Set Unacknowledged | `C2 64 08` | No automatic reply |

Confirmed in `model/vendor/src/mmdl_trsp_sr_main.c:52,56,89-92`: the server only sends the
auto-loopback Status for the acknowledged Set opcode, not for Set Unacknowledged.

**Maximum payload and timing.** The maximum vendor-model payload the demo will transmit in one
call is `MAX_TRSP_DATA_LEN = 377` bytes (`lightness-trsp/src/main.c:54`). Large payloads are
split into Lower Transport segments and retried until acknowledged; the actual delay formula,
confirmed in `lib-ble-mesh/profile/transport/src/transport.c` (`tx_retry_timer_delay_get()`), is:

```
delay = base_timeout + (per_hop_addition × TTL × segment_count × network_transmit_count)
```

with `base_timeout = 200ms` and `per_hop_addition = 50ms`
(`lib-ble-mesh/profile/transport/include/transport.h`). The base timeout is a flat addend, **not**
multiplied by the transmit count.

The TRSP client/server hardcode `TTL = 4` for their own traffic
(`model/vendor/src/mmdl_trsp_cl_main.c:36`, `mmdl_trsp_sr_main.c:35`). `network_transmit_count`
is the mesh stack's global default, currently `5`
(`lib-ble-mesh/profile/config-model/src/cfg_mdl_mib.c:171` — the source comment on this line
reads `//Nat need to check`, so treat `5` as this build's provisional default rather than a fixed
spec value).

With `TTL = 4` and `network_transmit_count = 5`:

- A 1-segment message: `200 + (50 × 4 × 1 × 5)` = **1200 ms**
- A 32-segment message (the largest a 377-byte payload needs): `200 + (50 × 4 × 32 × 5)` =
  **32200 ms**

### 1.3 Handling Lightness/OnOff Data Received Over BLE Mesh

Callback registration lives in
[`ble-mesh-element/src/ble_mesh_element_def.c`](../../examples/bluetooth-le/mesh/lightness-trsp/ble-mesh-element/src/ble_mesh_element_def.c);
the callbacks themselves are defined in
[`lightness-trsp/src/mesh_mdl_handler.c`](../../examples/bluetooth-le/mesh/lightness-trsp/lightness-trsp/src/mesh_mdl_handler.c).

Element 0's lightness state (driven by the Lightness/Level/OnOff extension group) is handled by:

```c
void app_process_element_lightness_model_state(uint16_t element_address, uint16_t state)
{
    uint8_t element_idx, duty;

    element_idx = element_address - pib_primary_address_get();

    printf("Set element[%d] act level %d\n", element_idx, state);
    if (element_idx == 0)
    {
        duty = 100-((state+1)/655);
        if ((state > 0) && (duty == 0))
        {
            duty = 1;
            duty = 1;
        }
        hosal_pwm_fmt0_duty(ELEMENT0_PWN_ID, duty);
    }
}
```

Element 1's standalone Generic OnOff state is handled by a separate callback:

```c
void app_process_element_onoff_model_state(uint16_t element_address, uint16_t state)
{
    uint8_t element_idx;

    element_idx = element_address - pib_primary_address_get();

    if (element_idx == 1)
    {
        if(state == 1)
        {
            hosal_pwm_fmt0_duty(ELEMENT1_PWN_ID, 0);
            printf("Set element[%d] on\n", element_idx);
        }
        else
        {
            hosal_pwm_fmt0_duty(ELEMENT1_PWN_ID, 100);
            printf("Set element[%d] off\n", element_idx);
        }
    }
}
```

The element/model tables that wire these callbacks up (`ble_mesh_element_def.c`):

```c
/* Declare the model list for element 0 */
ble_mesh_model_param_t *el0_light_lightness_element_model_list[] =
{
    &el0_light_lightness_model,
    &el0_light_lightness_setup_model,
    &el0_gen_level_model,
    &el0_gen_on_off_model,
    &el0_scene_model,
    &el0_scene_setup_model,
    &el0_raf_trsp_sr_model,
    &el0_raf_trsp_cl_model,
};

/* Declare the model list for element 1 */
ble_mesh_model_param_t *el1_gen_on_off_element_model_list[] =
{
    &el1_gen_on_off_model,
    &el1_scene_model,
    &el1_scene_setup_model,
};

const uint8_t el0_models_count = (sizeof(el0_light_lightness_element_model_list) / sizeof(el0_light_lightness_element_model_list[0]));
const uint8_t el1_models_count = (sizeof(el1_gen_on_off_element_model_list) / sizeof(el1_gen_on_off_element_model_list[0]));

ble_mesh_element_param_t g_element_info[] =
{
    // element 0, lightness element
    { el0_light_lightness_element_model_list, el0_models_count, 0xFFFF, 0 },
    // element 1, onoff element
    { el1_gen_on_off_element_model_list, el1_models_count, 0xFFFF, 0 },
};
```

Each element's model count is computed at compile time as `sizeof(list)/sizeof(list[0])`, so it
always matches its model list's length — it isn't a hand-maintained literal that can drift out of
sync with the array above it.

### 1.4 Handling Vendor Data Received Over BLE Mesh

```c
void app_process_element_raf_trsp_sr_model_state(raf_trsp_cb_params_t *p_raf_trsp_cb_params)
{
    printf("Get Rafael TRSP data from address 0x%04x\n", p_raf_trsp_cb_params->src_addr);

    for (uint16_t i = 0; i < p_raf_trsp_cb_params->data_len; i++)
    {
        printf("%02x ", p_raf_trsp_cb_params->data[i]);
    }
    printf("\n");
}
```

`p_raf_trsp_cb_params` fields: `src_addr`/`dst_addr` (source/destination of the received TRSP
data), `appkey_index`, `is_group` (group vs. unicast delivery), `data_len`, `data`.

**The vendor Client model has no state-change callback in current source** —
`el0_raf_trsp_cl_model`'s callback field is `NULL` (`ble_mesh_element_def.c:166`); there is no
`app_process_element_raf_trsp_cl_model_state()` function anywhere in the codebase. The Client
role only originates Set/Set-Unacknowledged requests (`model/vendor/src/mmdl_trsp_cl_main.c`);
it does not need an app callback to receive the Server's automatic Status loopback.

### 1.5 Running the "lightness-trsp" Demo

1. Connect the EVK board's USB port and start a serial terminal (PuTTY or Tera Term) at 115200
   baud.
2. Reset the device to factory-new and start provisioning, via the board buttons
   (`app_button_handler()`, `lightness-trsp/src/main.c`):
   - **Button 1** — factory reset + start BLE Mesh provisioning (`APP_MESH_PROVISION_EVT`).
   - **Button 2** — factory reset + start BLE GATT provisioning (`APP_GATT_PROVISION_EVT`).

**Testing with a smartphone.** See
`[Tool_05]Android_APP_BLE_Mesh_user_guide_V1.3`, section 5, for provisioning a device into the
mesh network from the Android app. Once provisioned, the app can drive the lightness element's
LED and send data through the vendor model.

**Testing with another device.** After multiple boards are provisioned, each board's log prints
its primary address. Use one board's UART console to target another board's vendor model
directly, via the `DstAddr0x<addr>` / `TxAckEnable<0|1>` console commands
(`lightness-trsp/src/main.c`, `g_user_cmd_list[]`):

Set the destination address to `0x0003`:

```
sysinfo: sys ver 0001
Device provisioned ... primary address 0x0004

destination address invalid, uart data ignore
command example:
DstAddr0x001c
TxAckEnable0
DstAddr0x0003
destination address for uart data: 0x0003
```

Any subsequent UART input is now sent as a TRSP Set to `0x0003`:

```
TxAckEnable0
DstAddr0x0003
destination address for uart data: 0x0003
123456
Send Rafael TRSP set, dst addr 0x0003
31 32 33 34 35 36
... result 0
```

The receiving board (primary address `0x0003`) logs the incoming vendor data:

```
Device provisioned ... primary address 0x0003
Get Rafael TRSP set from address 0x0004
31 32 33 34 35 36
```

Enable `TxAckEnable1` to request that the destination loop the data back to the sender — the
sender then also logs the returned "Rafael TRSP status":

```
TxAckEnable1
uart data tx with ack 1
12345
Send Rafael TRSP set, dst addr 0x0003
31 32 33 34 35
... result 0
Get Rafael TRSP status from address 0x0003
31 32 33 34 35
```

## 2. gateway — Mesh Provisioner/Network Manager Demo

Path: [`examples/bluetooth-le/mesh/gateway/`](../../examples/bluetooth-le/mesh/gateway/). The
gateway demo exposes two independent control surfaces: a debug-console CLI (UART0) and a binary
UART command set (UART1) for a host MCU — see
[`RT58x_BLE_Mesh_Gateway_Command_Manual.md`](RT58x_BLE_Mesh_Gateway_Command_Manual.md) for the
full UART wire protocol.

### 2.1 Running the Demo with CLI Commands

1. Connect the EVK board's USB port.
2. Start a serial terminal (PuTTY or Tera Term).
3. Configure the baud rate to 115200, no parity, 1 stop bit.

CLI commands, from `examples/bluetooth-le/mesh/gateway/cli/src/cli_cmd_gateway.c`:

| Command | Usage | Example | Description |
|---|---|---|---|
| `help` | `help` | `help` | List commands |
| `provisionall` | `provisionall [start]` | `provisionall 1` | Start/stop provisioning all unprovisioned devices |
| `compget` | `compget [address] [page num]` | `compget 0x0123 0` | Get a device's Composition Data |
| `appkeyadd` | `appkeyadd [address]` | `appkeyadd 0x0123` | Add the App key to a device (index 0) |
| `modelappbind` | `modelappbind [address] [model id]` | `modelappbind 0x0123 0x1000` | Bind App key index 0 to a specific model |
| `onoff` | `onoff [address] [onoffstatus]` | `onoff 0x0123 0` | Set a device's Generic OnOff state |
| `level` | `level [address] [levelstatus]` | `level 0x0123 0x8000` | Set a device's Generic Level state |
| `nodereset` | `nodereset [address]` | `nodereset 0x0123` | Reset a device to factory-new |
| `devkeyset` | `devkeyset [address] [key] [key] [key] [key]` | — | Manually inject a device key into the gateway |
| `subadd` | `subadd [dst address] [element address] [address] [model ID]` | — | Add a model subscription |
| `subdel` | `subdel [dst address] [element address] [address] [model ID]` | — | Delete a model subscription |

Provisioned-device storage note: device keys of provisioned devices are kept in
`prov_device_list[MAX_PROV_DEVICE_NUM]` with `MAX_PROV_DEVICE_NUM = 50`
(`cli_cmd_gateway.c`) — an in-RAM array, not persisted to flash, so it resets to empty on every
gateway reboot (previously-provisioned devices then need to be reconfigured, though they remain
provisioned in the mesh network itself).

**Provision a new device.** After resetting a `lightness-trsp` device to factory-new (which
starts it advertising for provisioning), run `provisionall 1` to scan for and provision every
unprovisioned device in range:

```
~# RT58x SDK for BLE Mesh ...
BLE Mesh Client Stack initial ...
timer_low_running_handler(0)
MAC address: 0xce 0x00 0xfc 0xf3 0x44 0xa0
sysinfo: sys ver 0001
~# provisionall 1
auto provision device start
~# start provision device: 0x0309030303080300060f030e040e0403
Send link open to 03 09 03 03 03 08 03 00 06 0f 03 0e 04 0e 04 03
Send link open to 03 09 03 03 03 08 03 00 06 0f 03 0e 04 0e 04 03
Send Provisioning Invite PDU.
Send Provisioning PDU
Get MESH_PRV_PDU_CAPABILITIES
Send Start PDU.
```

**Configure the device.** Once provisioning completes, use `appkeyadd` and `modelappbind` to add
the App key and bind it to the target model:

```
Get MESH_PRV_PDU_COMPLETE
Provision success
device uuid: 0x0309030303080300060f030e040e0403
element address 0x0100
element count 1

~# appkeyadd 0x0100
add app key to node 0x0100
~# config appkey status [src address: 0x0100]
status 0x00, index: 0
modelappbind 0x0100 0x1000
bind device 0x0100's app key to model 0x1000
~# config model app status [src address: 0x0100]
status 0x00, element_address: 0x0100, appkey index: 0 sig model ID 0x1000
```

**Control the device.** After binding the App key, control the model directly from the CLI:

```
~#
~# onoff 0x0100 0
set onoff status 0 to node 0x0100
~# generic onoff status [src address: 0x0100]
App key index: 0
present onoff: 0
onoff 0x0100 1
set onoff status 1 to node 0x0100
~# generic onoff status [src address: 0x0100]
App key index: 0
present onoff: 1
```

### 2.2 Running the Demo with UART Command

Connect a UART-to-USB dongle to the EVK board's UART1 port and send the binary command frames
from a host application. See
[`RT58x_BLE_Mesh_Gateway_Command_Manual.md`](RT58x_BLE_Mesh_Gateway_Command_Manual.md) for the
full command/event reference.
