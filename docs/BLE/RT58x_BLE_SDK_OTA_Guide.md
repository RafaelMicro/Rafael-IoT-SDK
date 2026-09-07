# RT58x BLE SDK OTA Guide

## Introduction

RT58x BLE SDK uses dual-bank flash to perform firmware over-the-air (FOTA) updates so a valid
image is always active. Bank 0 is the active FW block; Bank 1 is the FW update buffer that new
firmware is streamed into before it's applied.

```
+----------------------+
| bootloader           |  32K
+----------------------+
| Bank 0 — Active FW   |
| ...                  |  928K (Bank 0 + Bank 1)
+----------------------+
| Bank 1 — FW update   |
| buffer               |
+----------------------+
| MP Sector            |  64K
+----------------------+
```

These sizes are for a 1MB-flash chip variant. Exact bank addresses/sizes are chip-specific —
see `FOTA_UPDATE_BANK_INFO_ADDRESS`, `FOTA_UPDATE_BUFFER_FW_ADDRESS`, `SIZE_OF_FOTA_BANK`, and
`APP_START_ADDRESS` per `CONFIG_RT581`/`RT582`/`RT583`/`RT584H`/`RT584HA4`/`RT584L`/`RF1301` in
[`components/common/fota/Include/fota_define.h`](../../components/common/fota/Include/fota_define.h).

A complete FOTA update involves four tools/actors:

1. **KEIL/GCC** builds the new FW as a plain `.bin` file.
2. **PC tool** combines the new FW binary with system-information/address metadata into a FOTA
   bin file.
3. **Smart phone** app loads the FOTA bin file, parses out the system information and new FW,
   and queries the BLE device's current system information.
4. **BLE device** reports its current FW information; the phone decides whether to proceed with
   the update, then streams the new FW over the connection.

```
KEIL/GCC        PC tool                    Smart phone                BLE device
   |                |                            |                        |
   |-- BLE bin ---->|                            |                        |
   |         combine FW + sysinfo                |                        |
   |                |-- FOTA bin file ---------->|                        |
   |                |                     parse FW info & new FW          |
   |                |                            |-- Write req ---------->|
   |                |                            |<- Write rsp -----------|
   |                |                            |   (exchange current FW information)
   |                |                            |<- indication ----------|
   |                |                            |-- confirmation ------->|
   |                |                            |-- Write cmd ---------->|
   |                |                            |   (transfer new FW over the air)
```

## 1. Running FOTA Update Demo

Prerequisites: the target BLE device must already have the bootloader and the FOTA update
service programmed. A ready-made demo that exposes this service is
[`periph-trsp-fota`](../../examples/bluetooth-le/peripheral/periph-trsp-fota/) (advertised device
name `TRSP_FOTA_DEMO`), which wires up the TRSP and FOTA GATT services and drives the FOTA state
machine from `ble_fota_cmd()`/`ble_fota_data()`/`ble_fota_timerexpiry_handler()`.

1. Build a new BLE `.bin` file with updated BLE system information (e.g. FW version string).
2. Load the new BLE `.bin` file into the PC tool and key in the address information; the tool
   combines these into a FOTA bin file.
3. Use the mobile app to query the BLE device's current FW version and compare it against the
   FOTA bin file's version to decide whether to perform the update.

### 1.1 Build the BLE bin File with BLE System Information

BLE system information is the `systeminfo` array defined in
[`components/network/bluetooth/ble-host/fota/src/ble_fota.c`](../../components/network/bluetooth/ble-host/fota/src/ble_fota.c):

```c
const sys_information_t systeminfo =
{
    .prefix = "VerGet",          /* Specific string for FOTA tool to search system information, in the code base should not appear second string like this */
    .sysinfo = BLE_VERSION,      /* An example of current system information, able to modify by user */
    .feature_list = BLE_FOTA_IMAGE_COMPRESS_SUPPORT,
};
```

`sys_information_t` is `{ prefix[7]; sysinfo[16]; feature_list; }` — a 7-byte fixed prefix
string, up to 16 bytes of free-form version/system info (`BLE_VERSION`, default `"sys ver 0001"`),
and a 1-byte feature bitmask (currently just `BLE_FOTA_IMAGE_COMPRESS_SUPPORT = 0x01`). The
string `"VerGet"` is what the PC tool searches for in the built `.bin` to locate this structure —
it must not appear anywhere else in the firmware image.

### 1.2 Using the IOT Evaluation Tool to Get a FOTA bin File

Open the IOT Evaluation Tool and click the "FOTA" icon to reach the "FOTA update tool" page.

#### 1.2.1 Normal Mode

1. Set the active FW start address (default `0x8000`) — usually left unchanged.
2. Select the `.bin` file already built by KEIL/GCC.
3. Click "Output" to produce the FOTA bin file.

Since SDK v1.3.0, it's recommended to check the "Compress(LZMA)" option.

#### 1.2.2 Security Boot Mode

Security Boot Mode wraps [OpenSSL](http://slproweb.com/products/Win32OpenSSL.html) to generate an
ECDSA (SHA-256) key pair and sign firmware images, so the bootloader can verify a signature before
accepting an update. ECDSA is preferred over RSA here for its shorter keys at equivalent security,
and faster sign/verify.

Tool workflow (6 steps):

1. Type a private-key filename (e.g. `private.pem`) and click "GENERATE".
2. Type a public-key filename (e.g. `public.pem`) and click "GENERATE" — this also produces a
   `public.der` certificate.
3. Set the active FW start address (default `0x8000`; ignored when signing `bootloader.bin`).
4. Select the file to sign (e.g. `bootloader.bin`).
5. Check the "signature" option.
6. Click "Output" — the tool appends the signature to the end of the file and reports the output
   path/filename.

The `bootloader.bin` file cannot have "Compress(LZMA)" checked.

The PC tool generates 32 bytes of signing information, stored at the front of the FOTA bin file:

```
Octet: 0               4               8               12               16
       +---------------+---------------+---------------+----------------+
       |                      BLE FW information                        |   <- PC-tool generated
       +---------------+---------------+---------------+----------------+
       |      CRC      |     Start     |      End      |    Reserved    |   <- PC-tool generated
       +---------------+---------------+---------------+----------------+
       |     Padding data (= active FW start address, default 0x8000)   |   <- PC-tool generated
       +----------------------------------------------------------------+
       |                Updating FW (built by KEIL/GCC)                 |
       +----------------------------------------------------------------+
```

#### 1.2.3 Complete Steps to Operate Security Boot Mode

Worked example with three prebuilt binaries (`bootloader.bin`,
`TRSP_FOTA_Periph_0001.bin`, `TRSP_FOTA_Periph_0002.bin`) and six tool-generated outputs:

| # | File | Purpose |
|---|---|---|
| 4 | `private.pem` | FOTA private key |
| 5 | `public.pem` | FOTA public key |
| 6 | `bootloader_Signature.bin` | Signed bootloader, for the ISP tool |
| 7 | `TRSP_FOTA_Periph_0001_Signature_ISP.bin` | Signed app image case 1, for the ISP tool |
| 8 | `TRSP_FOTA_Periph_0001_Signature_FOTA.bin` | Signed app image case 1, for the mobile app |
| 9 | `TRSP_FOTA_Periph_0002_Signature_FOTA.bin` | Signed app image case 2, for the mobile app |

Generate `private.pem` and `public.pem` first (both land next to `IoT_EVALUATION_TOOL.exe`) —
everything else depends on them. Then:

- **#6** — sign `bootloader.bin` with "signature" checked; this becomes the bootloader the ISP
  tool flashes for security mode.
- **#7** — sign `TRSP_FOTA_Periph_0001.bin` with both "Signature" and "Output bin For ISP Tool"
  checked, for the initial ISP flash.
- **#8** — sign the same `TRSP_FOTA_Periph_0001.bin` with only "Signature" checked, for
  distribution through the FOTA app.
- **#9** — same as #8, for `TRSP_FOTA_Periph_0002.bin` (the FW version used to test an
  over-the-air update once the device is already in security mode).

**Put the DUT into Security Mode:**

1. Flash `bootloader_Signature.bin` and `TRSP_FOTA_Periph_0001_Signature_ISP.bin` to the DUT with
   the ISP tool — same steps as any ISP download, just using the signed binaries.
2. Write `public.pem` to the DUT's security page. This is the step that actually switches the
   device into security mode: from then on, it checks the public key on every reset. When
   pressing "Write", reset the DUT within 7 seconds to enter ISP-connect mode; success prints
   "write MAC address and public key OK".

Once in security mode, use the mobile app's FOTA function to transfer
`TRSP_FOTA_Periph_0002_Signature_FOTA.bin` to the DUT — it should update successfully. Sending an
unsigned bin file will fail the update.

### 1.3 Start the FOTA Update Using the Mobile App

1. Put the generated FOTA bin file into the phone's `download/fota_bin` folder.
2. Open the "Rafael BLE" app, scan for and connect to the BLE device.
3. Open the FOTA update page for the connected device. No BLE pairing/bonding is required — the
   FOTA service's GATT attributes all have bonding/encryption/authentication permissions
   disabled (`ble_service_fotas.c`), so a plain GATT connection is sufficient.
4. On the FOTA update page:
   - **Step 1** — select the FOTA bin file.
   - **Step 2** — the app parses the FOTA bin file's system information and queries the device's
     current system information.
   - **Step 3** — the app checks the device's FW update buffer and determines whether this is a
     resumed or a new transmission. If an incomplete previous update is found in the buffer and
     it doesn't match the new FW, the app sends an Erase command (see §3.1) — after that,
     reconnect from the scan page and restart from Step 1. Otherwise, the app resumes from the
     last aborted address.
   - **Step 4** — start transferring the FW over the air; the app shows progress and log
     information.
   - **Step 5** — once the device confirms it received the FW completely, it reboots to apply
     the new FW.

### 1.4 Start the FOTA Update for Multiple Devices Using the Mobile App

For updating several devices in one pass:

1. Put the FOTA bin file into `download/fota_bin` as above.
2. Open the app and scan; since no connection is made yet, select devices using their checkbox
   rather than tapping to connect.
3. Once more than one device is selected, a "TO DO OTA" button appears — press it to enter the
   multi-device OTA screen, which re-displays each selected device's name/MAC for confirmation
   (return to the scan screen to reselect if anything looks wrong).
4. On the multi-device page:
   - **Step 1** — select the FOTA bin file.
   - **Step 2** — start the loop below over every selected device.

```
loop over each selected device:
    connect device
    check version              [§1.3 step 2]
    check Bank 1 state         [§1.3 step 3]
    download FW                [§1.3 step 4]
    CRC check and reboot       [§1.3 step 5]
    disconnect
more devices? -> yes: repeat / no: continue

save log [download/fota_log]
finish
```

## 2. FOTA Communication Flow

Once the BLE device is paired with the phone, the full handshake proceeds as:

```
Smart phone                                              BLE device
    |<----------------------- Adv_ind ------------------------------|   (x3)
    |======================= connection established ================|
    |------ FOTA command: Query ----------------------------------->|
    |<----- Indication: BLE system information ---------------------|   (Query BLE FW information)
    |------ FOTA command: Start ----------------------------------->|
    |<----- Indication: Start result -------------------------------|   (Return start result by bank status)
    |------ FOTA command: Erase (optional) ------------------------>|
    |<----- Indication: Success ------------------------------------|
    |====== disconnect & back to standby, erase legacy FW ==========|   (if Bank 1 has useless legacy FW)
    |------ FOTA data (Write cmd, repeated) ----------------------->|
    |<----- Notification (optional) --------------------------------|   (programming FOTA data into bank if valid)
    |====== disconnect & system reboot if updating FW is valid =====|
```

Two functions in
[`ble_fota.c`](../../components/network/bluetooth/ble-host/fota/src/ble_fota.c) drive this:

```c
void ble_fota_cmd(uint8_t host_id, uint8_t length, uint8_t *p_data);
void ble_fota_data(uint8_t host_id, uint8_t length, uint8_t *p_data);
```

### 2.1 FOTA Command

`ble_fota_cmd()` parses one command per call. The first payload byte is the command ID
(`fota_cmd_t`, `ble_fota.h`); each ID carries different trailing data.

| Command | ID | Parameters | Notes |
|---|---|---|---|
| Query | `0x00` | — | Query current system information |
| Start | `0x01` | FW length:4, FW CRC:4, Notify interval:4, FW info:1, Signature length:3 | See below |
| Erase | `0x02` | — | Terminate the connection and erase legacy FW + FW info; only sent if the device reports a leftover FW in Bank 1 that doesn't match the incoming update |
| Apply | `0x03` | Bank0 address:4 | Apply the new FW once its length/CRC matched the Start command |

**Start command detail** — `fota_cmd_start_t` packs `fw_info` (1 byte) and `signature_len` (3
bytes) into one 4-byte field, so the full Start payload (after the 1-byte command ID) is 16
bytes:

```
octets    |1          |4       |4    |4         |1     |3          |
parameter | CommandID | FW     | FW  | Notify   | FW   | Signature |
          | (0x01)    | length | CRC | interval | info | length    |
```

(matches the format comment above `OTA_CMD_START` in `ble_fota.c`). `fw_info` is a bitmask
(`fota_define.h`): bit 0 = image is LZMA-compressed, bit 1 = image carries an ECDSA signature
(`signature_len` is only meaningful when this bit is set). For backward compatibility,
`ble_fota_cmd()` also still accepts a legacy 12-byte payload (length+CRC+interval only, no
`fw_info`/`signature_len` at all) — a 13-byte payload (matching only `fw_info` with no signature
field) is not one of the two currently accepted sizes.

If a Start command arrives while a FOTA update is already in progress, the device does not start
over — it responds with the "Already start" indication (below) instead, and the phone resumes
from the reported address.

**Indications** — every command gets an indication in response, with a 1-byte error code
(`fota_errcode_t`, `ble_fota.h`) plus, for some codes, extra parameters:

| Error code | Value | Extra parameters | Notes |
|---|---|---|---|
| No error | `0x00` | none, **except** for a successful Query response, which appends the full system info: Info:16, Feature list:1 | |
| Command error | `0x01` | — | Unsupported command ID |
| Already start | `0x02` | Updating FW length:4, Updating FW CRC:4, Updating FW next-expect address:3 | FOTA procedure already started; the third field is the already-buffered length, which doubles as the resume address (source names it `stored_fw_len`) |
| Update not started | `0x03` | — | Apply received before Start |
| Flash erase error | `0x04` | — | Bank flash erase failed |
| FW CRC error | `0x05` | — | Received FW's CRC didn't match |
| FW length error | `0x06` | — | Received FW's length didn't match |
| Out of bank size | `0x07` | Current FW bank size:4 | Updating FW's length is larger than the bank |

### 2.2 FOTA Data

`ble_fota_data()` handles the actual FW transfer. Every data packet starts with a 4-byte header:

| Field | Size (octets) | Notes |
|---|---|---|
| Data programming address | 3 | |
| Data programming length | 1 | |
| Data | variable | |

If the header is invalid, the device replies with a notification (below); otherwise there is no
reply unless the phone configured periodic notifications via the Start command's notify
interval.

| Notify reason | Value | Extra parameters | Notes |
|---|---|---|---|
| Periodic | `0x00` | Updating FW next-expect address:3 | Sent every `notify_interval` bytes, per the Start command |
| Timeout | `0x01` | — | Defined in `ble_fota.h` but not currently triggered anywhere in `ble_fota.c` — no code path sets this notify reason |
| Address unexpected | `0x02` | Updating FW next-expect address:3 | Received data's address wasn't the expected next address |
| Length error | `0x03` | Received FOTA data length:1 | |
| Total length error | `0x04` | Total received FOTA data length:4 | Total received length would exceed the bank size |
| Address error | `0x05` | — | Received data's address is larger than the bank size |
| Not started | `0x06` | — | Data received but no FOTA procedure has started |
