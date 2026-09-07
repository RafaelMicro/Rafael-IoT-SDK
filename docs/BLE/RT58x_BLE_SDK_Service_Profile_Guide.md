# RT58x BLE SDK Service & Profile Guide

## Introduction

This is a reference for creating a custom BLE GATT service and combining services into a BLE
profile with the Rafael RT58x BLE SDK. It covers how the SDK represents ATT/GATT attributes in
code (`ble_att_param_t`), how to build a service table, and how services are combined into the
per-link profile a demo actually registers.

The worked examples below use the real, current
[`trsps` (Transparent Service)](../../components/network/bluetooth/ble-host/ble-service/trsps/)
service and the
[`periph-trsp`](../../examples/bluetooth-le/peripheral/periph-trsp/) demo's profile wiring — the
same service used as the reference example in the original guide.

## 1. ATT and GATT Introduction

The Generic Attribute Profile (GATT) defines a service framework built on the Attribute Protocol
(ATT). GATT defines the procedures and formats for discovering, reading, writing, notifying, and
indicating characteristics, and for configuring characteristic broadcasts.

### 1.1 Generic Attribute Profile (GATT)

A **profile** is composed of one or more **services** needed to fulfill a use case. A **service**
is composed of **characteristics**, or references to other services. Each characteristic
contains a value and optional descriptors. Services, characteristics, and their components are
all represented as **Attributes** on the GATT server (Bluetooth Core Specification, Vol 3, Part
G).

### 1.2 Attribute Protocol (ATT)

An Attribute has four parts:

- **Attribute Handle** — an index identifying a specific attribute.
- **Attribute Type** — a UUID describing the attribute's meaning.
- **Attribute Value** — the data described by the Attribute Type.
- **Attribute Permissions** — whether read/write access is permitted.

**Service definition** — a service declaration, plus optional Include and Characteristic
definitions.

**Characteristic definition** — a characteristic declaration (which itself carries the
characteristic's properties), a Characteristic Value declaration, and optional descriptor
declarations (e.g. Client Characteristic Configuration for notify/indicate).

Other declaration types not covered here (Include, Server Characteristic Configuration,
Characteristic Presentation Format, etc.) are in the Bluetooth Core Specification, Vol 3, Part G.

## 2. Create a Customized Service

A profile is composed of one or more services, so services are built first.

### 2.1 Design the Service Hierarchy

For each characteristic, decide its **Properties** — which determine how the characteristic
value can be used:

| Property | Bit value (`ble_att_gatt.h`) |
|---|---|
| `GATT_DECLARATIONS_PROPERTIES_BROADCAST` | `0x01` |
| `GATT_DECLARATIONS_PROPERTIES_READ` | `0x02` |
| `GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE` | `0x04` |
| `GATT_DECLARATIONS_PROPERTIES_WRITE` | `0x08` |
| `GATT_DECLARATIONS_PROPERTIES_NOTIFY` | `0x10` |
| `GATT_DECLARATIONS_PROPERTIES_INDICATE` | `0x20` |
| `GATT_DECLARATIONS_PROPERTIES_AUTHENTICATED_SIGNED_WRITES` | `0x40` |
| `GATT_DECLARATIONS_PROPERTIES_EXTENDED_PROPERTIES` | `0x80` |

A characteristic with `NOTIFY` or `INDICATE` needs a Client Characteristic Configuration
Descriptor (CCCD) so the client can enable/disable it.

Worked example — a service with three characteristics (read-only, notify+indicate, read+write):

```
Service
├── Characteristic 1 (Properties: Read)
│   └── Value
├── Characteristic 2 (Properties: Notify, Indicate)
│   ├── Value
│   └── Descriptor: Client Characteristic Configuration
└── Characteristic 3 (Properties: Read, Write)
    └── Value
```

This is exactly the shape of the real `trsps` service below: `UDATR01` (read), `UDATNI01`
(notify/indicate + CCCD), `UDATRW01` (read/write).

Turning the hierarchy into a table of declarations:

| Declaration | Value | Permission |
|---|---|---|
| Service | service UUID | Read only |
| Characteristic 1 | characteristic 1 UUID | Read only |
| Characteristic 1 Value | Properties: Read + characteristic 1 value | (Properties) + no auth |
| Characteristic 2 | characteristic 2 UUID | Read only |
| Characteristic 2 Value | Properties: Notify, Indicate | (Properties) + no auth |
| Characteristic 2 CCCD | client configuration bits | Read, Write |
| Characteristic 3 | characteristic 3 UUID | Read only |
| Characteristic 3 Value | Properties: Read, Write + characteristic 3 value | (Properties) + no auth |

Service and Characteristic declarations are always Read-only (no authentication/authorization).
A Characteristic Value declaration's value is only ever set to a fixed value when its Properties
include Read **and** the device is Peripheral — in that case the fixed value is returned directly
on a Read Request. Otherwise (and in every service shipped in this SDK) the value is left at
zero and the actual data is produced in the service's callback function.

### 2.2 The `ble_att_param_t` Structure

The SDK represents every Attribute — service declaration, characteristic declaration,
characteristic value, or descriptor — as one `ble_att_param_t`
(`components/network/bluetooth/ble-host/ble-cmd-api/include/ble_att_gatt.h`):

```c
typedef struct ble_att_param_s
{
    void            *p_uuid_type;                                 /**< Attribute type which defined by a UUID, an UUID is used to identify every attribute type. */
    void            *p_uuid_value;                                /**< Attribute Value shall be the 16-bit Bluetooth UUID or 128-bit UUID for the service/ characteristic, known as the service/ characteristic UUID.*/
    uint16_t        att_len;                                      /**< The length of attribute value.*/
    uint8_t         property_value;                               /**< Characteristic properties.*/
    uint8_t         db_permission_format;                         /**< UUID format and characteristic permission.*/
    void            (*att_handler)(ble_evt_att_param_t *p_param); /**< Register callback function. */
} ble_att_param_t;
```

- **`p_uuid_type`** — the Attribute Type. For Service/Characteristic/CCCD declarations this is
  one of the SDK's predefined type-UUID arrays (`attr_uuid_type_primary_service`,
  `attr_uuid_type_characteristic`, `attr_uuid_type_client_charc_configuration`, etc., defined in
  `ble-service/common/src/ble_service_common.c`). For a Characteristic Value declaration, it's
  the characteristic's own UUID array.
- **`p_uuid_value`** — the Attribute Value. For Service/Characteristic declarations, this is the
  service/characteristic UUID. For a Characteristic Value declaration it's either a fixed value
  (only valid when Properties include Read and the role is Peripheral) or `0` — every
  Characteristic Value declaration in this SDK uses `0` and produces its data from the callback.
- **`att_len`** — length of the attribute value.
- **`property_value`** — for a Characteristic Value declaration, the Characteristic Properties
  bitmask (§2.1 table above). For every other declaration, it instead encodes the attribute's
  read/write permission directly, using the same `GATT_DECLARATIONS_PROPERTIES_READ` /
  `_WRITE` bits.
- **`db_permission_format`** — UUID format plus encryption/authentication/authorization
  requirements:

  | Macro | Value | Meaning |
  |---|---|---|
  | `ATT_TYPE_FORMAT_128UUID` | `0x00` | 128-bit UUID |
  | `ATT_TYPE_FORMAT_16UUID` | `0x01` | 16-bit UUID |
  | `ATT_VALUE_BOND_ENABLE` | `0x02` | Bond the value when writing |
  | `ATT_PERMISSION_ENC_READ` | `0x04` | Encryption required to read |
  | `ATT_PERMISSION_ENC_WRITE` | `0x08` | Encryption required to write |
  | `ATT_PERMISSION_AUTHE_READ` | `0x10` | Authentication required to read |
  | `ATT_PERMISSION_AUTHE_WRITE` | `0x20` | Authentication required to write |
  | `ATT_PERMISSION_AUTHO_READ` | `0x40` | Authorization required to read |
  | `ATT_PERMISSION_AUTHO_WRITE` | `0x80` | Authorization required to write |

- **`att_handler`** — the callback invoked on read/write/notify/indicate for a Characteristic
  Value declaration. Every other declaration sets this to `attr_null_access` (a no-op defined in
  `ble_service_common.c`), since it needs no callback.

### 2.3 Worked Example: the TRSPS Service

The real UUIDs and declarations from
[`ble_service_trsps.c`](../../components/network/bluetooth/ble-host/ble-service/trsps/src/ble_service_trsps.c):

```c
const uint16_t attr_uuid_trsps_primary_service[] =        // 128-bit UUID: 00112233445566778899AABBCCDDEEFF
{
    0xEEFF, 0xCCDD, 0xAABB, 0x8899, 0x6677, 0x4455, 0x2233, 0x0011,
};
const uint16_t attr_uuid_trsps_charc_udatr01[] =           // 101112131415161718191A1B1C1D1E1F
{
    0x1E1F, 0x1C1D, 0x1A1B, 0x1819, 0x1617, 0x1415, 0x1213, 0x1011,
};
const uint16_t attr_uuid_trsps_charc_udatni01[] =          // 303132333435363738393A3B3C3D3E3F
{
    0x3E3F, 0x3C3D, 0x3A3B, 0x3839, 0x3637, 0x3435, 0x3233, 0x3031,
};
const uint16_t attr_uuid_trsps_charc_udatrw01[] =          // 505152535455565758595A5B5C5D5E5F
{
    0x5E5F, 0x5C5D, 0x5A5B, 0x5859, 0x5657, 0x5455, 0x5253, 0x5051,
};
```

**Service declaration:**

```c
const ble_att_param_t att_trsps_primary_service =
{
    (void *)attr_uuid_type_primary_service,
    (void *)attr_uuid_trsps_primary_service,
    sizeof(attr_uuid_trsps_primary_service),
    ( GATT_DECLARATIONS_PROPERTIES_READ ),
    ( ATT_TYPE_FORMAT_16UUID ),
    attr_null_access,
};
```

**Characteristic 1 (`UDATR01`, read-only) — declaration + value:**

```c
const ble_att_param_t att_trsps_characteristic_udatr01 =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_trsps_charc_udatr01,
    sizeof(attr_uuid_trsps_charc_udatr01),
    ( GATT_DECLARATIONS_PROPERTIES_READ ),
    ( ATT_TYPE_FORMAT_16UUID ),
    attr_null_access,
};

const ble_att_param_t att_trsps_udatr01 =
{
    (void *)attr_uuid_trsps_charc_udatr01,
    (void *)0,
    0,
    ( GATT_DECLARATIONS_PROPERTIES_READ ),
    ( 0x00 ),                              // 128-bit UUID, no extra permission bits
    ble_svcs_trsps_handler,                // registered callback function
};
```

**Characteristic 2 (`UDATNI01`, notify + indicate) — declaration, value, and CCCD:**

```c
const ble_att_param_t att_trsps_characteristic_udatni01 =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_trsps_charc_udatni01,
    sizeof(attr_uuid_trsps_charc_udatni01),
    ( GATT_DECLARATIONS_PROPERTIES_READ ),
    ( ATT_TYPE_FORMAT_16UUID ),
    attr_null_access,
};

const ble_att_param_t att_trsps_udatni01 =
{
    (void *)attr_uuid_trsps_charc_udatni01,
    (void *)0,
    0,
    ( GATT_DECLARATIONS_PROPERTIES_NOTIFY | GATT_DECLARATIONS_PROPERTIES_INDICATE ),
    ( 0x00 ),
    ble_svcs_trsps_handler,
};

const ble_att_param_t att_trsps_udatni01_client_charc_configuration =
{
    (void *)attr_uuid_type_client_charc_configuration,
    (void *)0,
    0,
    ( GATT_DECLARATIONS_PROPERTIES_READ | GATT_DECLARATIONS_PROPERTIES_WRITE ),
    ( ATT_TYPE_FORMAT_16UUID ),
    ble_svcs_trsps_handler,
};
```

**Characteristic 3 (`UDATRW01`, read + write) — declaration + value:**

```c
const ble_att_param_t att_trsps_characteristic_udatrw01 =
{
    (void *)attr_uuid_type_characteristic,
    (void *)attr_uuid_trsps_charc_udatrw01,
    sizeof(attr_uuid_trsps_charc_udatrw01),
    ( GATT_DECLARATIONS_PROPERTIES_READ ),
    ( ATT_TYPE_FORMAT_16UUID ),
    attr_null_access,
};

const ble_att_param_t att_trsps_udatrw01 =
{
    (void *)attr_uuid_trsps_charc_udatrw01,
    (void *)0,
    0,
    ( GATT_DECLARATIONS_PROPERTIES_READ | GATT_DECLARATIONS_PROPERTIES_WRITE_WITHOUT_RESPONSE
      | GATT_DECLARATIONS_PROPERTIES_WRITE ),
    ( 0x00 ),
    ble_svcs_trsps_handler,
};
```

### 2.4 Combine into a Service Table

The declarations above are combined, in ATT database order, into one macro
(`ble_service_trsps.h`):

```c
#define ATT_TRSPS_SERVICE                                    \
    &att_trsps_primary_service,                              \
    &att_trsps_characteristic_udatr01,                       \
    &att_trsps_udatr01,                                      \
    &att_trsps_characteristic_udatni01,                      \
    &att_trsps_udatni01,                                     \
    &att_trsps_udatni01_client_charc_configuration,          \
    &att_trsps_characteristic_udatrw01,                      \
    &att_trsps_udatrw01
```

### 2.5 Implement the Service's Definitions and Callback

**Event codes** the service posts to the application (`ble_service_trsps.h`):

```c
#define BLESERVICE_TRSPS_UDATR01_READ_EVENT                   0x01
#define BLESERVICE_TRSPS_UDATR01_READ_RSP_EVENT               0x02
#define BLESERVICE_TRSPS_UDATNI01_NOTIFY_EVENT                0x03
#define BLESERVICE_TRSPS_UDATNI01_INDICATE_CONFIRM_EVENT      0x04
#define BLESERVICE_TRSPS_UDATNI01_INDICATE_EVENT              0x05
#define BLESERVICE_TRSPS_UDATNI01_CCCD_READ_EVENT             0x06
#define BLESERVICE_TRSPS_UDATNI01_CCCD_READ_RSP_EVENT         0x07
#define BLESERVICE_TRSPS_UDATNI01_CCCD_WRITE_EVENT            0x08
#define BLESERVICE_TRSPS_UDATNI01_CCCD_WRITE_RSP_EVENT        0x09
#define BLESERVICE_TRSPS_UDATRW01_READ_EVENT                  0x0a
#define BLESERVICE_TRSPS_UDATRW01_READ_RSP_EVENT              0x0b
#define BLESERVICE_TRSPS_UDATRW01_WRITE_EVENT                 0x0c
#define BLESERVICE_TRSPS_UDATRW01_WRITE_RSP_EVENT             0x0d
#define BLESERVICE_TRSPS_UDATRW01_WRITE_WITHOUT_RSP_EVENT     0x0e
```

Every Characteristic Value/CCCD declaration above points its `att_handler` at one shared
callback, `ble_svcs_trsps_handler()`. Internally it looks up which registered link the event
belongs to, then dispatches by GATT role:

```c
void ble_svcs_trsps_handler(ble_evt_att_param_t *p_param)
{
    uint8_t index;

    if (ble_svcs_common_info_index_query(p_param->host_id, p_param->gatt_role, MAX_NUM_CONN_TRSPS, trsps_basic_info, &index) != BLE_ERR_OK)
    {
        return;   // host id not registered -> nothing to do
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_CLIENT)
    {
        handle_trsps_client(index, p_param);
    }

    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        handle_trsps_server(index, p_param);
    }
}
```

`handle_trsps_server()` matches the incoming ATT opcode and handle number against the service's
own handle table, then either answers automatically (e.g. `UDATR01` reads are served directly
from a fixed buffer via `ble_svcs_auto_handle_read_req()`, and CCCD reads/writes are handled by
`ble_svcs_auto_handle_cccd_read_req()`/`ble_svcs_handle_cccd_write_req()`) or posts an app-facing
event — e.g. a write to `UDATRW01` sets `p_param->event = BLESERVICE_TRSPS_UDATRW01_WRITE_EVENT`
and forwards it to the application's own registered callback. `handle_trsps_client()` does the
mirror image for the Client role (read responses, write responses, notifications,
indications).

## 3. Create a Customized Profile

A profile combines one or more services into the attribute database for a link. The tables below
are from [`periph-trsp`](../../examples/bluetooth-le/peripheral/periph-trsp/)'s
`ble-app-profile/src/ble_profile_def.c` and `include/ble_profile.h`.

### 3.1 Combine Services into a Profile Table

```c
/** Service Combination #00
 * @note included ATT_GAPS_SERVICE, ATT_GATTS_SERVICE, ATT_DIS_SERVICE, ATT_TRSPS_SERVICE
*/
const ble_att_param_t *const att_service_comb00[] =
{
    &ATT_NULL_INVALID,       //mandatory, don't remove it.
    ATT_GAPS_SERVICE,
    ATT_GATTS_SERVICE,
    ATT_DIS_SERVICE,
    ATT_TRSPS_SERVICE
};
```

`ATT_NULL_INVALID` is a mandatory placeholder at index 0 (attribute handle 0 is reserved/invalid
per the ATT spec); every other entry is one service's `ATT_..._SERVICE` combination macro (§2.4).

### 3.2 BLE Link Definition Table

Each link gets a Client profile pointer and a Server profile pointer (either may be `0`/NULL if
that role isn't used on that link):

```c
const ble_att_role_by_id_t att_db_link[] =
{
    // Link 0
    {
        ((const ble_att_param_t **)0),     // Client Profile
        att_service_comb00,                // Server Profile
    },
};
```

### 3.3 BLE Link Mapping Table

At runtime, each attribute in `att_service_comb00[]` gets a corresponding runtime handle/data
slot (`ble_att_handle_param_t`) — the array is sized to match the profile table it mirrors:

```c
ble_att_handle_param_t att_hdl_para_links00[SIZE_ARRAY_ROW(att_service_comb00)]; // Link 0 Server

const ble_att_db_mapping_by_id_t att_db_mapping[] =
{
    // Link 0
    {
        ((ble_att_handle_param_t *)0),     // Client Link Parameter
        att_hdl_para_links00,              // Server Link Parameter
    },
};
```

### 3.4 BLE Link Mapping Size Table

```c
const ble_att_db_mapping_by_id_size_t att_db_mapping_size[] =
{
    // Link 0
    {
        0,                                      // Client Link Mapping Size
        SIZE_ARRAY_ROW(att_service_comb00),     // Server Link Mapping Size
    },
};
```

### 3.5 Maximum Number of Connections

```c
const uint8_t max_num_conn_host = (SIZE_ARRAY_ROW(att_db_mapping_size));
```

The number of links is driven by how many rows exist in `att_db_mapping_size[]` — one row per
link, matching `att_db_link[]`/`att_db_mapping[]`. `periph-trsp` defines one link
(`CONFIG_BLE_SUPPORT_NUM_CONN_MAX=1`, wired to `BLE_SUPPORT_NUM_CONN_MAX` at build time).

### 3.6 BLE Host Connection Link Information

```c
uint8_t *param_rsv_host[SIZE_ARRAY_ROW(att_db_mapping_size)][(REF_SIZE_LE_HOST_PARA >> 2)];
```

Reserved working memory for the host layer, sized per link. The application's own per-link
state is separate from this — see `ble_info_link0_t`/`ble_app_link_info[]` below.

### 3.7 Define the Number of Connection Links for Each Service

Each service that supports more than one simultaneous instance needs its own connection-count
define, referenced by that service's internal per-link bookkeeping arrays
(`ble_profile.h`):

```c
#define MAX_NUM_CONN_GAPS      1
#define MAX_NUM_CONN_GATTS     1
#define MAX_NUM_CONN_DIS       1
#define MAX_NUM_CONN_TRSPS     1
```

### 3.8 Implement the Profile's Application Data and Callback

The application's own per-link attribute-information struct groups every service's info struct
for that link:

```c
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
```

`ble_profile_app.c` instantiates this per link and registers each service against it during
initialization:

```c
ble_info_link0_t   ble_info_link0;

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

The demo's own event handler (`periph-trsp/src/main.c`) then calls each service's `_init()`
(e.g. `ble_svcs_trsps_init(host_id, BLE_GATT_ROLE_SERVER, &p_profile_info->svcs_info_trsps,
ble_svcs_trsps_evt_handler)`) and implements `ble_svcs_trsps_evt_handler()` to react to the
event codes from §2.5 — this is the "customized function" a new service/profile ultimately
needs on top of everything above.
