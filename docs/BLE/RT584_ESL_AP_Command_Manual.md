# RT584 ESL AP Command Manual

## Introduction

This document specifies the binary UART command protocol used to control an ESL (Electronic
Shelf Label) Access Point module. It covers BLE network configuration commands and ESL
application commands, plus the events/responses the module sends back.

This is the wire protocol implemented by
[`central-esl-ap`](../../examples/bluetooth-le/central/central-esl-ap/) — see
`ble_esl_ap_cmd_handle.c` for the command dispatch table and `ble_esl_ap_cmd.h` for every
parameter/event struct. The ESL Tool GUI and the demo's CLI (see
`RT58x_BLE_SDK_Application_Guide.md`, section 3.3) are two different front ends to this same
protocol.

## Hardware Interface Setup

The ESL AP module is connected to the host control unit over UART. Default settings: 115200
baud, 8-bit data, no parity, 1 stop bit.

## Command Data Format

Commands and events use the same little-endian frame:

| Header | Length | Command ID / Event code | Parameter | Checksum |
|---|---|---|---|---|
| 4 octets | 2 octets | 1 octet | n octets | 1 octet |

- **Header** — 4 bytes: `0xFF 0xFC 0xFC 0xFF` for BLE-control/application-control commands,
  `0xFF 0xFD 0xFD 0xFF` for events/responses.
- **Length** — 2 octets (`uint16_t`). The sum of the Command ID/Event code field and the
  Parameter field's lengths (i.e. everything between Length and Checksum).
- **Command ID / Event code** — 1 byte: the command ID for a command frame, or the event code
  for an event/response frame.
- **Parameter** — variable length, defined per command/event below.
- **Checksum** — 1 byte: bitwise NOT of the sum of every byte from Length through the end of
  Parameter (header excluded): `checksum = ~(length_lo + length_hi + command_id + parameter[0] + ... + parameter[n-1])`.

## 1. BLE Control Commands

### 1.1 Scan control — `0x01`

| Field | Size (octets) | Notes |
|---|---|---|
| Interval | 2 | Scan interval |
| Window | 2 | Scan window |
| Control | 1 | 1 = start scan, 0 = stop scan |

Scan interval/window unit is 0.625ms; range **0x0004–0x4000** (2.5ms–10.24s) for both.

**Event(s):** a Command Complete event follows; if successful, discovered advertisers are
reported via the Advertising Report event.

### 1.2 Create connection — `0x02`

| Field | Size (octets) | Notes |
|---|---|---|
| Scan interval | 2 | |
| Scan window | 2 | |
| Connection interval | 2 | |
| Connection timeout | 2 | |
| Address type | 1 | Peer address type |
| Address | 6 | Peer address |

Connection interval unit is 1.25ms, range **0x0006–0x0C80** (7.5ms–4s). Connection timeout unit
is 10ms, range **0x000A–0x0C80** (100ms–32s).

**Event(s):** Command Complete, then Connection Complete once established.

### 1.3 Enhance create connection — `0x03`

Same fields as Create Connection, plus:

| Field | Size (octets) | Notes |
|---|---|---|
| Group ID | 1 | The group of the peer address |

Used to connect to a specific tag over PAwR rather than through normal scannable advertising
(see the Application Guide's central-esl-ap "enhanced create connection" walkthrough).

**Event(s):** Command Complete, then Connection Complete once established.

### 1.4 Connection cancel — `0x04`

No parameters. Cancels an in-progress create-connection.

**Event(s):** Command Complete.

### 1.5 Disconnect — `0x05`

No parameters. Disconnects the current connection.

**Event(s):** Command Complete, then Disconnection Complete once torn down.

### 1.6 Sync configuration — `0x07`

| Field | Size (octets) | Notes |
|---|---|---|
| Periodic interval | 2 | Periodic advertising interval |
| Number of subevents | 1 | Total subevent count |
| Subevent interval | 1 | Interval between subevents |
| Response slot delay | 1 | Delay before the first response slot |
| Slot spacing | 1 | Spacing between response slots |
| Number of response slots | 1 | Total response slot count |

Configures PAwR parameters. Periodic interval unit is 1.25ms, range **0x0006–0xFFFF**
(7.5ms–81.91875s). Number of subevents: **0x00–0x80**. Slot spacing unit is 0.125ms, range
**0x02–0xFF** (0.25ms–31.875ms). Number of response slots: **1–0xFF**.

**Event(s):** Command Complete.

### 1.7 Sync control — `0x08`

| Field | Size (octets) | Notes |
|---|---|---|
| Control | 1 | Enable/disable PAwR |

**Event(s):** Command Complete.

### 1.8 Set periodic advertising information transfer — `0x09`

| Field | Size (octets) | Notes |
|---|---|---|
| Skip | 2 | Number of periodic ADV packets that may be missed since the last successful receive |
| Sync timeout | 2 | Synchronization timeout for PAwR |
| Service data | 2 | Application-defined value |

Skip range: **0–0x01F3**. Sync timeout unit is 10ms, range **0x000A–0x4000** (100ms–163.84s).

**Event(s):** Command Complete.

## 2. Application Control Commands

### 2.1 ESL AP configuration — `0x20`

| Field | Size (octets) | Notes |
|---|---|---|
| AP sync key | 16 | Input to the encrypted message the AP sends to ESL tags |
| AP sync nonce | 8 | Input to the encrypted message the AP sends to ESL tags |
| ESL response key | 16 | Input used by ESL tags to send encrypted messages to the AP |
| ESL response nonce | 8 | Input used by ESL tags to send encrypted messages to the AP |

**Event(s):** Command Complete.

### 2.2 ESL tag configuration — `0x21`

| Field | Size (octets) | Notes |
|---|---|---|
| Group ID | 1 | The tag's group |
| ESL ID | 1 | Unique ID within the group |

An ESL address is the 7-bit Group ID + 8-bit ESL ID pair.

**Event(s):** Command Complete.

### 2.3 ESL image information — `0x22`

| Field | Size (octets) | Notes |
|---|---|---|
| Image index | 1 | Unique index for the image |
| Part | 1 | Part of the image (e.g. black or red plane, for multi-part images sharing one index) |
| Image length | 2 | Length of the image data that follows |

**Event(s):** Command Complete; if successful, an ESL Image Information Response follows.

### 2.4 ESL image transfer — `0x23`

| Field | Size (octets) | Notes |
|---|---|---|
| Image data | n | Image data, max 240 bytes per command |

The first two bytes of the first image-data chunk must contain the total image length.

**Event(s):** Command Complete; if successful, an ESL Image Transfer Complete event follows.

### 2.5–2.15 ESL device commands (Ping, Unassociate, Service Reset, Factory Reset, Update
Complete, Read Sensor Data, Refresh Display, Display Image, Display Timed Image, LED Control,
LED Timed Control)

These 11 commands all share one envelope:

| Field | Size (octets) | Notes |
|---|---|---|
| Over encrypted ADV | 1 | 1 = send via encrypted periodic advertising, 0 = send via the BLE ESL GATT service |
| Group ID | 1 | Target group |
| Entries[n] | see below | One fixed-size entry per targeted ESL, back-to-back (not separate parallel arrays) |

Each entry starts with a 1-byte opcode (fixed per command, matching the ESL Control Point opcode
used over both transports) and the target's 1-byte ESL ID, followed by any command-specific
fields:

| Command | Command ID | Opcode | Entry layout (per targeted ESL) | Entry size |
|---|---|---|---|---|
| ESL ping | `0x24` | `0x00` | opcode, esl_id | 2 |
| Unassociated from AP | `0x25` | `0x01` | opcode, esl_id | 2 |
| Service reset | `0x26` | `0x02` | opcode, esl_id | 2 |
| Factory reset | `0x27` | `0x03` | opcode, esl_id | 2 |
| Update complete | `0x28` | `0x04` | opcode, esl_id | 2 |
| Read sensor data | `0x29` | `0x10` | opcode, esl_id, sensor_index | 3 |
| Refresh display | `0x2A` | `0x11` | opcode, esl_id, display_index | 3 |
| Display image | `0x2B` | `0x20` | opcode, esl_id, display_index, image_index | 4 |
| Display timed image | `0x2C` | `0x60` | opcode, esl_id, display_index, image_index, absolute_time (4 bytes) | 8 |
| LED control | `0x2D` | `0xB0` | opcode, esl_id, led_index, color (1 byte), pattern (7 bytes), repeat (2 bytes) | 13 |
| LED timed control | `0x2E` | `0xF0` | opcode, esl_id, led_index, color (1), pattern (7), repeat (2), absolute_time (4) | 17 |

**Color** byte (bitfields, LSB first): `color_red`:2, `color_green`:2, `color_blue`:2,
`brightness`:2 (0=25%, 1=50%, 2=75%, 3=100%).

**Pattern** (7 bytes): the LED's flashing pattern.

**Repeat** (2 bytes, bitfields): `repeat_type`:1, `repeat_duration`:15. If `repeat_type` is 0,
`repeat_duration` is an integer repeat count of the pattern; otherwise it's a duration in
seconds.

**Absolute time** (4 bytes, timed commands only): when the display/LED state change should take
effect.

**Event(s):** every command above gets a Command Complete event; on success:
- Ping / Unassociate / Service Reset → Basic State Response
- Read Sensor Data → Sensor Value Response
- Refresh Display / Display Image / Display Timed Image → Display State Response
- LED Control / LED Timed Control → LED State Response
- Factory Reset / Update Complete → Command Complete only (no follow-up response documented)

### 2.16 OTA image — `0x2F`

| Field | Size (octets) | Notes |
|---|---|---|
| Address | 4 | Flash address where the image data is stored |
| Image data | n | Upgrade image data |

**Event(s):** Command Complete.

### 2.17 OTA control — `0x30`

| Field | Size (octets) | Notes |
|---|---|---|
| OTA command | 1 | See table below |
| Command data | n | Payload, depends on OTA command |

| OTA command | Value | Note |
|---|---|---|
| Query | `0x00` | Query current system information; no command data |
| Start | `0x01` | Start FW OTA upgrade |
| Erase | `0x02` | Erase legacy FW; no command data |
| Apply | `0x03` | Apply new FW |

**Start** command data:

| Field | Size (octets) | Note |
|---|---|---|
| FW length | 4 | Length of the new FW |
| FW CRC | 4 | CRC of the new FW |
| Notify interval | 4 | Interval for progress notifications |
| FW information | 1 | 1 = compressed, 2 = needs signature check, 3 = both |
| Signature length | 3 | FW length used for signature calculation when compressed |

**Apply** command data:

| Field | Size (octets) | Note |
|---|---|---|
| Start address | 4 | Starting address for storing the new FW |

**Event(s):** Command Complete; if successful, an OTA Control Response follows.

## 3. Events & Responses

All events/responses use the `0xFF 0xFD 0xFD 0xFF` header.

| Event | Code | Parameters |
|---|---|---|
| Command complete | `0x01` | `command` (1): the command ID this completes; `error_code` (1): result, see [§4.2](#42-error-code-of-command-complete-event) |
| Advertising report | `0x02` | `address_type` (1): 0=public/1=random/2=public-identity/3=random-identity; `address` (6) |
| Connection complete | `0x03` | `status` (1); `address_type` (1); `address` (6); `interval` (2); `latency` (2); `timeout` (2) |
| Disconnection complete | `0x04` | `status` (1); `reason` (1) |
| Basic state response | `0x05` | `group_id` (1); `esl_id` (1); `basic_state` (16): bit0=service needed, bit1=synchronized, bit2=active LED, bit3=pending LED update, bit4=pending display update, bits5–15=RFU |
| Sensor value response | `0x06` | `group_id` (1); `esl_id` (1); `sensor_index` (1); `sensor_value` (n) |
| Display state response | `0x07` | `group_id` (1); `esl_id` (1); `display_index` (1); `image_index` (1) |
| LED state response | `0x08` | `group_id` (1); `esl_id` (1); `led_index` (1) |
| Error response | `0x09` | `group_id` (1); `esl_id` (1); `error_code` (1), see [§4.3](#43-error-code-of-error-response) |
| OTA control response | `0x0A` | `ota_command` (1): which OTA command this responds to; `ota_command_error_code` (1); `response_data` (n): varies per command/error |
| OTA download response | `0x0B` | `ota_notify_code` (1); `ota_notify_data` (n), see table below |
| ESL image information response | `0x0C` | `error_code` (1); `mtu_size` (1): current MTU size |
| ESL image transfer complete | `0x0D` | `complete_number` (1): count completed by the ESL Image Transfer command |

**OTA download response — `ota_notify_code` values:**

| Code | Description | Notify data | Size (octets) |
|---|---|---|---|
| `0x00` | Periodic notification | Expected address | 3 |
| `0x01` | FOTA data not received within the expected interval | — | 0 |
| `0x02` | Received FOTA data's address was not continuous | Expected address | 3 |
| `0x03` | Received FOTA data length incorrect | Length of current OTA data | 1 |
| `0x04` | Total received FOTA data length exceeds bank size | Length of total received OTA data | 4 |
| `0x05` | Received FOTA data's address exceeds bank size | — | 0 |
| `0x06` | FOTA data received but FOTA procedure wasn't started | — | 0 |

## 4. Error Codes

### 4.1 Error code of BLE control & application control

| Value | Description |
|---|---|
| 0 | Success |
| 1 | Unknown type |
| 2 | Not initial |
| 3 | Duplicate init |
| 4 | Data malloc fail |
| 5 | Queue malloc fail |
| 6 | Thread malloc fail |
| 7 | Semaphore malloc fail |
| 8 | Wrong configuration |
| 9 | Busy |
| 10 | SENDTO pointer NULL |
| 11 | SENDTO fail |
| 12 | RECVFROM pointer NULL |
| 13 | RECVFROM no data |
| 14 | RECVFROM fail |
| 15 | RECVFROM length error |
| 16 | RECVFROM memory fail |
| 17 | Timer op |
| 18 | Invalid state |
| 19 | Invalid parameter |
| 20 | Command not supported |
| 21 | Invalid host ID |
| 22 | Invalid BLE handle |
| 23 | Host peripheral database parsing still in progress |
| 24 | Sequential protocol violation (another GATT request already in progress — wait and retry) |
| 25 | BLE TX command queue full |
| 26 | BLE TX data queue full |
| 27 | BLE resource not enough |
| 28 | BLE L2CAP data channel does not exist |
| 29 | BLE L2CAP data destination ID mismatch |

### 4.2 Error code of command complete event

| Value | Description |
|---|---|
| 0x00 | Success |
| 0x01 | Unknown HCI command |
| 0x02 | Unknown connection identifier |
| 0x03 | Hardware failure |
| 0x04 | Page timeout |
| 0x05 | Authentication failure |
| 0x06 | PIN or key missing |
| 0x07 | Memory capacity exceeded |
| 0x08 | Connection timeout |
| 0x09 | Connection limit exceeded |
| 0x0A | Synchronous connection limit to a device exceeded |
| 0x0B | Connection already exists |
| 0x0C | Command disallowed |
| 0x0D | Connection rejected due to limited resources |
| 0x0E | Connection rejected due to security reasons |
| 0x0F | Connection rejected due to unacceptable BD_ADDR |
| 0x10 | Connection accept timeout exceeded |
| 0x11 | Unsupported feature or parameter value |
| 0x12 | Invalid HCI command parameters |
| 0x13 | Remote user terminated connection |
| 0x14 | Remote device terminated connection due to low resources |
| 0x15 | Remote device terminated connection due to power off |
| 0x16 | Connection terminated by local host |
| 0x17 | Repeated attempts |
| 0x18 | Pairing not allowed |
| 0x19 | Unknown LMP PDU |
| 0x1A | Unsupported remote feature |
| 0x1B | SCO offset rejected |
| 0x1C | SCO interval rejected |
| 0x1D | SCO air mode rejected |
| 0x1E | Invalid LMP parameters / invalid LL parameters |
| 0x1F | Unspecified error |
| 0x20 | Unsupported LMP parameter value / unsupported LL parameter value |
| 0x21 | Role change not allowed |
| 0x22 | LMP response timeout / LL response timeout |
| 0x23 | LMP error transaction collision / LL procedure collision |
| 0x24 | LMP PDU not allowed |
| 0x25 | Encryption mode not acceptable |
| 0x26 | Link key cannot be changed |
| 0x27 | Requested QoS not supported |
| 0x28 | Instant passed |
| 0x29 | Pairing with unit key not supported |
| 0x2A | Different transaction collision |
| 0x2C | QoS unacceptable parameter |
| 0x2D | QoS rejected |
| 0x2E | Channel classification not supported |
| 0x2F | Insufficient security |
| 0x30 | Parameter out of mandatory range |
| 0x32 | Role switch pending |
| 0x34 | Reserved slot violation |
| 0x35 | Role switch failed |
| 0x36 | Extended inquiry response too large |
| 0x37 | Secure simple pairing not supported by host |
| 0x38 | Host busy - pairing |
| 0x39 | Connection rejected due to no suitable channel found |
| 0x3A | Controller busy |
| 0x3B | Unacceptable connection parameters |
| 0x3C | Advertising timeout |
| 0x3D | Connection terminated due to MIC failure |
| 0x3E | Connection failed to be established / synchronization timeout |
| 0x3F | MAC connection failed |
| 0x40 | Coarse clock adjustment rejected but will try to adjust using clock dragging |
| 0x41 | Type0 submap not defined |
| 0x42 | Unknown advertising identifier |
| 0x43 | Limit reached |
| 0x44 | Operation cancelled by host |
| 0x45 | Packet too long |
| 0xFE | Connection parameter update rejected |
| 0xFF | Command timeout |

### 4.3 Error code of error response

| Value | Description |
|---|---|
| 0x00 | RFU |
| 0x01 | Unspecified error |
| 0x02 | Invalid opcode |
| 0x03 | Invalid state |
| 0x04 | Invalid image index |
| 0x05 | Image not available |
| 0x06 | Invalid parameters |
| 0x07 | Capacity limit |
| 0x08 | Insufficient battery |
| 0x09 | Insufficient resources |
| 0x0A | Retry |
| 0x0B | Queue full |
| 0x0C | Implausible absolute time |

## 5. Command Flow

### 5.1 Join an ESL device into the ESL network

Actors: **UART tool** (host), **AP**, **ESL** (tag, starts *Unassociated*).

1. **Configure AP** — UART tool → AP: ESL AP configuration (`0x20`); AP → UART tool: Command
   Complete (`0x01`). Then UART tool → AP: Sync configuration (`0x07`); AP → UART tool: Command
   Complete (`0x01`). Meanwhile the ESL tag is broadcasting plain (unencrypted) ADV.
2. **Start AP** — UART tool → AP: Sync control (`0x08`); AP → UART tool: Command Complete
   (`0x01`). The AP now transmits Periodic ADV.
3. **Scan for unassociated devices** — UART tool → AP: Scan control (`0x01`, enable); AP → UART
   tool: Command Complete (`0x01`). The AP continues Periodic ADV in parallel while scanning.
4. **Found unassociated device** — AP → UART tool: an Advertising Report event (`0x02`) arrives
   for each discovered tag.
5. **Configure the unassociated device:**
   - UART tool → AP: Scan control (`0x01`, disable); AP → UART tool: Command Complete (`0x01`).
   - UART tool → AP: Create connection (`0x02`); AP → UART tool: Command Complete (`0x01`), then
     Connection Complete (`0x03`) once the AP↔ESL link is up.
   - UART tool → AP: ESL tag configuration (`0x21`); AP → UART tool: Command Complete (`0x01`).
     The tag moves to *Configuring*.
   - UART tool → AP: Update complete (`0x28`); AP → UART tool: Command Complete (`0x01`).
   - UART tool → AP: Set periodic ADV info transfer (`0x09`); AP → UART tool: Command Complete
     (`0x01`). The AP forwards this as a periodic-adv sync transfer over the connection.
   - On receiving the sync transfer, the tag sets its PAwR subevent
     (`ble_cmd_periodic_sync_subevent_set()`), marks itself `synchronized = 1`, turns on its
     status LED, and actively terminates the connection itself
     (`ble_cmd_conn_terminate()`) — again, the tag disconnects, not the AP. AP → UART tool:
     Disconnection Complete (`0x04`). The tag moves to *Synchronized* and stops normal
     connectable advertising.
6. UART tool → AP: Scan control (`0x01`, enable); AP → UART tool: Command Complete (`0x01`) to
   continue looking for more unassociated devices.

### 5.2 Control a synchronized ESL device

Actors: same three, ESL tag starts *Synchronized*.

1. **Ping over PAwR** — UART tool → AP: ESL ping (`0x24`); AP → UART tool: Command Complete
   (`0x01`), then Basic State Response (`0x05`) once the tag replies in its response slot.
2. **Enhanced create connection** — UART tool → AP: Enhance create connection (`0x03`); AP →
   UART tool: Command Complete (`0x01`), then Connection Complete (`0x03`) once connected via
   the tag's PAwR subevent. The tag moves to *Updating*.
3. **Image update over the enhanced connection:**
   - UART tool → AP: ESL image info (`0x22`); AP → UART tool: Command Complete (`0x01`), then
     ESL Image Information Response (`0x0C`).
   - UART tool → AP: ESL image transfer (`0x23`); AP → UART tool: Command Complete (`0x01`), then
     ESL Image Transfer Complete (`0x0D`) — repeated for each chunk/image.
   - Once all transfers are done, UART tool → AP: Update complete (`0x28`); AP → UART tool:
     Command Complete (`0x01`). The AP forwards this over the connection to the tag as the
     `ESL_CMD_UPDATE_COMPLETE` opcode.
   - The tag checks whether it is synchronized (`esl_ctrl_info.synchronized`); since it is (it
     only reaches *Updating* via an enhanced connection to an already-synchronized tag), the tag
     itself actively terminates the connection (`ble_cmd_conn_terminate()`) — the AP does not
     initiate the disconnect here. AP → UART tool: Disconnection Complete (`0x04`). The tag
     returns to *Synchronized*.
4. **Display an image over PAwR** — UART tool → AP: Display image (`0x2B`); AP → UART tool:
   Command Complete (`0x01`), then Display State Response (`0x07`).
