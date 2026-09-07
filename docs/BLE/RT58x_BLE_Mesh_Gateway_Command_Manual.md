# RT58x BLE Mesh Gateway Command Manual

## Introduction

This document specifies the binary UART command protocol used to control a BLE Mesh gateway
module. It covers Mesh Network management (provisioning, keys), Device Configuration (the
standard Bluetooth Mesh Configuration and Health Foundation Models), and Device Application
commands (the standard Bluetooth Mesh Generic/Sensor/Time/Scene/Scheduler/Lighting models).

This is the wire protocol implemented by
[`gateway`](../../examples/bluetooth-le/mesh/gateway/) — see `ble_mesh_gateway_cmd_handle.c` for
the command dispatch tables and `ble_mesh_gateway.h` for the command-ID/opcode definitions. The
underlying mesh stack lives under `components/network/bluetooth/ble-mesh/`.

## Hardware Interface Setup

The gateway module is connected to the host control unit over UART. Default settings: 115200
baud, 8-bit data, no parity, 1 stop bit.

## Command Data Format

Commands and events share the same little-endian frame:

| Header | Length | Command ID | Sub-Header | Parameter | Checksum |
|---|---|---|---|---|---|
| 4 octets | 1 octet | 4 octets | n octets | n octets | 1 octet |

- **Header** — 4 bytes: `0xFF 0xFC 0xFC 0xFF`. Unlike the sibling ESL AP protocol (which uses a
  distinct header for events), this gateway protocol uses the **same** header magic for both
  host→gateway commands and gateway→host events/status — confirmed via source
  (`ble_mesh_gateway_cmd_send()` is the single function used for both outbound command
  acknowledgements and unsolicited event forwarding).
- **Length** — 1 octet (`uint8_t`). The sum of the Command ID, Sub-Header, and Parameter field
  lengths (i.e. everything between Length and Checksum). Confirmed genuinely 1 octet via source
  (`gateway_cmd_hdr.len`, `uart_handler.c`).
- **Command ID** — 4 octets, one of:

  | Command ID | Description |
  |---|---|
  | `0x1000-0000` | Mesh network service command |
  | `0x1000-1000` | Device configuration service command |
  | `0x1000-2000` | Device application service command |

- **Sub-Header** — variable, depends on Command ID:

  | Command ID group | Sub-Header |
  |---|---|
  | Mesh network service | Opcode only (1 octet) |
  | Device configuration service | Address (2 octets) + Opcode |
  | Device application service | Address (2 octets) + App key index (2 octets) + Opcode |

- **Opcode** — 1–3 octets (the PDF this manual is derived from states "1–4 octets"; the
  Bluetooth Mesh access-layer opcode encoding used here has a maximum of 3 octets — confirmed
  via source, `ble_mesh_gateway_cmd_handle.c`: opcode length is derived from the top bits of the
  first opcode byte, `0xC0` → 3 octets, `0x80` → 2 octets, else → 1 octet). The bytes shown in
  each command's Opcode table below are already in wire order (Byte 0 is transmitted first).
  **Implementation note:** the on-chip Config Model client headers (`cfg_mdl_opcode.h`) store
  opcode constants pre-swapped to this wire order, but the generic model client header
  (`mmdl_opcodes.h`, used by Section 7) stores constants in natural spec notation and must be
  passed through the `BE2LE16()` byte-swap macro before being placed on the wire — a real trap
  for anyone hand-encoding frames against the on-chip client source instead of this manual's
  tables.
- **Address** — 2 octets, unicast or group address.
- **App key index** — 2 octets.
- **Parameter** — variable length, defined per command below.
- **Checksum** — 1 byte: bitwise NOT of the sum of every byte from Length through the end of
  Parameter (header excluded): `checksum = ~(length + command_id[0..3] + sub_header[...] + parameter[0] + ... + parameter[n-1])`.
  Confirmed via source (`_gateway_checksum_calc()`, computed over `&hdr->len` through
  `hdr->len + 1` bytes).

## 4. Supplemental Parameter Requirements

### 4.1 Key Index Packing

All key indexes (Network Key Index and App Key Index) are 12 bits long. Two key indexes are
packed into three octets: the 8 LSbs of the first key index go into the first octet, its
remaining 4 MSbs go into the 4 LSbs of the second octet, the first 4 LSbs of the second key
index go into the 4 MSbs of the second octet, and the remaining 8 MSbs of the second key index
go into the third octet.

Where an odd number of key indexes need to be packed, all but the last are packed in sequences
of three octets as above, and the last key index alone is packed into two octets: 8 LSbs into
the first octet, remaining 4 MSbs into the 4 LSbs of the second octet, with the top 4 bits of the
second octet set to 0.

### 4.2 Publish Period Format

A 1-octet value: a 2-bit Step Resolution field + a 6-bit Number of Steps field.

| Step Resolution | Meaning |
|---|---|
| `0b00` | 100 milliseconds |
| `0b01` | 1 second |
| `0b10` | 10 seconds |
| `0b11` | 10 minutes |

| Number of Steps | Meaning |
|---|---|
| `0x00` | Publish Period disabled |
| `0x01`–`0x3F` | Number of steps |

`Publish Period = Step Resolution × Number of Steps` (e.g. Step Resolution `0b10` and Number of
Steps `0x31` → 490 seconds).

### 4.3 Transition Time Format

A 1-octet value: a 2-bit Step Resolution field (same encoding as Publish Period above) + a 6-bit
Number of Steps field.

| Number of Steps | Meaning |
|---|---|
| `0x00` | Immediate |
| `0x01`–`0x3E` | Number of steps |
| `0x3F` | Unknown — cannot be set to this value, but may be reported if the actual transition is longer than `0x3E` steps or undetermined |

## 5. Mesh Network Service

Command ID `0x1000-0000`. There is no Sub-Header (only a 1-octet Opcode).

Get/Set commands are sent by the host; the corresponding Status/indication messages are
generated by the gateway and are **not accepted as inbound host commands** — confirmed via
source (`prcss_mesh_nwk_cmd[]` dispatch table, `ble_mesh_gateway_cmd_handle.c`: opcodes `0x02`,
`0x03`, `0x05`, and the reserved range `0x06`–`0x0F` all route to a no-op stub when received
*from* the host).

### 5.1 Unprovisioned Device Scan Get — `0x00`

No parameters. Acknowledged; gets the unprovisioned-device scan state. Response: Unprovisioned
Device Scan Status.

### 5.2 Unprovisioned Device Scan Set — `0x01`

| Field | Size (octets) | Notes |
|---|---|---|
| ScanFlg | 1 | 1 = start scanning, 0 = stop scanning |

Acknowledged. Response: Unprovisioned Device Scan Status. Calls
`ble_mesh_find_unprov_device_start()` / `_stop()`.

### 5.3 Unprovisioned Device Scan Status — `0x02`

| Field | Size (octets) | Notes |
|---|---|---|
| ScanFlg | 1 | 1 = scanning, 0 = stopped |

Unacknowledged; gateway → host only.

### 5.4 Unprovisioned Device Indication — `0x03`

| Field | Size (octets) | Notes |
|---|---|---|
| DeviceUUID | 16 | UUID of the unprovisioned device |
| MAC | 6 | MAC address of the unprovisioned device |
| RSSI | 1 | RSSI of the unprovisioned device |

Unacknowledged; gateway → host only, one per discovered device, raised from
`TYPE_MESH_UNPROV_DEVICE_IDC`.

### 5.5 Device Provision Set — `0x04`

| Field | Size (octets) | Notes |
|---|---|---|
| PrimaryAddress | 2 | Address to assign to the device being provisioned |
| DeviceUUID | 16 | UUID of the unprovisioned device to select |

Acknowledged; provisions the selected device into the mesh network. Response: Device Provision
Status. Calls `ble_mesh_select_unprovisioned_device()`.

### 5.6 Device Provision Status — `0x05`

| Field | Size (octets) | Notes |
|---|---|---|
| Status | 1 | Provision status, §5.16.1 |
| DeviceUUID | 16 | UUID of the provisioned device |
| PrimaryAddress | 2 | (optional) Primary address of the provisioned device |
| ElementCount | 1 | (optional) Number of elements supported by the device |
| DeviceKey | 16 | (optional) Device key, used for Config Model security |

Unacknowledged; gateway → host only, raised from `TYPE_MESH_PROVISION_COMPLETE_IDC`. Only the
Status field is present if Status is not "Success" (`0x00`).

### 5.7 Mesh Network Key Get — `0x10`

No parameters. Acknowledged. Response: Mesh Network Key Status.

### 5.8 Mesh Network Key Set — `0x11`

| Field | Size (octets) | Notes |
|---|---|---|
| NetworkKey | 16 | New network key |

Acknowledged. Response: Mesh Network Key Status.

### 5.9 Mesh Network Key Status — `0x12`

| Field | Size (octets) | Notes |
|---|---|---|
| Status | 1 | §5.16.2 |
| NetworkKey | 16 | |

### 5.10 Mesh Application Key Get — `0x13`

| Field | Size (octets) | Notes |
|---|---|---|
| AppKeyIndex | 1 | |

Acknowledged. Response: Mesh Application Key Status.

### 5.11 Mesh Application Key Set — `0x14`

| Field | Size (octets) | Notes |
|---|---|---|
| AppKeyIndex | 1 | |
| AppKey | 16 | New application key |

Acknowledged. Response: Mesh Application Key Status.

### 5.12 Mesh Application Key Status — `0x15`

| Field | Size (octets) | Notes |
|---|---|---|
| Status | 1 | §5.16.2 |
| AppKeyIndex | 1 | |
| AppKey | 16 | |

### 5.13 Mesh Device Key Get — `0x16`

No parameters. Acknowledged. Response: Mesh Device Key Status.

### 5.14 Mesh Device Key Set — `0x17`

| Field | Size (octets) | Notes |
|---|---|---|
| DeviceKey | 16 | New device key of the BLE mesh gateway, used for Config Model |

Acknowledged. Response: Mesh Device Key Status.

### 5.15 Mesh Device Key Status — `0x18`

| Field | Size (octets) | Notes |
|---|---|---|
| Status | 1 | §5.16.2 |
| DeviceKey | 16 | |

### 5.16 Code Definitions

#### 5.16.1 Provision Status Code

| Status Code | Name | Description |
|---|---|---|
| `0x00` | Success | Provisioning was successful |
| `0x01` | Timeout | Provisioning transaction timed out |
| `0x02` | Provision Fail | Provisioning failed |
| `0x03` | Address Invalid | Primary address is invalid |
| `0x04`–`0xFF` | RFU | Reserved for Future Use |

#### 5.16.2 Mesh Network Status Code

| Value | Description |
|---|---|
| `0x00` | Success |
| `0x01` | Invalid Parameter |
| `0x02` | Invalid Role |
| `0x03` | Invalid State |
| `0x04` | Empty |
| `0x05`–`0xFF` | Reserved for Future Use |

## 6. Device Configuration Service

Command ID `0x1000-1000`. Sub-Header is Address (2 octets) + Opcode. This section implements the
standard Bluetooth Mesh Configuration Model and Health Model — confirmed against source
(`components/network/bluetooth/ble-mesh/lib-ble-mesh/profile/config-model/` and
`.../upper-layer/src/ht_main.c`): the gateway forwards any opcode from the host verbatim to the
mesh stack (no opcode allowlisting), and the server-side dispatch tables
(`cfg_mdl_data.c`) implement essentially the full standard Config Foundation Model opcode set, so
every message below is genuinely reachable end-to-end.

### 6.1 Network Management

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Config AppKey Add | `0x00` | NetKeyIndexAndAppKeyIndex:3, AppKey:16 | Indexes packed per §4.1; response: Config AppKey Status |
| Config AppKey Delete | `0x80 0x00` | NetKeyIndexAndAppKeyIndex:3 | Response: Config AppKey Status |
| Config AppKey Get | `0x80 0x01` | NetKeyIndex:2 | Response: Config AppKey List |
| Config AppKey List | `0x80 0x02` | Status:1, NetKeyIndex:2, AppKeyIndexes:variable | Response to Config AppKey Get |
| Config AppKey Status | `0x80 0x03` | Status:1, NetKeyIndexAndAppKeyIndex:3 | Status = Success also for redundant add/update/delete |
| Config AppKey Update | `0x01` | NetKeyIndexAndAppKeyIndex:3, AppKey:16 | Response: Config AppKey Status |
| Config Beacon Get | `0x80 0x09` | — | Response: Config Beacon Status |
| Config Beacon Set | `0x80 0x0A` | Beacon:1 | Response: Config Beacon Status |
| Config Beacon Status | `0x80 0x0B` | Beacon:1 | — |
| Config Composition Data Get | `0x80 0x08` | Page:1 | Response: Config Composition Data Status |
| Config Composition Data Status | `0x02` | Page:1, Data:variable | 1-octet opcode (maximizes payload); Page 0 = CID:2, PID:2, VID:2, CRPL:2, Features:2, Elements:variable |
| Config Model Publication Set | `0x03` | ElementAddress:2, PublishAddress:2, AppKeyIndex/CredentialFlag/RFU:2, PublishTTL:1, PublishPeriod:1 (§4.2), PublishRetransmitCount/IntervalSteps:1, ModelIdentifier:2/4 | 1-octet opcode; PublishAddress must not be a virtual address; response: Config Model Publication Status |
| Config Default TTL Get | `0x80 0x0C` | — | Response: Config Default TTL Status |
| Config Default TTL Set | `0x80 0x0D` | TTL:1 | Response: Config Default TTL Status |
| Config Default TTL Status | `0x80 0x0E` | TTL:1 | — |
| Config Friend Get | `0x80 0x0F` | — | Response: Config Friend Status |
| Config Friend Set | `0x80 0x10` | Friend:1 | Response: Config Friend Status |
| Config Friend Status | `0x80 0x11` | Friend:1 | — |
| Config GATT Proxy Get | `0x80 0x12` | — | Response: Config GATT Proxy Status |
| Config GATT Proxy Set | `0x80 0x13` | GATTProxy:1 | Response: Config GATT Proxy Status |
| Config GATT Proxy Status | `0x80 0x14` | GATTProxy:1 | — |
| Config Key Refresh Phase Get | `0x80 0x15` | NetKeyIndex:2 | Response: Config Key Refresh Phase Status |
| Config Key Refresh Phase Set | `0x80 0x16` | NetKeyIndex:2, Transition:1 | Only defined phase transitions allowed |
| Config Key Refresh Phase Status | `0x80 0x17` | Status:1, NetKeyIndex:2, Phase:1 | Status = Success also if the transition already occurred |
| Config Low Power Node PollTimeout Get | `0x80 0x2D` | LPNAddress:2 | Send only to a Friend node handling OBO for the LPN |
| Config Low Power Node PollTimeout Status | `0x80 0x2E` | LPNAddress:2, PollTimeout:3 | `0x000000` if not currently Friend for that LPN |
| Config Model App Bind | `0x80 0x3D` | ElementAddress:2, AppKeyIndex:2, ModelIdentifier:2/4 | Response: Config Model App Status |
| Config Model App Status | `0x80 0x3E` | Status:1, ElementAddress:2, AppKeyIndex:2, ModelIdentifier:2/4 | — |
| Config Model App Unbind | `0x80 0x3F` | ElementAddress:2, AppKeyIndex:2, ModelIdentifier:2/4 | Response: Config Model App Status |
| Config Model Publication Get | `0x80 0x18` | ElementAddress:2, ModelIdentifier:2/4 | Response: Config Model Publication Status |
| Config Model Publication Status | `0x80 0x19` | Status:1, ElementAddress:2, PublishAddress:2, AppKeyIndex/CredentialFlag/RFU:2, PublishTTL:1, PublishPeriod:1, PublishRetransmitCount/IntervalSteps:1, ModelIdentifier:2/4 | Same packing as Model Publication Set |
| Config Model Publication Virtual Address Set | `0x80 0x1A` | ElementAddress:2, PublishAddress:16(Label UUID), AppKeyIndex/CredentialFlag/RFU:2, PublishTTL:1, PublishPeriod:1, PublishRetransmitCount/IntervalSteps:1, ModelIdentifier:2/4 | Response: Config Model Publication Status |
| Config Model Subscription Add | `0x80 0x1B` | ElementAddress:2, Address:2, ModelIdentifier:2/4 | Address must not be unassigned/unicast/all-nodes/virtual |
| Config Model Subscription Delete | `0x80 0x1C` | ElementAddress:2, Address:2, ModelIdentifier:2/4 | Response: Config Model Subscription Status |
| Config Model Subscription Delete All | `0x80 0x1D` | ElementAddress:2, ModelIdentifier:2/4 | Discards the entire Subscription List |
| Config Model Subscription Overwrite | `0x80 0x1E` | ElementAddress:2, Address:2, ModelIdentifier:2/4 | Clears the list then adds Address |
| Config Model Subscription Status | `0x80 0x1F` | Status:1, ElementAddress:2, Address:2, ModelIdentifier:2/4 | — |
| Config Model Subscription Virtual Address Add | `0x80 0x20` | ElementAddress:2, Label:16, ModelIdentifier:2/4 | Response: Config Model Subscription Status |
| Config Model Subscription Virtual Address Delete | `0x80 0x21` | ElementAddress:2, Address:16(Label UUID), ModelIdentifier:2/4 | Response: Config Model Subscription Status |
| Config Model Subscription Virtual Address Overwrite | `0x80 0x22` | ElementAddress:2, Address:16(Label UUID), ModelIdentifier:2/4 | Response: Config Model Subscription Status |
| Config NetKey Add | `0x80 0x40` | NetKeyIndex:2, NetKey:16 | Response: Config NetKey Status |
| Config NetKey Delete | `0x80 0x41` | NetKeyIndex:2 | Response: Config NetKey Status |
| Config NetKey Get | `0x80 0x42` | — | Response: Config NetKey List |
| Config NetKey List | `0x80 0x43` | NetKeyIndexes:variable | — |
| Config NetKey Status | `0x80 0x44` | Status:1, NetKeyIndex:2 | Status = Success also for redundant add/update/delete |
| Config NetKey Update | `0x80 0x45` | NetKeyIndex:2, NetKey:16 | Response: Config NetKey Status |
| Config Network Transmit Get | `0x80 0x23` | — | Response: Config Network Transmit Status |
| Config Network Transmit Set | `0x80 0x24` | NetworkTransmitCount/IntervalSteps:1 | Response: Config Network Transmit Status |
| Config Network Transmit Status | `0x80 0x25` | NetworkTransmitCount/IntervalSteps:1 | — |
| Config Node Identity Get | `0x80 0x46` | NetKeyIndex:2 | — |
| Config Node Identity Set | `0x80 0x47` | NetKeyIndex:2, Identity:1 | Response: Config Node Identity Status |
| Config Node Identity Status | `0x80 0x48` | Status:1, NetKeyIndex:2, Identity:1 | — |
| Config Node Reset | `0x80 0x49` | — | Resets a non-Provisioner node and removes it from the network; response: Config Node Reset Status |
| Config Node Reset Status | `0x80 0x4A` | — | Acknowledges receipt of Config Node Reset |
| Config Relay Get | `0x80 0x26` | — | Response: Config Relay Status |
| Config Relay Set | `0x80 0x27` | Relay:1, RelayRetransmitCount/IntervalSteps:1 | Response: Config Relay Status |
| Config Relay Status | `0x80 0x28` | Relay:1, RelayRetransmitCount/IntervalSteps:1 | — |
| Config SIG Model App Get | `0x80 0x4B` | ElementAddress:2, ModelIdentifier:2 | SIG models only |
| Config SIG Model App List | `0x80 0x4C` | Status:1, ElementAddress:2, ModelIdentifier:2, AppKeyIndexes:variable | — |
| Config SIG Model Subscription Get | `0x80 0x29` | ElementAddress:2, ModelIdentifier:2 | SIG models only |
| Config SIG Model Subscription List | `0x80 0x2A` | Status:1, ElementAddress:2, ModelIdentifier:2, Addresses:variable | Empty list → zero-length Addresses |
| Config Vendor Model App Get | `0x80 0x4D` | ElementAddress:2, ModelIdentifier:4 | Vendor models only |
| Config Vendor Model App List | `0x80 0x4E` | Status:1, ElementAddress:2, ModelIdentifier:4, AppKeyIndexes:variable | — |
| Config Vendor Model Subscription Get | `0x80 0x2B` | ElementAddress:2, ModelIdentifier:4 | Vendor models only; response: Config Vendor Model Subscription List |
| Config Vendor Model Subscription List | `0x80 0x2C` | Status:1, ElementAddress:2, ModelIdentifier:4, Addresses:variable | Empty list → zero-length Addresses |

### 6.2 Heartbeat State Management

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Config Heartbeat Publication Get | `0x80 0x38` | — | Response: Config Heartbeat Publication Status |
| Config Heartbeat Publication Set | `0x80 0x39` | Destination:2, CountLog:1, PeriodLog:1, TTL:1, Features:2, NetKeyIndex:2 | Response: Config Heartbeat Publication Status |
| Config Heartbeat Publication Status | `0x06` | Status:1, Destination:2, CountLog:1, PeriodLog:1, TTL:1, Features:2, NetKeyIndex:2 | 1-octet opcode |
| Config Heartbeat Subscription Get | `0x80 0x3A` | — | Response: Config Heartbeat Subscription Status |
| Config Heartbeat Subscription Set | `0x80 0x3B` | Source:2, Destination:2, PeriodLog:1 | Response: Config Heartbeat Subscription Status |
| Config Heartbeat Subscription Status | `0x80 0x3C` | Status:1, Source:2, Destination:2, PeriodLog:1, CountLog:1, MinHops:1, MaxHops:1 | — |

### 6.3 Device Health Management

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Health Attention Get | `0x80 0x04` | — | Response: Health Attention Status |
| Health Attention Set | `0x80 0x05` | Attention:1 | Response: Health Attention Status |
| Health Attention Set Unacknowledged | `0x80 0x06` | Attention:1 | — |
| Health Attention Status | `0x80 0x07` | Attention:1 | — |
| Health Current Status | `0x04` | TestID:1, CompanyID:2, FaultArray:variable | Unsolicited; 1-octet opcode maximizes FaultArray size; no fault fields = no fault |
| Health Fault Clear | `0x80 0x2F` | CompanyID:2 | Response: Health Fault Status |
| Health Fault Clear Unacknowledged | `0x80 0x30` | CompanyID:2 | — |
| Health Fault Get | `0x80 0x31` | CompanyID:2 | Response: Health Fault Status |
| Health Fault Status | `0x05` | TestID:1, CompanyID:2, FaultArray:variable | 1-octet opcode; no fault fields = no registered fault |
| Health Fault Test | `0x80 0x32` | TestID:1, CompanyID:2 | Invokes an implementation-specific self-test; response: Health Fault Status |
| Health Fault Test Unacknowledged | `0x80 0x33` | TestID:1, CompanyID:2 | — |
| Health Period Get | `0x80 0x34` | — | Response: Health Period Status |
| Health Period Set | `0x80 0x35` | FastPeriodDivisor:1 | Divides the Publish Period while active faults exist |
| Health Period Set Unacknowledged | `0x80 0x36` | FastPeriodDivisor:1 | — |
| Health Period Status | `0x80 0x37` | FastPeriodDivisor:1 | — |

Health model server logic is implemented separately from the Config model
(`components/network/bluetooth/ble-mesh/lib-ble-mesh/profile/upper-layer/src/ht_main.c`) but
reuses the same Command ID `0x1000-1000` gateway framing.

### 6.4 Code Definitions

#### 6.4.1 Message Status Code

| Value | Name |
|---|---|
| `0x00` | Success |
| `0x01` | Invalid Address |
| `0x02` | Invalid Model |
| `0x03` | Invalid AppKey Index |
| `0x04` | Invalid NetKey Index |
| `0x05` | Insufficient Resources |
| `0x06` | Key Index Already Stored |
| `0x07` | Invalid Publish Parameters |
| `0x08` | Not a Subscribe Model |
| `0x09` | Storage Failure |
| `0x0A` | Feature Not Supported |
| `0x0B` | Cannot Update |
| `0x0C` | Cannot Remove |
| `0x0D` | Cannot Bind |
| `0x0E` | Temporarily Unable to Change State |
| `0x0F` | Cannot Set |
| `0x10` | Unspecified Error |
| `0x11` | Invalid Binding |
| `0x12`–`0xFF` | Reserved for Future Use |

#### 6.4.2 Health Test Fault Status Code

| Value | Name | Value | Name |
|---|---|---|---|
| `0x00` | No Fault | `0x1D` | Input Too High Warning |
| `0x01` | Battery Low Warning | `0x1E` | Input Too High Error |
| `0x02` | Battery Low Error | `0x1F` | Input No Change Warning |
| `0x03` | Supply Voltage Too Low Warning | `0x20` | Input No Change Error |
| `0x04` | Supply Voltage Too Low Error | `0x21` | Actuator Blocked Warning |
| `0x05` | Supply Voltage Too High Warning | `0x22` | Actuator Blocked Error |
| `0x06` | Supply Voltage Too High Error | `0x23` | Housing Opened Warning |
| `0x07` | Power Supply Interrupted Warning | `0x24` | Housing Opened Error |
| `0x08` | Power Supply Interrupted Error | `0x25` | Tamper Warning |
| `0x09` | No Load Warning | `0x26` | Tamper Error |
| `0x0A` | No Load Error | `0x27` | Device Moved Warning |
| `0x0B` | Overload Warning | `0x28` | Device Moved Error |
| `0x0C` | Overload Error | `0x29` | Device Dropped Warning |
| `0x0D` | Overheat Warning | `0x2A` | Device Dropped Error |
| `0x0E` | Overheat Error | `0x2B` | Overflow Warning |
| `0x0F` | Condensation Warning | `0x2C` | Overflow Error |
| `0x10` | Condensation Error | `0x2D` | Empty Warning |
| `0x11` | Vibration Warning | `0x2E` | Empty Error |
| `0x12` | Vibration Error | `0x2F` | Internal Bus Warning |
| `0x13` | Configuration Warning | `0x30` | Internal Bus Error |
| `0x14` | Configuration Error | `0x31` | Mechanism Jammed Warning |
| `0x15` | Element Not Calibrated Warning | `0x32` | Mechanism Jammed Error |
| `0x16` | Element Not Calibrated Error | `0x33`–`0x7F` | Reserved for Future Use |
| `0x17` | Memory Warning | `0x80`–`0xFF` | Vendor Specific Warning / Error |
| `0x18` | Memory Error | | |
| `0x19` | Self-Test Warning | | |
| `0x1A` | Self-Test Error | | |
| `0x1B` | Input Too Low Warning | | |
| `0x1C` | Input Too Low Error | | |

## 7. Device Application Service

Command ID `0x1000-2000`. Sub-Header is Address (2 octets) + App key index (2 octets) + Opcode.
This section covers the standard Bluetooth Mesh Generic, Sensor, Time, Scene, Scheduler, and
Lighting models. The gateway forwards these commands generically to whichever mesh node the
Address field identifies — the target device does not need to run this SDK, so every message
below is documented regardless of which models this SDK's own mesh stack implements locally.

### 7.1 Generic OnOff

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Generic OnOff Get | `0x82 0x01` | — | — |
| Generic OnOff Set | `0x82 0x02` | OnOff:1, TID:1, Transition Time:1(optional), Delay:1(optional) | §4.3 (Transition Time format). Transition Time and Delay must both be present or both absent |
| Generic OnOff Set Unacknowledged | `0x82 0x03` | OnOff:1, TID:1, Transition Time:1(optional), Delay:1(optional) | Same conditional rule as Set |
| Generic OnOff Status | `0x82 0x04` | Present OnOff:1, Target OnOff:1(optional), Remaining Time:1(optional) | Target OnOff and Remaining Time must both be present or both absent |

### 7.2 Generic Level

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Generic Level Get | `0x82 0x05` | — | — |
| Generic Level Set | `0x82 0x06` | Level:2, TID:1, Transition Time:1(optional), Delay:1(optional) | Transition Time and Delay must both be present or both absent |
| Generic Level Set Unacknowledged | `0x82 0x07` | Level:2, TID:1, Transition Time:1(optional), Delay:1(optional) | Same conditional rule as Set |
| Generic Level Status | `0x82 0x08` | Present Level:2, Target Level:2(optional), Remaining Time:1(optional) | Target Level and Remaining Time must both be present or both absent |
| Generic Delta Set | `0x82 0x09` | Delta Level:4, TID:1, Transition Time:1(optional), Delay:1(optional) | TID groups a cumulative sequence; response is Generic Level Status |
| Generic Delta Set Unacknowledged | `0x82 0x0A` | Delta Level:4, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Generic Move Set | `0x82 0x0B` | Delta Level:2, TID:1, Transition Time:1(optional), Delay:1(optional) | Delta Level is a 2-octet speed step (differs from Delta Set's 4-octet field); response is Generic Level Status |
| Generic Move Set Unacknowledged | `0x82 0x0C` | Delta Level:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |

### 7.3 Generic Default Transition Time

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Generic Default Transition Time Get | `0x82 0x0D` | — | — |
| Generic Default Transition Time Set | `0x82 0x0E` | Transition Time:1 | Only `0x00`–`0x3E` valid |
| Generic Default Transition Time Set Unacknowledged | `0x82 0x0F` | Transition Time:1 | — |
| Generic Default Transition Time Status | `0x82 0x10` | Transition Time:1 | — |

### 7.4 Generic OnPowerUp

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Generic OnPowerUp Get | `0x82 0x11` | — | — |
| Generic OnPowerUp Status | `0x82 0x12` | OnPowerUp:1 | — |
| Generic OnPowerUp Set | `0x82 0x13` | OnPowerUp:1 | — |
| Generic OnPowerUp Set Unacknowledged | `0x82 0x14` | OnPowerUp:1 | — |

### 7.5 Generic Power Level

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Generic Power Level Get | `0x82 0x15` | — | — |
| Generic Power Level Set | `0x82 0x16` | Power:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Generic Power Level Set Unacknowledged | `0x82 0x17` | Power:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Generic Power Level Status | `0x82 0x18` | Present Power:2, Target Power:2(optional), Remaining Time:1(optional) | — |
| Generic Power Last Get | `0x82 0x19` | — | — |
| Generic Power Last Status | `0x82 0x1A` | Power:2 | — |
| Generic Power Default Get | `0x82 0x1B` | — | — |
| Generic Power Default Status | `0x82 0x1C` | Power:2 | — |
| Generic Power Range Get | `0x82 0x1D` | — | — |
| Generic Power Range Status | `0x82 0x1E` | Status Code:1, Range Min:2, Range Max:2 | Status codes §7.21.1 |
| Generic Power Default Set | `0x82 0x1F` | Power:2 | — |
| Generic Power Default Set Unacknowledged | `0x82 0x20` | Power:2 | — |
| Generic Power Range Set | `0x82 0x21` | Range Min:2, Range Max:2 | Range Max ≥ Range Min |
| Generic Power Range Set Unacknowledged | `0x82 0x22` | Range Min:2, Range Max:2 | Range Max ≥ Range Min |

### 7.6 Generic Battery

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Generic Battery Get | `0x82 0x23` | — | — |
| Generic Battery Status | `0x82 0x24` | Battery Level:1, Time to Discharge:3, Time to Charge:3, Flags:1 | — |

### 7.7 Generic Location

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Generic Location Global Get | `0x82 0x25` | — | — |
| Generic Location Global Set | `0x41` | Global Latitude:4, Global Longitude:4, Global Altitude:2 | 1-octet opcode |
| Generic Location Global Set Unacknowledged | `0x42` | Global Latitude:4, Global Longitude:4, Global Altitude:2 | 1-octet opcode |
| Generic Location Global Status | `0x40` | Global Latitude:4, Global Longitude:4, Global Altitude:2 | 1-octet opcode |
| Generic Location Local Get | `0x82 0x26` | — | — |
| Generic Location Local Set | `0x82 0x27` | Local North:2, Local East:2, Local Altitude:2, Floor Number:1, Uncertainty:2 | — |
| Generic Location Local Set Unacknowledged | `0x82 0x29` | Local North:2, Local East:2, Local Altitude:2, Floor Number:1, Uncertainty:2 | — |
| Generic Location Local Status | `0x82 0x2A` | Local North:2, Local East:2, Local Altitude:2, Floor Number:1, Uncertainty:2 | — |

### 7.8 Generic Manufacturer Property

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Generic Manufacturer Properties Get | `0x82 0x2A` | — | — |
| Generic Manufacturer Properties Status | `0x43` | Manufacturer Property IDs:variable (2×N) | 1-octet opcode |
| Generic Manufacturer Property Get | `0x82 0x2B` | Manufacturer Property ID:2 | — |
| Generic Manufacturer Property Set | `0x44` | Manufacturer Property ID:2, Manufacturer User Access:1 | 1-octet opcode; no raw-value field — only User Access is client-writable |
| Generic Manufacturer Property Set Unacknowledged | `0x45` | Manufacturer Property ID:2, Manufacturer User Access:1 | 1-octet opcode |
| Generic Manufacturer Property Status | `0x46` | Manufacturer Property ID:2, Manufacturer User Access:1(optional), Manufacturer Property Value:variable(optional) | 1-octet opcode; if User Access present, Property Value must also be present |

### 7.9 Generic Admin Property

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Generic Admin Properties Get | `0x82 0x2C` | — | — |
| Generic Admin Properties Status | `0x47` | Admin Property IDs:variable (2×N) | 1-octet opcode |
| Generic Admin Property Get | `0x82 0x2D` | Admin Property ID:2 | — |
| Generic Admin Property Set | `0x49` | Admin Property ID:2, Admin User Access:1, Admin Property Value:variable | 1-octet opcode |
| Generic Admin Property Set Unacknowledged | `0x4A` | Admin Property ID:2, Admin User Access:1, Admin Property Value:variable | 1-octet opcode |
| Generic Admin Property Status | `0x48` | Admin Property ID:2, Admin User Access:1(optional), Admin Property Value:variable(optional) | 1-octet opcode |

### 7.10 Generic User Property

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Generic User Properties Get | `0x82 0x2E` | — | — |
| Generic User Properties Status | `0x4B` | User Property IDs:variable (2×N) | 1-octet opcode |
| Generic User Property Get | `0x82 0x2F` | User Property ID:2 | — |
| Generic User Property Set | `0x4C` | User Property ID:2, User Property Value:variable | 1-octet opcode |
| Generic User Property Set Unacknowledged | `0x4D` | User Property ID:2, User Property Value:variable | 1-octet opcode |
| Generic User Property Status | `0x4E` | User Property ID:2, User Access:1(optional), User Property Value:variable(optional) | 1-octet opcode; if User Access present, Property Value must also be present |

### 7.11 Generic Client Property

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Generic Client Properties Get | `0x4F` | Client Property ID:2 | 1-octet opcode; ID is the smallest/starting Property ID requested (pagination) |
| Generic Client Properties Status | `0x50` | Client Property IDs:variable (2×N) | 1-octet opcode |

### 7.12 Sensor

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Sensor Descriptor Get | `0x82 0x30` | Property ID:2(optional) | — |
| Sensor Descriptor Status | `0x51` | Descriptor:variable (8×N or 2) | 1-octet opcode; 2-octet form signals "no descriptor" |
| Sensor Get | `0x82 0x31` | Property ID:2(optional) | — |
| Sensor Status | `0x52` | Marshalled Sensor Data:variable | 1-octet opcode |
| Sensor Column Get | `0x82 0x32` | Property ID:2, Raw Value X:variable | — |
| Sensor Column Status | `0x53` | Property ID:2, Raw Value X:variable, Column Width:variable(optional), Raw Value Y:variable(optional) | 1-octet opcode; if Column Width present, Raw Value Y must also be present |
| Sensor Series Get | `0x82 0x33` | Property ID:2, Raw Value X1:variable(optional), Raw Value X2:variable(optional) | If X1 present, X2 must also be present |
| Sensor Series Status | `0x54` | Property ID:2, repeating (Raw Value X, Column Width, Raw Value Y):variable(optional) | 1-octet opcode; repeating triplet, all-or-none per triplet |
| Sensor Cadence Get | `0x82 0x34` | Property ID:2 | — |
| Sensor Cadence Set | `0x55` | Property ID:16b, Fast Cadence Period Divisor:7b, Status Trigger Type:1b, Status Trigger Delta Down:variable, Status Trigger Delta Up:variable, Status Min Interval:8b, Fast Cadence Low:variable, Fast Cadence High:variable | 1-octet opcode; bit-packed message; Fast Cadence Period Divisor relates to Publish Period §4.2 |
| Sensor Cadence Set Unacknowledged | `0x56` | Same fields as Cadence Set | 1-octet opcode |
| Sensor Cadence Status | `0x57` | Same fields as Cadence Set, all optional | 1-octet opcode; if Fast Cadence Period Divisor present, all remaining fields must also be present |
| Sensor Settings Get | `0x82 0x35` | Sensor Property ID:2 | — |
| Sensor Settings Status | `0x58` | Sensor Property ID:2, Sensor Setting Property IDs:variable (2×N, optional) | 1-octet opcode |
| Sensor Setting Get | `0x82 0x36` | Sensor Property ID:2, Sensor Setting Property ID:2 | — |
| Sensor Setting Set | `0x59` | Sensor Property ID:2, Sensor Setting Property ID:2, Sensor Setting Raw:variable | 1-octet opcode |
| Sensor Setting Set Unacknowledged | `0x5A` | Sensor Property ID:2, Sensor Setting Property ID:2, Sensor Setting Raw:variable | 1-octet opcode |
| Sensor Setting Status | `0x5B` | Sensor Property ID:2, Sensor Setting Property ID:2, Sensor Setting Access:1(optional), Sensor Setting Raw:variable(optional) | 1-octet opcode; if Access present, Raw must also be present |

### 7.13 Time

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Time Get | `0x82 0x37` | — | — |
| Time Set | `0x5C` | TAI Seconds:40b, Subsecond:8b, Uncertainty:8b, Time Authority:1b, TAI-UTC Delta:15b, Time Zone Offset:8b | 1-octet opcode; bit-packed, 10 octets total |
| Time Status | `0x5D` | Same fields as Time Set, all optional | 1-octet opcode; if TAI Seconds = 0, all other fields are omitted; otherwise all present |
| Time Role Get | `0x82 0x38` | — | — |
| Time Role Set | `0x82 0x39` | Time Role:1 | — |
| Time Role Status | `0x82 0x3A` | Time Role:1 | — |
| Time Zone Get | `0x82 0x3B` | — | — |
| Time Zone Set | `0x82 0x3C` | Time Zone Offset New:1, TAI of Zone Change:5 | — |
| Time Zone Status | `0x82 0x3D` | Time Zone Offset Current:1, Time Zone Offset New:1, TAI of Zone Change:5 | — |
| TAI-UTC Delta Get | `0x82 0x3E` | — | — |
| TAI-UTC Delta Set | `0x82 0x3F` | TAI-UTC Delta New:15b, Padding:1b, TAI of Delta Change:40b | Bit-packed, 7 octets total |
| TAI-UTC Delta Status | `0x82 0x40` | TAI-UTC Delta Current:15b, Padding:1b, TAI-UTC Delta New:15b, Padding:1b, TAI of Delta Change:40b | Bit-packed, 9 octets total; distinct 2-octet opcode, not to be confused with Generic Location Global Status's 1-octet `0x40` |

### 7.14 Scene

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Scene Get | `0x82 0x41` | — | — |
| Scene Recall | `0x82 0x42` | Scene Number:2, TID:1, Transition Time:1(optional), Delay:1(optional) | Scene Number `0x0000` is Prohibited; response is Scene Status |
| Scene Recall Unacknowledged | `0x82 0x43` | Scene Number:2, TID:1, Transition Time:1(optional), Delay:1(optional) | Same rules as Recall |
| Scene Status | `0x5E` | Status Code:1, Current Scene:2, Target Scene:2(optional), Remaining Time:1(optional) | 1-octet opcode; status codes §7.21.2; If Target Scene present, Remaining Time must also be present |
| Scene Register Get | `0x82 0x44` | — | — |
| Scene Register Status | `0x82 0x45` | Status Code:1, Current Scene:2, Scenes:variable | Status codes §7.21.2 |
| Scene Store | `0x82 0x46` | Scene Number:2 | Scene Number `0x0000` Prohibited; response is Scene Register Status |
| Scene Store Unacknowledged | `0x82 0x47` | Scene Number:2 | — |
| Scene Delete | `0x82 0x9E` | Scene Number:2 | Response is Scene Register Status |
| Scene Delete Unacknowledged | `0x82 0x9F` | Scene Number:2 | — |

### 7.15 Scheduler

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Scheduler Action Get | `0x82 0x48` | Index:1 | Valid range `0x00`–`0x0F`; `0x10`–`0xFF` Prohibited |
| Scheduler Action Status | `0x5F` | Index:4b, Schedule Register:76b | 1-octet opcode; bit-packed, 10 octets total |
| Scheduler Get | `0x82 0x49` | — | — |
| Scheduler Status | `0x82 0x4A` | Schedules:2 | Each bit identifies a Schedule Register entry (16 total) |
| Scheduler Action Set | `0x60` | Index:4b, Schedule Register:76b | 1-octet opcode |
| Scheduler Action Set Unacknowledged | `0x61` | Index:4b, Schedule Register:76b | 1-octet opcode |

### 7.16 Light Lightness

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Light Lightness Get | `0x82 0x4B` | — | — |
| Light Lightness Set | `0x82 0x4C` | Lightness:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light Lightness Set Unacknowledged | `0x82 0x4D` | Lightness:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light Lightness Status | `0x82 0x4E` | Present Lightness:2, Target Lightness:2(optional), Remaining Time:1(optional) | — |
| Light Lightness Linear Get | `0x82 0x4F` | — | — |
| Light Lightness Linear Set | `0x82 0x50` | Lightness:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light Lightness Linear Set Unacknowledged | `0x82 0x51` | Lightness:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light Lightness Linear Status | `0x82 0x52` | Present Lightness:2, Target Lightness:2(optional), Remaining Time:1(optional) | — |
| Light Lightness Last Get | `0x82 0x53` | — | — |
| Light Lightness Last Status | `0x82 0x54` | Lightness:2 | — |
| Light Lightness Default Get | `0x82 0x55` | — | — |
| Light Lightness Default Status | `0x82 0x56` | Lightness:2 | — |
| Light Lightness Range Get | `0x82 0x57` | — | — |
| Light Lightness Range Status | `0x82 0x58` | Status Code:1, Range Min:2, Range Max:2 | Status codes §7.21.1 |
| Light Lightness Default Set | `0x82 0x59` | Lightness:2 | — |
| Light Lightness Default Set Unacknowledged | `0x82 0x5A` | Lightness:2 | — |
| Light Lightness Range Set | `0x82 0x5B` | Range Min:2, Range Max:2 | Range Max ≥ Range Min |
| Light Lightness Range Set Unacknowledged | `0x82 0x5C` | Range Min:2, Range Max:2 | Range Max ≥ Range Min |

### 7.17 Light CTL

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Light CTL Get | `0x82 0x5D` | — | — |
| Light CTL Set | `0x82 0x5E` | CTL Lightness:2, CTL Temperature:2, CTL Delta UV:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light CTL Set Unacknowledged | `0x82 0x5F` | CTL Lightness:2, CTL Temperature:2, CTL Delta UV:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light CTL Status | `0x82 0x60` | Present CTL Lightness:2, Present CTL Temperature:2, Target CTL Lightness:2(optional), Target CTL Temperature:2(optional), Remaining Time:1(optional) | If Target CTL Lightness present, Target CTL Temperature and Remaining Time must also be present |
| Light CTL Temperature Get | `0x82 0x61` | — | — |
| Light CTL Temperature Range Get | `0x82 0x62` | — | — |
| Light CTL Temperature Range Status | `0x82 0x63` | Status Code:1, Range Min:2, Range Max:2 | Status codes §7.21.1 |
| Light CTL Temperature Set | `0x82 0x64` | CTL Temperature:2, CTL Delta UV:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light CTL Temperature Set Unacknowledged | `0x82 0x65` | CTL Temperature:2, CTL Delta UV:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light CTL Temperature Status | `0x82 0x66` | Present CTL Temperature:2, Present CTL Delta UV:2, Target CTL Temperature:2(optional), Target CTL Delta UV:2(optional), Remaining Time:1(optional) | If Target CTL Temperature present, Target CTL Delta UV and Remaining Time must also be present |
| Light CTL Default Get | `0x82 0x67` | — | — |
| Light CTL Default Status | `0x82 0x68` | Lightness:2, Temperature:2, Delta UV:2 | — |
| Light CTL Default Set | `0x82 0x69` | Lightness:2, Temperature:2, Delta UV:2 | — |
| Light CTL Default Set Unacknowledged | `0x82 0x6A` | Lightness:2, Temperature:2, Delta UV:2 | — |
| Light CTL Temperature Range Set | `0x82 0x6B` | Range Min:2, Range Max:2 | Range Max ≥ Range Min |
| Light CTL Temperature Range Set Unacknowledged | `0x82 0x6C` | Range Min:2, Range Max:2 | Range Max ≥ Range Min |

### 7.18 Light HSL

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Light HSL Get | `0x82 0x6D` | — | — |
| Light HSL Hue Get | `0x82 0x6E` | — | — |
| Light HSL Hue Set | `0x82 0x6F` | Hue:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light HSL Hue Set Unacknowledged | `0x82 0x70` | Hue:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light HSL Hue Status | `0x82 0x71` | Present Hue:2, Target Hue:2(optional), Remaining Time:1(optional) | — |
| Light HSL Saturation Get | `0x82 0x72` | — | — |
| Light HSL Saturation Set | `0x82 0x73` | Saturation:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light HSL Saturation Set Unacknowledged | `0x82 0x74` | Saturation:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light HSL Saturation Status | `0x82 0x75` | Present Saturation:2, Target Saturation:2(optional), Remaining Time:1(optional) | — |
| Light HSL Set | `0x82 0x76` | HSL Lightness:2, HSL Hue:2, HSL Saturation:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light HSL Set Unacknowledged | `0x82 0x77` | HSL Lightness:2, HSL Hue:2, HSL Saturation:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light HSL Status | `0x82 0x78` | HSL Lightness:2, HSL Hue:2, HSL Saturation:2, Remaining Time:1(optional) | — |
| Light HSL Target Get | `0x82 0x79` | — | Poll target state while a Status message reports positive Remaining Time |
| Light HSL Target Status | `0x82 0x7A` | HSL Lightness Target:2, HSL Hue Target:2, HSL Saturation Target:2, Remaining Time:1(optional) | — |
| Light HSL Default Get | `0x82 0x7B` | — | — |
| Light HSL Default Status | `0x82 0x7C` | Lightness:2, Hue:2, Saturation:2 | — |
| Light HSL Range Get | `0x82 0x7D` | — | — |
| Light HSL Range Status | `0x82 0x7E` | Status Code:1, Hue Range Min:2, Hue Range Max:2, Saturation Range Min:2, Saturation Range Max:2 | Status codes §7.21.1 |
| Light HSL Default Set | `0x82 0x7F` | Lightness:2, Hue:2, Saturation:2 | — |
| Light HSL Default Set Unacknowledged | `0x82 0x80` | Lightness:2, Hue:2, Saturation:2 | — |
| Light HSL Range Set | `0x82 0x81` | Hue Range Min:2, Hue Range Max:2, Saturation Range Min:2, Saturation Range Max:2 | Both range pairs: Max ≥ Min |
| Light HSL Range Set Unacknowledged | `0x82 0x82` | Hue Range Min:2, Hue Range Max:2, Saturation Range Min:2, Saturation Range Max:2 | Both range pairs: Max ≥ Min |

### 7.19 Light xyL

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Light xyL Get | `0x82 0x83` | — | — |
| Light xyL Set | `0x82 0x84` | xyL Lightness:2, xyL x:2, xyL y:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light xyL Set Unacknowledged | `0x82 0x85` | xyL Lightness:2, xyL x:2, xyL y:2, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light xyL Status | `0x82 0x86` | xyL Lightness:2, xyL x:2, xyL y:2, Remaining Time:1(optional) | — |
| Light xyL Target Get | `0x82 0x87` | — | Poll target state while a Status message reports positive Remaining Time |
| Light xyL Target Status | `0x82 0x88` | Target xyL Lightness:2, Target xyL x:2, Target xyL y:2, Remaining Time:1(optional) | — |
| Light xyL Default Get | `0x82 0x89` | — | — |
| Light xyL Default Status | `0x82 0x8A` | Lightness:2, xyL x:2, xyL y:2 | — |
| Light xyL Range Get | `0x82 0x8B` | — | — |
| Light xyL Range Status | `0x82 0x8C` | Status Code:1, xyL x Range Min:2, xyL x Range Max:2, xyL y Range Min:2, xyL y Range Max:2 | Status codes §7.21.1 |
| Light xyL Default Set | `0x82 0x8D` | Lightness:2, xyL x:2, xyL y:2 | — |
| Light xyL Default Set Unacknowledged | `0x82 0x8E` | Lightness:2, xyL x:2, xyL y:2 | — |
| Light xyL Range Set | `0x82 0x8F` | xyL x Range Min:2, xyL x Range Max:2, xyL y Range Min:2, xyL y Range Max:2 | Both range pairs: Max ≥ Min |
| Light xyL Range Set Unacknowledged | `0x82 0x90` | xyL x Range Min:2, xyL x Range Max:2, xyL y Range Min:2, xyL y Range Max:2 | Both range pairs: Max ≥ Min |

### 7.20 Light LC

| Message | Opcode | Parameters | Notes |
|---|---|---|---|
| Light LC Mode Get | `0x82 0x91` | — | — |
| Light LC Mode Set | `0x82 0x92` | Mode:1 | — |
| Light LC Mode Set Unacknowledged | `0x82 0x93` | Mode:1 | — |
| Light LC Mode Status | `0x82 0x94` | Mode:1 | — |
| Light LC OM Get | `0x82 0x95` | — | — |
| Light LC OM Set | `0x82 0x96` | Mode:1 | — |
| Light LC OM Set Unacknowledged | `0x82 0x97` | Mode:1 | — |
| Light LC OM Status | `0x82 0x98` | Mode:1 | — |
| Light LC Light OnOff Get | `0x82 0x99` | — | — |
| Light LC Light OnOff Set | `0x82 0x9A` | Light OnOff:1, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light LC Light OnOff Set Unacknowledged | `0x82 0x9B` | Light OnOff:1, TID:1, Transition Time:1(optional), Delay:1(optional) | — |
| Light LC Light OnOff Status | `0x82 0x9C` | Present Light OnOff:1, Target Light OnOff:1(optional), Remaining Time:1(optional) | — |
| Light LC Property Get | `0x82 0x9D` | Light LC Property ID:2 | — |
| Light LC Property Set | `0x62` | Light LC Property ID:2, Light LC Property Value:variable | 1-octet opcode |
| Light LC Property Set Unacknowledged | `0x63` | Light LC Property ID:2, Light LC Property Value:variable | 1-octet opcode |
| Light LC Property Status | `0x64` | Light LC Property ID:2, Light LC Property Value:variable | 1-octet opcode |

### 7.21 Application Model Status

#### 7.21.1 Status Code

| Status Code | Name | Description |
|---|---|---|
| `0x00` | Success | Command successfully processed |
| `0x01` | Cannot Set Range Min | The provided value for Range Min cannot be set |
| `0x02` | Cannot Set Range Max | The provided value for Range Max cannot be set |
| `0x03`–`0xFF` | RFU | Reserved for Future Use |

#### 7.21.2 Scene Status Code

| Status Code | Description |
|---|---|
| `0x00` | Success |
| `0x01` | Scene Register Full |
| `0x02` | Scene Not Found |
| `0x03`–`0xFF` | Reserved for Future Use |

## 8. Commands for Provision and Control Device

This section is a pure diagram in the source PDF (no extractable text). It has been transcribed
below and cross-checked against the actual gateway/CLI implementation
(`examples/bluetooth-le/mesh/gateway/gateway/src/main.c`,
`examples/bluetooth-le/mesh/gateway/cli/src/cli_cmd_gateway.c`) for the underlying function calls.

Actors: **UART tool** (host), **gateway**, **device** (the mesh node being provisioned).

### 8.1 Configure the RT58x gateway

1. UART tool → gateway: Mesh Application Key Set (§5.11), App key index `0`, App key
   `ffeeddccbbaa998877665544332211ff`.

### 8.2 Scan for unprovisioned devices

1. Device broadcasts a Mesh Provisioning ADV containing its device UUID (the device itself
   begins mesh provisioning).
2. UART tool → gateway: Unprovisioned Device Scan Set (§5.2), scan flag `1`.
3. For each Provisioning ADV the gateway receives: gateway → UART tool: Unprovisioned Device
   Indication (§5.4), device UUID.
4. UART tool → gateway: Unprovisioned Device Scan Set (§5.2), scan flag `0`, to stop scanning
   once the target device has been identified.

### 8.3 Provision the device

1. UART tool → gateway: Device Provision Set (§5.5), device UUID, element address `0x0100`.
   The gateway calls `ble_mesh_select_unprovisioned_device()` to start the provisioning
   transaction with the device.
2. Once provisioning completes: gateway → UART tool: Device Provision Status (§5.6), device
   UUID, element address `0x0100`, element count `1`, device key
   `0102030405060708090a0b0c0d0e0f`. Raised from `TYPE_MESH_PROVISION_COMPLETE_IDC`.

### 8.4 Configure the device

1. UART tool → gateway: Mesh Device Key Set (§5.14), device key
   `0102030405060708090a0b0c0d0e0f` — loads the newly-provisioned device's device key so the
   gateway can secure Config Model traffic to it.
2. gateway → UART tool: Mesh Device Key Status (§5.15).
3. UART tool → gateway: Config Composition Data Get (§6.1) → device: Config model message
   exchange → gateway → UART tool: Config Composition Data Status (§6.1), device element &
   model information.
4. UART tool → gateway: Config AppKey Add (§6.1), NetKeyIndexAndAppKeyIndex `0`, App key
   `ffeeddccbbaa998877665544332211ff` → device: Config model message exchange → gateway → UART
   tool: Config AppKey Status (§6.1), status `0`, NetKeyIndexAndAppKeyIndex `0`.
5. UART tool → gateway: Config Model App Bind (§6.1), element address `0x0100`, app key
   index `0`, model ID `0x1000` (Generic OnOff model) → device: Config model message exchange →
   gateway → UART tool: Config Model App Status (§6.1), status `0`, element address `0x0100`,
   app key index `0`, model ID `0x1000` (Generic OnOff model).

### 8.5 Control the device

1. UART tool → gateway: Generic OnOff Set (§7.1), OnOff `1`, TID `1` → device: Generic OnOff
   model message exchange → gateway → UART tool: Generic OnOff Status (§7.1), present OnOff
   `1`.
2. UART tool → gateway: Generic OnOff Set (§7.1), OnOff `0`, TID `2` → device: Generic OnOff
   model message exchange → gateway → UART tool: Generic OnOff Status (§7.1), present OnOff
   `0`.
