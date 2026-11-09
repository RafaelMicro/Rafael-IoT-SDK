# RT58x Zigbee Gateway Command Manual

## About This Document

This document specifies the binary UART command protocol used to drive an RT58x Zigbee
coordinator module as a gateway. It covers Zigbee network and device management (ZDO discovery,
binding, permit-join, network formation, install codes), OTA image distribution, and the
application-level ZCL services (device information, attributes, Identify, Groups, Scenes, On/Off,
Level Control, Color Control, Door Lock, IAS Zone, custom clusters).

This is the wire protocol implemented by
[`examples/zigbee/gateway-module`](../../examples/zigbee/gateway-module/):

| Concern | Source |
|---|---|
| Frame parsing / UART transport | `src/uart_handler.c` |
| Network and device management commands (`0x0000-xxxx`) | `src/zigbee_cmd_nwk.c`, `include/zigbee_cmd_nwk.h` |
| Application service commands (`0x0001-xxxx` … `0xFC00-xxxx`) | `src/zigbee_cmd_app.c`, `include/zigbee_cmd_app.h` |
| OTA commands (`0xF000-xxxx`) | `src/zigbee_cmd_ota.c`, `include/zigbee_cmd_ota.h` |
| ZCL responses and indications forwarded to the host | `src/zigbee_zcl_msg_handler.c` |

The module also exposes a human-readable CLI over the debug UART (`src/zigbee_cli.c`) for the same
operations; that CLI is not part of this protocol.

## Implementation Status

Every command specified in this document is implemented by
[`examples/zigbee/gateway-module`](../../examples/zigbee/gateway-module/), verified against its
command dispatch (`g_zb_cmd_func[]` in `src/zigbee_cmd_nwk.c`, the per-cluster handlers in
`src/zigbee_cmd_app.c`, and `_gw_ota_cmd_handle()` in `src/zigbee_cmd_ota.c`).

Four Network Management commands that appear in older revisions of this specification are
deliberately **omitted** here, because the firmware has no handler for them: Neighbor information
request/response (`0x0000-0031` / `0x0000-8031`) and Routing information request/response
(`0x0000-0032` / `0x0000-8032`). There is no dispatch entry and no `ZIGBEE_CMD_*` enumerator for
`0x31`/`0x32` at all, so a host sending them gets no reply.

One discrepancy is worth knowing about when reading the source:

- **The On/Off enumerator names in the source are inverted relative to the wire IDs.**
  `zigbee_cmd_app.h` declares `GW_CMD_APP_SRV_ONOFF_ON = 0` and `GW_CMD_APP_SRV_ONOFF_OFF = 1`, but
  `_cmd_dev_onoff_ctrl_handle()` passes the command index straight through as the ZCL On/Off
  command ID, where `0x00` is Off and `0x01` is On. The wire behaviour therefore matches this
  document (Off = `0x0007-0000`, On = `0x0007-0001`); only the enumerator names are misleading.

`GW_CMD_APP_SRV_GET_LOG_RECORD` (`0x0024-0003`) exists in the Door Lock enumeration in
`zigbee_cmd_app.h` but has no handler case and is not specified here.

## Table of Contents

- [1. Introduction](#1-introduction)
- [2. Hardware Interface Setup](#2-hardware-interface-setup)
- [3. Command Data Format](#3-command-data-format)
  - [3.1 Command Structure](#31-command-structure)
  - [3.2 Command Example](#32-command-example)
- [4. Device and Network Management Service](#4-device-and-network-management-service)
  - [4.1 Device and Service Discovery](#41-device-and-service-discovery)
  - [4.2 Device Bind Management](#42-device-bind-management)
  - [4.3 Network Management](#43-network-management)
  - [4.4 Device and Network Management Service Status Enumeration Description](#44-device-and-network-management-service-status-enumeration-description)
- [5. Over the Air(OTA) Upgrade Management](#5-over-the-airota-upgrade-management)
  - [5.1 OTA file upload flow](#51-ota-file-upload-flow)
  - [5.2 OTA Upgrade command list](#52-ota-upgrade-command-list)
  - [5.3 OTA Command Example](#53-ota-command-example)
- [6. Application Service Management](#6-application-service-management)
  - [6.1 Device Information](#61-device-information)
  - [6.2 Device Identify](#62-device-identify)
  - [6.3 Group Management](#63-group-management)
  - [6.4 Scene Management](#64-scene-management)
  - [6.5 On/Off Control](#65-onoff-control)
  - [6.6 Level Control](#66-level-control)
  - [6.7 Alarm](#67-alarm)
- [7. Lighting Application Service](#7-lighting-application-service)
  - [7.1 Color Control](#71-color-control)
- [8. Sensor Application Cluster Information](#8-sensor-application-cluster-information)
  - [8.1 Illuminance Measurement (Cluster ID: 0x0400)](#81-illuminance-measurement-cluster-id-0x0400)
  - [8.2 Temperature Measurement (Cluster ID: 0x0402)](#82-temperature-measurement-cluster-id-0x0402)
  - [8.3 Pressure Measurement (Cluster ID: 0x0403)](#83-pressure-measurement-cluster-id-0x0403)
  - [8.4 Flow Measurement (Cluster ID: 0x0404)](#84-flow-measurement-cluster-id-0x0404)
  - [8.5 Relative Humidity Measurement (Cluster ID: 0x0405)](#85-relative-humidity-measurement-cluster-id-0x0405)
  - [8.6 Occupancy Sensing (Cluster ID: 0x0406)](#86-occupancy-sensing-cluster-id-0x0406)
- [9. Closures](#9-closures)
  - [9.1 Door Lock (Cluster ID: 0x0101)](#91-door-lock-cluster-id-0x0101)
- [10. Security and Safety](#10-security-and-safety)
  - [10.1 IAS Zone (Cluster ID: 0x0500)](#101-ias-zone-cluster-id-0x0500)
- [11. Custom cluster](#11-custom-cluster)
  - [11.1 Custom command (0xFC00-0000)](#111-custom-command-0xfc00-0000)
  - [11.2 Custom command response (0xFC00-8000)](#112-custom-command-response-0xfc00-8000)
- [12. Application Service Management Status Enumeration Description](#12-application-service-management-status-enumeration-description)

## 1. Introduction

This document is the command sets for implementing the Zigbee gateway function with coordinator module. It includes the Zigbee network management and application service commands.

## 2. Hardware Interface Setup

The coordinator module is connected to host control unit by UART port. The default baud rate is 115200 with 8-bit data length, no parity bit, and 1 stop bit format.

## 3. Command Data Format

### 3.1 Command Structure

The Zigbee gateway command is constructed as the following format. It uses the little endian format:

| | | | | | | | |
|---|---|---|---|---|---|---|---|
| Header | Length | Comm-<br>and<br>Id | Address | Address<br>mode | Endpoint | Param-<br>eter | Checks-<br>um |
| 4 octets | 1 octet | 4 octets | 2 octets | 1 octets | 0/1 octet | n octets | 1 octet |

#### Header field

The command header is 4 bytes long and should be formatted as 0xFF 0xFC 0xFC 0xFF.

#### Length field

The command data length value is the length sum of command id, address, address mode, endpoint(if present) and parameter.

#### Command id field

The command id is 4 bytes long and will be defined in the following command description.

#### Address field

Address data is 2 bytes long and could be unicast or group address identified by address mode field. Some special addresses are defined as broadcast address and the address mode data will be ignored if these addresses are used.

0xFFFF: Broadcast to all devices in PAN.

0xFFFE: Reserved.

0xFFFD: Broadcast to devices which Rx are on when in idle state

(macRxOnWhenIdle = TRUE).

0xFFFC: Broadcast to all routers and coordinator.

0xFFFB: Broadcast to all low power routers only.

0xFFF8 – 0xFFFA: Reserved

Please note that the ‘Address field’ here will represent different meanings depending on the direction of the command transmission: When the command is initiated by the user and sent to the gateway module, the 'Address field’ represents the address of the target device(destination address); when the gateway receives the relevant message and reports back to the user's upper layer, the 'Address field’ represents the address of the issuer device(source address).

Direction A: Address field is 0xAABB (destination address to the zigbee device)

Direction B: Address field is 0xAABB (source address from the zigbee device)

#### Address mode field

Address mode is 1-byte long. Mode value is defined as follow. Mode value 0: the address field is a unicast address. Mode value 1: the address field is a group address.

#### Endpoint field

For “Device and Network Management Service”, the “Endpoint” filed will not present. For “Application Management Service”, the “Endpoint” field should present for specific endpoint and its application.

#### Parameter field

The parameter is variable bytes and used for command to configure the devices. The following command description has more detail information.

#### Checksum field

The checksum is 1-byte long and to confirm the received data correctly. Its value is bitwise not(~) of the sum of all command data fields but header field excluded. Checksum value = ~(length[0]+command id[0]+ command id[1]+ command id[2]+ command id[3]+address[0]+address[1]+address mode[0]+enpoint[0]+ parameter[0]+parameter[1]+…….+parameter[n-1]).

### 3.2 Command Example

Assume host sends a “Device and Network Management Service” command to group devices with group address is 0x5566, command id is 0x12005678, parameter is 2-octet short integer 0x3567. The command data should be formatted as flowing byte stream in little endian style. The endpoint field is not presented.

Header: 0xFF 0xFC 0xFC 0xFF

Length: 0x09

Command id: 0x78 0x56 0x00 0x12

Address: 0x66 0x55

Address Mode: 0x01

Parameters: 0x67 0x35

Checksum: ~(0x09+0x78+0x56+0x00+0x12+0x66+0x55+0x01+0x67+0x35) =

0xBE Then the command should be {0xFF 0xFC 0xFC 0xFF 0x09 0x78 0x56 0x00 0x12 0x66 0x55 0x01 0x67 0x35 0xBE}

Assume host sends an “Application Management Service” command to single device with address is 0x5566, command id is 0x12005678, endpoint is 0x0c, parameter is 2-octet short integer 0x3567. The command data should be formatted as flowing byte stream in little endian style. The endpoint field must be presented.

Header: 0xFF 0xFC 0xFC 0xFF

Length: 0x0A

Command id: 0x78 0x56 0x00 0x12

Address: 0x66 0x55

Address Mode: 0x00

Endpoint: 0x0C

Parameters: 0x67 0x35

Checksum: ~(0x0A+0x78+0x56+0x00+0x12+0x66+0x55+0x00+0x0C+0x67+0x35) = 0xB2 Then the command should be {0xFF 0xFC 0xFC 0xFF 0x0A 0x78 0x56 0x00 0x12 0x66 0x55 0x00 0x0C 0x67 0x35 0xB2}

## 4. Device and Network Management Service

### 4.1 Device and Service Discovery

#### Network address request (0x0000-0000)

The network address request is generated for wishing to inquire as to the 16-bit address of the Remote Device based on its known IEEE address. The destination addressing on this command shall be unicast or broadcast to all devices for which macRxOnWhenIdle = TRUE.

**Command Id**

`0x0000-0000`

**Parameter**

| 8 octets | 1 octet | 1 octet |
|---|---|---|
| IEEEAddr | RequestType | StartIndex |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| IEEEAddr | IEEE Address | A valid 64-bit<br>IEEE address | The IEEE address<br>to be matched by<br>the Remote<br>Device |
| RequestType | Integer | 0x00-0xff | Request type for<br>this command:<br>0x00 – Single<br>device response<br>0x01 – Extended<br>response<br>0x02-0xFF –<br>reserved |
| StartIndex | Integer | 0x00-0xff | If the Request<br>type for this<br>command is<br>Extended<br>response, the<br>StartIndex<br>provides the<br>starting index for<br>the requested<br>elements of the<br>associated<br>devices list |

Request type: Single device response

A NWK_addr_resp command shall be generated and sent back to the local device with the Status field set to SUCCESS, the IEEEAddrRemoteDev field set to the IEEE address of the request; the NWKAddrRemoteDev field set to the NWK address of the discovered device; and the NumAssocDev, StartIndex, and NWKAddrAssocDevList fields shall not be included.

Request type: Extended response

The Remote Device is either the ZigBee coordinator or router, a NWK_addr_resp command shall be generated and sent back to the local device with the Status field set to SUCCESS, the IEEEAddrRemoteDev field set to the IEEE address of the device itself, and the NWKAddrRemoteDev field set to the NWK address of the device itself. The Remote Device shall also supply a list of all 16-bit NWK addresses in the NWKAddrAssocDevList field, starting with the entry StartIndex and continuing with whole entries until the packet maximum length reached.

#### Network address response (0x0000-8000)

The network address response is a Remote Device in response to a network address request command inquiring as to the NWK address of the Remote Device or the NWK address of an address held in the neighbor table

**Command id**

`0x0000-8000`

**Parameter**

| 1 octet | 8 octets | 2 octets | 0/1 octet | 0/1 octet | variable |
|---|---|---|---|---|---|
| Status | IEEEAddr<br>RemoteDev | NWKAddr<br>RemoteDev | Num<br>AssocDev | StartIndex | NWKAddr<br>AssocDevList |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | SUCCESS,<br>INV_REQUESTTYPE,<br>or<br>DEVICE_NOT_FOUND | The status of<br>the<br>NWK_addr_req<br>command. |
| IEEEAddrRemoteDev | Device<br>Address | An extended 64-bit,<br>IEEE address | 64-bit address<br>for the Remote<br>Device |
| NWKAddrRemoteDev | Device<br>Address | A 16-bit, NWK address | 16-bit address<br>for the Remote<br>Device |
| NumAssocDev | Integer | 0x00-0xff | Count of the<br>number of 16-bit<br>short addresses<br>to follow. If the<br>RequestType in<br>the request is<br>Extended<br>Response and<br>there are no<br>associated<br>devices on the<br>Remote Device,<br>this field shall<br>be set to 0. If an<br>error occurs or<br>the Request<br>Type in the<br>request is for a<br>Single Device<br>Response, this<br>field shall not be<br>included in the<br>frame. |
| StartIndex | Integer | 0x00-0xff | Starting index<br>into the list of<br>associated<br>devices for this<br>report. If the<br>RequestType in<br>the request is<br>Extended<br>Response and<br>there are no<br>associated<br>devices on the<br>Remote Device,<br>this field shall<br>not be included<br>in the frame. If<br>an error occurs<br>or the Request<br>Type in the<br>request is for a<br>Single Device<br>Response, this<br>field shall not be<br>included in the<br>frame. |
| NWKAddrAssocDevList | Device<br>Address<br>Lis | List of NumAssocDev<br>16-bit short addresses,<br>each with range<br>0x0000 - 0xffff | A list of 16-bit<br>addresses, one<br>corresponding<br>to each<br>associated<br>device to<br>Remote Device;<br>The number of<br>16-bit network<br>addresses<br>contained in this<br>field is specified<br>in the<br>NumAssocDev<br>field. If the<br>RequestType in<br>the request is<br>Extended |

| |
|---|
| Response and<br>there are no<br>associated<br>devices on the<br>Remote Device,<br>this field shall<br>not be included<br>in the frame. If<br>an error occurs<br>or the Request<br>Type in the<br>request is for a<br>Single Device<br>Response, this<br>field shall not be<br>included in the<br>frame. |

#### IEEE address request (0x0000-0001)

The IEEE address request is generated for wishing to inquire as to the 64-bit IEEE address of the Remote Device based on their known 16-bit address. The destination addressing on this command shall be unicast or broadcast to all devices for which macRxOnWhenIdle = TRUE.

**Command id**

`0x0000-0001`

**Parameter**

| 2 octets | 1 octet | 1 octet |
|---|---|---|
| NWKAddrOfInterest | RequestType | StartIndex |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| NWKAddrOfInterest | Device Address | 16-bit NWK<br>address | NWK address that<br>is used for IEEE<br>address mapping |
| RequestType | Integer | 0x00-0xff | Request type for<br>this command:<br>0x00 – Single<br>device response<br>0x01 – Extended<br>response<br>0x02-0xFF –<br>reserved |
| StartIndex | Integer | 0x00-0xff | If the Request<br>type for this<br>command is<br>Extended<br>response, the<br>StartIndex<br>provides the<br>starting index for<br>the requested<br>elements of the<br>associated<br>devices list |

#### IEEE address response (0x0000-8001)

The IEEE address response is in response to an IEEE address request command inquiring as to the 64-bit IEEE address of the Remote Device or the 64-bit IEEE address of an address held in the neighbor table.

**Command Id**

`0x0000-8001`

**Parameter**

| 1 octet | 8 octets | 2 octets | 0/1 octet | 0/1 octet | variable |
|---|---|---|---|---|---|
| Status | IEEEAddr<br>RemoteDev | NWKAddr<br>RemoteDev | Num<br>AssocDev | StartIndex | NWKAddr<br>AssocDevList |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | SUCCESS,<br>INV_REQUESTTYPE,<br>or<br>DEVICE_NOT_FOUND | The status of<br>the<br>NWK_addr_req<br>command. |
| IEEEAddrRemoteDev | Device<br>Address | An extended 64-bit,<br>IEEE address | 64-bit address<br>for the Remote<br>Device |
| NWKAddrRemoteDev | Device<br>Address | A 16-bit, NWK address | 16-bit address<br>for the Remote<br>Device |
| NumAssocDev | Integer | 0x00-0xff | Count of the<br>number of 16-bit<br>short addresses<br>to follow. If the<br>RequestType in<br>the request is<br>Extended<br>Response and<br>there are no<br>associated<br>devices on the<br>Remote Device,<br>this field shall<br>be set to 0. If an<br>error occurs or<br>the Request<br>Type in the<br>request is for a<br>Single Device<br>Response, this<br>field shall not be<br>included in the<br>frame. |
| StartIndex | Integer | 0x00-0xff | Starting index<br>into the list of<br>associated<br>devices for this<br>report. If the<br>RequestType in<br>the request is<br>Extended<br>Response and<br>there are no<br>associated<br>devices on the<br>Remote Device,<br>this field shall<br>not be included<br>in the frame. If<br>an error occurs<br>or the Request<br>Type in the<br>request is for a<br>Single Device<br>Response, this<br>field shall not be<br>included in the<br>frame. |
| NWKAddrAssocDevList | Device<br>Address<br>Lis | List of NumAssocDev<br>16-bit short addresses,<br>each with range<br>0x0000 - 0xffff | A list of 16-bit<br>addresses, one<br>corresponding<br>to each<br>associated<br>device to<br>Remote Device;<br>The number of<br>16-bit network<br>addresses<br>contained in this<br>field is specified |

| |
|---|
| in the<br>NumAssocDev<br>field. If the<br>RequestType in<br>the request is<br>Extended<br>Response and<br>there are no<br>associated<br>devices on the<br>Remote Device,<br>this field shall<br>not be included<br>in the frame. If<br>an error occurs<br>or the Request<br>Type in the<br>request is for a<br>Single Device<br>Response, this<br>field shall not be<br>included in the<br>frame. |

#### Node descriptor request (0x0000-0002)

The Node descriptor request command is generated for wishing to inquire as to the node descriptor of a remote device. This command shall be unicast either to the remote device itself or to an alternative device that contains the discovery information of the remote device.

**Command id**

`0x0000-0002`

**Parameter**

| 2 octets |
|---|
| NWKAddrOfInterest |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| NWKAddrOfInterest | Device Address | 16-bit NWK<br>address | NWK address for<br>the request |

#### Node descriptor response (0x0000-8002)

The node descriptor response is in response to a node descriptor request directed to the remote device. This command shall be unicast to the originator of the node descriptor request command.

**Command id**

0x0000-80002

**Parameter**

| 1 octet | 2 octets | Variable |
|---|---|---|
| Status | NWKAddrOfInterest | Node Descriptor |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | SUCCESS,<br>DEVICE_NOT_FOUND,<br>INV_REQUESTTYPE,<br>or NO_DESCRIPTOR | The status of the<br>command |
| NWKAddrOfInterest | Device<br>Address | 16-bit NWK address | NWK address for<br>the request |
| NodeDescriptor | Node<br>Descriptor |  | This field shall<br>only be included<br>in the frame if the<br>status field is<br>equal to<br>SUCCESS |

Node Descriptor The node descriptor contains information about the capabilities of the ZigBee node and is mandatory for each node. There shall be only one node descriptor in a node.

| Field Name<br>Logical type<br>Complex descriptor available<br>User descriptor available<br>Reserved<br>APS flags<br>Frequency band<br>MAC capability flags<br>Manufacturer code<br>Maximum buffer size<br>Maximum incoming transfer size<br>Server mask<br>Maximum outgoing transfer size<br>Descriptor capability field | Length(bits) |
|---|---|

#### Power descriptor request (0x0000-0003)

The Power descriptor request command is generated for wishing to inquire as to the power descriptor of a remote device. This command shall be unicast either to the remote device itself or to an alternative device that contains the discovery information of the remote device.

**Command id**

`0x0000-0003`

**Parameter**

| 2 octets |
|---|
| NWKAddrOfInterest |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| NWKAddrOfInterest | Device Address | 16-bit NWK<br>address | NWK address for<br>the request |

#### Power descriptor response (0x0000-8003)

The power descriptor response is in response to a power descriptor request directed to the remote device. This command shall be unicast to the originator of the power descriptor request command.

**Command id**

`0x0000-8003`

**Parameter**

| 1 octet | 2 octets | Variable |
|---|---|---|
| Status | NWKAddrOfInterest | Power Descriptor |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | SUCCESS,<br>DEVICE_NOT_FOUND,<br>INV_REQUESTTYPE,<br>or NO_DESCRIPTOR | The status of the<br>command |
| NWKAddrOfInterest | Device<br>Address | 16-bit NWK address | NWK address for<br>the request |
| PowerDescriptor | Power<br>Descriptor |  | This field shall<br>only be included<br>in the frame if the<br>status field is<br>equal to<br>SUCCESS |

Power Descriptor The node power descriptor gives a dynamic indication of the power status of the node and is mandatory for each node. There shall be only one node power descriptor in a node.

| Field Name<br>Current power mode<br>Available power sources<br>Current power source<br>Current power source level | Length(bits) |
|---|---|

#### Simple descriptor request (0x0000-0004)

The Simple descriptor request command is generated for wishing to inquire as to the simple descriptor of a remote device on a specified endpoint. This command shall be unicast either to the remote device itself or to an alternative device that contains the discovery information of the remote device.

**Command id**

`0x0000-0004`

**Parameter**

| 2 octets | 1 octet |
|---|---|
| NWKAddrOfInterest | EndPoint |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| NWKAddrOfInterest | Device Address | 16-bit NWK<br>address | NWK address for<br>the request |
| Endpoint | 8 bits | 1-254 | The endpoint on<br>the destination |

#### Simple descriptor response (0x0000-8004)

The simple descriptor response is in response to a simple descriptor request directed to the remote device. This command shall be unicast to the originator of the simple descriptor request command.

**Command id**

0x0000-80004

**Parameter**

| 1 octet | 2 octets | 1 octet | Variable |
|---|---|---|---|
| Status | NWKAddrOfInterest | Length | Simple Descriptor |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | SUCCESS,<br>DEVICE_NOT_FOUND,<br>INV_REQUESTTYPE,<br>or NO_DESCRIPTOR | The status of the<br>command |
| NWKAddrOfInterest | Device<br>Address | 16-bit NWK address | NWK address for<br>the request |
| Length | Integer | 0x00-0xff | Length in bytes of<br>the Simple<br>Descriptor to<br>follow. |
| SimpleDescriptor | Simple<br>Descriptor |  | This field shall<br>only be included<br>in the frame if the<br>status field is<br>equal to<br>SUCCESS |

Simple Descriptor The simple descriptor contains information specific to each endpoint contained in this node. The simple descriptor is mandatory for each endpoint present in the node.

| Field Name<br>Endpoint<br>Application profile identifier<br>Application device identifier<br>Application device version<br>Reserved<br>Application input cluster count | Length(bits) |
|---|---|
| Application input cluster list<br>Application output cluster count | 16*i (where i is<br>the value of the<br>application input<br>cluster count) |
| Application output cluster list | 16*o (where o is<br>the value of the<br>application output<br>cluster count) |

#### Active endpoint request (0x0000-0005)

The Active endpoint request command is generated for wishing to acquire the list of endpoints on a remote device with simple descriptors. This command shall be unicast either to the remote device itself or to an alternative device that contains the discovery information of the remote device.

**Command id**

`0x0000-0005`

**Parameter**

| 2 octets |
|---|
| NWKAddrOfInterest |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| NWKAddrOfInterest | Device Address | 16-bit NWK<br>address | NWK address for<br>the request |

#### Active endpoint response (0x0000-8005)

The active endpoint response is in response to an active endpoint request directed to the remote device. This command shall be unicast to the originator of the active endpoint request command.

**Command id**

`0x0000-8005`

**Parameter**

| 1 octet | 2 octets | 1 octet | Variable |
|---|---|---|---|
| Status | NWKAddrOfInterest | ActiveEPCount | ActiveEPList |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | SUCCESS,<br>DEVICE_NOT_FOUND,<br>INV_REQUESTTYPE,<br>or NO_DESCRIPTOR | The status of the<br>command |
| NWKAddrOfInterest | Device<br>Address | 16-bit NWK address | NWK address for<br>the request |
| ActiveEPCount | Integer | 0x00-0xff | The count of<br>active endpoints<br>on the Remote<br>Device. |
| ActiveEPList |  |  | List of bytes each<br>of which<br>represents an 8-<br>bit endpoint |

#### Device announce indication (0x0000-0013)

The Device announce indication is provided to notify upper layer that the device has joined or re-joined the network, identifying the device’s 64-bit IEEE address and new 16-bit NWK address, and informing the Remote Devices of the capability of the ZigBee device .

**Command id**

`0x0000-0013`

**Parameter**

| 2 octets | 8 octets | 1 octet |
|---|---|---|
| NWKAddr | IEEEAddr | Capability |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| NWKAddr | Device Address | 16-bit NWK | NWK address for |
|  |  | address | the Local Device |
| IEEEAddr | Device Address | 64-bit IEEE<br>address | IEEE address for<br>the Local Device |
| Capability | Bitmap |  | Capability of the<br>local device |

MAC Capability Flags Field

| Bit 0 | Bit 1 | Bit 2 | Bit 3 | Bit 4-5 | Bit 6 | Bit 7 |
|---|---|---|---|---|---|---|
| Alternate<br>PAN<br>coordinator | Device<br>type | Power<br>source | Receiver<br>on when<br>idle | Reserved | Security<br>capability | Allocate<br>address |

Bit 0: 1, node is capable of becoming a PAN coordinator. Otherwise, 0. Bit 1: 1, node is full function device(FFD). 0, node is reduced function device. Bit 2: 1, the power source is mains power. Otherwise, 0. Bit 3: 1, the device does not disable its receiver to conserve power during idle periods. Otherwise, 0. Bit 6: 1, the device is capable of sending and receiving frames secured using the security suite specified in IEEE 802.15.4-2015. Otherwise, 0. Bit 7: 1, the device is wishing to allocate a network address. Otherwise, 0.

#### Device leave indication (0x0000-0014)

The Device leave indication is provided to notify upper layer that the device has permanently leave current network. .

**Command id**

`0x0000-0014`

**Parameter**

| 2 octets |
|---|
| NWKAddr |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| NWKAddr | Device Address | 16-bit NWK<br>address | NWK address of<br>the Leaved<br>Device |

### 4.2 Device Bind Management

#### Bind request (0x0000-0021)

The bind request is for wishing to create a Binding Table entry for the source and destination addresses contained as parameters. The destination addressing on this command shall be unicast only.

**Command id**

`0x0000-0021`

**Parameter**

| 8 octets | 1 octet | 2 octets | 1 octet | 2/8 octets | 0/1 octet |
|---|---|---|---|---|---|
| SrcAddress | SrcEndp | ClusterID | DstAddrMode | DstAddress | DstEndp |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| SrcAddress | IEEE Address | A valid 64-bit<br>IEEE address | The IEEE address<br>for the source. |
| SrcEndp | Integer | 0x01-0xfe | The source<br>endpoint for the<br>binding entry. |
| ClusterID | Integer | 0x0000-0xffff | The identifier of<br>the cluster on the<br>source device that<br>is bound to the<br>destination |
| DstAddrMode | Integer | 0x00-0xff | The addressing<br>mode for the<br>destination<br>address used in<br>this command.<br>This field can take<br>one of the non-<br>reserved values<br>from the following<br>list:<br>0x00 = reserved<br>0x01 = 16-bit<br>group address for<br>DstAddress and<br>DstEndp not<br>present<br>0x02 = reserved<br>0x03 = 64-bit<br>extended address<br>for DstAddress<br>and DstEndp<br>present<br>0x04 – 0xff =<br>reserved |
| DstAddress | Address | As specified by<br>the DstAddrMode<br>field | The destination<br>address for the<br>binding entry. |
| DstEndp | Integer | 0x01-0xfe | This field shall be<br>present only if the<br>DstAddrMode field<br>has a value of<br>0x03 and, if<br>present, shall be<br>the destination<br>endpoint for the<br>binding entry |

#### Bind response (0x0000-8021)

The bind response is in response to a bind request. If the bind request is processed and the Binding Table entry committed on the Remote Device, a Status of SUCCESS is returned.

**Command id**

`0x0000-8021`

**Parameter**

| 1 octet |
|---|
| Status |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | SUCCESS,<br>NOT_SUPPORTED,<br>INVALID_EP,<br>TABLE_FULL or<br>NOT_AUTHORIZED | The status of the<br>command |

#### Unbind request (0x0000-0022)

The unbind request is for wishing to remove a Binding Table entry for the source and destination addresses contained as parameters. The destination addressing on this command shall be unicast only.

**Command id**

`0x0000-0022`

**Parameter**

| 8 octets | 1 octet | 2 octets | 1 octet | 2/8 octets | 0/1 octet |
|---|---|---|---|---|---|
| SrcAddress | SrcEndp | ClusterID | DstAddrMode | DstAddress | DstEndp |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| SrcAddress | IEEE Address | A valid 64-bit<br>IEEE address | The IEEE address<br>for the source. |
| SrcEndp | Integer | 0x01-0xfe | The source<br>endpoint for the<br>binding entry. |
| ClusterID | Integer | 0x0000-0xffff | The identifier of<br>the cluster on the<br>source device that<br>is bound to the<br>destination |
| DstAddrMode | Integer | 0x00-0xff | The addressing<br>mode for the<br>destination<br>address used in<br>this command.<br>This field can take<br>one of the non-<br>reserved values<br>from the following<br>list:<br>0x00 = reserved<br>0x01 = 16-bit<br>group address for<br>DstAddress and<br>DstEndp not<br>present<br>0x02 = reserved<br>0x03 = 64-bit<br>extended address<br>for DstAddress<br>and DstEndp<br>present<br>0x04 – 0xff =<br>reserved |
| DstAddress | Address | As specified by<br>the DstAddrMode<br>field | The destination<br>address for the<br>binding entry. |
| DstEndp | Integer | 0x01-0xfe | This field shall be |

| |
|---|
| present only if the<br>DstAddrMode field<br>has a value of<br>0x03 and, if<br>present, shall be<br>the destination<br>endpoint for the<br>binding entry |

#### Unbind response (0x0000-8022)

The unbind response is in response to an unbind request. If the unbind request is processed and the corresponding Binding Table entry is removed from the Remote Device, a Status of SUCCESS is returned.

**Command id**

`0x0000-8022`

**Parameter**

| 1 octet |
|---|
| Status |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | SUCCESS,<br>NOT_SUPPORTED,<br>INVALID_EP,<br>TABLE_FULL or<br>NOT_AUTHORIZED | The status of the<br>command |

### 4.3 Network Management

#### Device binding information request (0x0000-0033)

The device binding information request is for wishing to retrieve the contents of the Binding Table from the Remote Device. The destination addressing on this command shall be unicast only.

**Command id**

`0x0000-0033`

**Parameter**

| 1 octet |
|---|
| StartIndex |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| StartIndex | Integer | 0x00-0xff | Starting Index for<br>the requested<br>elements of the<br>Binding Table. |

#### Device binding information response (0x0000-8033)

The device binding information response is in response to a device binding information request. If this management command is not supported, a status of NOT_SUPPORTED shall be returned and all parameter fields after the Status field shall be omitted.

**Command id**

`0x0000-8033`

**Parameter**

| 1 octet | 1 octet | 1 octet | 1 octet | variable |
|---|---|---|---|---|
| Status | BindingTable<br>Entries | Start Index | BindingTable<br>ListCount | BindingTable<br>List |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | NOT_SUPPORTED<br>or any status code | The status of the<br>command |
| BindingTableEntries | Integer | 0x00-0xff | Total number of<br>Binding Table entries<br>within the Remote<br>Device. |
| StartIndex | Integer | 0x00-0xff | Starting index within<br>the Binding Table to<br>begin reporting for<br>the BindingTableList. |
| BindingTableListCou<br>nt | Integer | 0x00-0xff | Number of Binding<br>Table entries<br>included within<br>BindingTableList |
| BindingTableList | List of<br>Binding<br>Descriptor<br>s | The list shall contain<br>the number elements<br>given by the<br>BindingTableListCou<br>nt | A list of descriptors,<br>beginning with the<br>StartIndex element<br>and continuing for<br>BindingTableListCou<br>nt, of the elements in<br>the Remote Device's<br>Binding Table |

BindingTableList Record Format.

| Name | Type | Valid Range | Description |
|---|---|---|---|
| SrcAddr | IEEE<br>address | A valid 64-bit IEEE<br>address | The source IEEE<br>address for the<br>binding entry. |
| SrcEndpoint | Integer | 0x01-0xfe | The source<br>endpoint for the<br>binding entry. |
| ClusterId | Integer | 0x0000-0xffff | The identifier of the<br>cluster on the<br>source device that<br>is bound to the<br>destination de-vice. |
| DstAddrMode | Integer | 0x00-0xff | The addressing<br>mode for the<br>destination<br>address. This field<br>can take one of the<br>non-reserved<br>values from the<br>following list:<br>0x00 = reserved<br>0x01 = 16-bit group<br>address for<br>DstAddr and<br>DstEndpoint not<br>present<br>0x02 = reserved<br>0x03 = 64-bit<br>extended address<br>for DstAddr and<br>DstEndp present<br>0x04 – 0xff =<br>reserved |
| DstAddr | Address | As specified by the<br>DstAddr-Mode field | The destination<br>address for the<br>binding entry. |
| DstEndpoint | Integer | 0x01-0xff | This field shall be<br>present only if the<br>DstAddrMode field<br>has a value of 0x03<br>and, if present,<br>shall be the<br>destination<br>endpoint for the<br>binding entry. |

#### Device leave request (0x0000-0034)

The device leave request is for requesting that a Remote Device leave the network.

**Command id**

`0x0000-0034`

**Parameter**

| 8 octets | 1 octet | 1 octet |
|---|---|---|
| Device Address | Remove Children | Rejoin |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DeviceAddress | Device Address | An extended 64-<br>bit, IEEE address | Device IEEE<br>address |
| Remove Children | Bool | 0/1 | This field has a<br>value of 1 if the<br>device being<br>asked to leave the<br>network is also<br>being asked to<br>remove its child<br>devices, if any.<br>Otherwise, it has<br>a value of 0. |
| Rejoin | Bool | 0/1 | This field has a<br>value of 1 if the<br>device being<br>asked to leave<br>from the current<br>parent is<br>requested to<br>rejoin the network.<br>Otherwise, it has<br>a value of 0. |

#### Device leave response (0x0000-8034)

The device leave response is in response to a device leave request. If this management command is not supported, a status of NOT_SUPPORTED shall be returned.

**Command id**

`0x0000-8034`

**Parameter**

| 1 octet |
|---|
| Status |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | NOT_SUPPORTED,<br>NOT_AUTHORIZED or<br>any status code | The status of the<br>command |

#### Permit join request (0x0000-0036)

The permit join request is requesting that a remote device or devices allow or disallow association. If the remote device is the Trust Center and TC_Significance is set to 1, the Trust Center authentication policy will be affected. The addressing may be unicast or broadcast to all routers for request remote device, unicast with address value 0x0000 to request Zigbee coordinator enable permit duration.

**Command id**

`0x0000-0036`

**Parameter**

| 1 octet | 1 octet |
|---|---|
| PermitDuration | TC_Significance |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| PermitDuration | Integer | 0x00-0xff | The length of time<br>in seconds during<br>which the ZigBee<br>coordinator or<br>router will allow<br>associations. The<br>value 0x00<br>indicate that<br>permission is<br>disabled. |
| TC_Significance | Bool | 0/1 | This field shall<br>always have a<br>value of 1,<br>indicating a<br>request to change<br>the Trust Center<br>policy. If a frame<br>is received with a<br>value of 0, it shall<br>be treated as<br>having a value of<br>1. |

#### Permit join response (0x0000-8036)

The permit join response is in response to a unicast permit join request. In the description which follows, note that no response shall be sent if the permit join request was received as a broadcast to all routers.

**Command id**

`0x0000-8036`

**Parameter**

| 1 octet |
|---|
| Status |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | SUCCESS,<br>INVALID_REQUEST,<br>NOT_AUTHORIZED or<br>any status code | The status of the<br>command |

#### Permit join timeout notification (0x0000-8037)

When the user receives this command, it indicates that the previously executed permit join request has expired, and the device currently has its permit join set to disabled.

**Command id**

`0x0000-8037`

**Parameter**

None

#### Network update request (0x0000-0038)

This command is provided to allow updating of network configuration parameters or to request information from devices on network conditions in the local operating environment. The destination addressing on this primitive shall be unicast or broadcast to all devices for which macRxOnWhenIdle = TRUE.

**Command id**

`0x0000-0038`

**Parameter**

| 4 octets | 1 octet | 0/1 octet | 0/1 octet | 0/2 octet |
|---|---|---|---|---|
| ScanChannels | ScanDuration | ScanCount | nwkUpdateId | nwkManagerAddr |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| ScanChannels | Bitmap | 32-bit field | The five most<br>significant bits<br>(b27,..., b31)<br>represent the binary<br>encoded Channel<br>Page. The 27 least<br>significant bits (b0,<br>b1,... b26) indicate<br>which channels are<br>to be scanned (1 =<br>scan, 0 = do not<br>scan) for each of the<br>27 valid channels |
| ScanDuration | Integer | 0x00-0x05 or<br>0xfe or<br>0xff | 0x00-0x05: A value<br>used to calculate the<br>length of time to<br>spend scanning<br>each channel.<br>If ScanDuration has<br>a value of 0xfe this<br>is a request for<br>channel change.<br>If ScanDuration has<br>a value of 0xff this is<br>a request to change<br>the<br>apsChannelMaskList<br>and<br>nwkManagerAddr<br>attributes. |
| ScanCount | Integer | 0x00-0x01 | This field represents<br>the number of<br>energy scans to be<br>conducted and<br>reported. This field<br>shall be present only<br>if the ScanDuration<br>is within the range of<br>0x00 to 0x05. |
| nwkUpdateId | Integer | 0x00 - 0xFF | The value of the<br>nwkUpdateId<br>contained in this<br>request. This value<br>is set by the Network<br>Channel Manager<br>prior to sending the<br>message. This field<br>shall only be present<br>of the ScanDuration<br>is 0xfe or 0xff. If the<br>ScanDuration is 0xff,<br>then the value in the<br>nwkUpdateID shall<br>be ignored. |
| nwkManagerAddr | Device Address | 16-bit NWK<br>address | This field shall be<br>present only if the<br>ScanDuration is set<br>to 0xff, and, where<br>present, indicates<br>the NWK address for<br>the device with the<br>Network Manager bit<br>set in its Node<br>Descriptor. |

#### Network update notify (0x0000-8038)

The network update notify is provided to enable ZigBee devices to report the condition on local channels to a network manager. When sent in response to a network update request command the status field shall represent the status of the request.

**Command id**

`0x0000-8038`

**Parameter**

| 1 octet | 4 octets | 2 octets | 2 octets | 1 octet | variable |
|---|---|---|---|---|---|
| Status | Scanned<br>Channels | TotalTransmi<br>ssions | TransmissionFai<br>lures | ScannedCha<br>nnelsListCou<br>nt | EnergyVa<br>lues |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | SUCCESS,<br>INVALID_REQUEST,<br>NOT_SUPPORTED<br>or any status values | The status of<br>this command |
| ScanChannels | Bitmap | 32-bit field | The five most<br>significant bits<br>(b27,..., b31)<br>represent the<br>binary<br>encoded<br>Channel Page.<br>The 27 least<br>significant bits<br>(b0, b1,... b26)<br>indicate which<br>channels are<br>to be scanned<br>(1 = scan, 0 =<br>do not scan)<br>for each of the<br>27 valid<br>channels |
| TotalTransmissions | Integer | 0x0000 -0xffff | Count of the<br>total<br>transmissions<br>reported by the<br>device |
| TransmissionFailures | Integer | 0x0000 -0xffff | Sum of the<br>total<br>transmission<br>failures<br>reported by the<br>device |
| ScannedChannelsListCount | Integer | 0x00 - 0xff | The list shall<br>contain the<br>number of<br>records<br>contained in<br>the<br>EnergyValues<br>parameter. |
| EnergyValues | Integer | List of ED values<br>each of which can<br>be in the range of<br>0x00 - 0xff | The result of<br>an energy<br>measurement<br>made on this<br>channel |

#### Gateway start (0x0000-0039)

Start the Gateway to from a network.

Please be noted that when constructing this command, the “Address” and “Address mode” fields should set to 0x0000 and 0x00, and it should not contain the “Endpoint” information in the command struct.

**Command id**

`0x0000-0039`

**Parameter**

| 1 octet | 2 octets | 1 octet |
|---|---|---|
| Channel (11-26) | PanID | ResetFlag (0/1) |

#### Gateway start response (0x0000-8039)

The gateway start response is in response to a gateway start. If this command is sent before, a status of FAILURE shall be returned.

**Command id**

`0x0000-8039`

**Parameter**

| 1 octet |
|---|
| Status |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | SUCCESS, FAILURE | The status of the<br>command |

#### Channel energy scan (0x0000-003A)

The user can use this command to check the channel status before the Gateway starting to from a zigbee network. This command performs the energy scan for each zigbee channel from CH11(2405 MHz), CH12(2410 MHz) …. CH26(2480 MHz), and based on the result of Channel energy scan response (0x0000-803A) to help the user to select the suitable channel to start the zigbee network.

Please be noted that this command can only be performed before starting the zigbee network.

**Command id**

`0x0000-003A`

**Parameter**

None

#### Channel energy scan response(0x0000-803A)

This command returns the result of executing the "Channel energy scan" command. The response returns the energy level obtained for each Zigbee channel. This value provides a reference for the user on the energy distribution of the channels in the current environment. Please note that the smaller the value, the greater the energy of the corresponding channel. It is recommended to select the channel with the largest value(channel with lower energy distribution) as the channel to be used for the Zigbee network.

**Command id**

`0x0000-803A`

**Parameter**

| 1 octet | 1 octet | 1 octet | 1 octet | … | 1 octet | 1 octet | 1 octet |
|---|---|---|---|---|---|---|---|
| Status | CH11 | CH12 | CH13 | … | CH24 | CH25 | CH26 |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | 0x00 ~ 0xFF | 0x00: energy scan succeed<br>Others: energy scan failed |
| CH11 | Integer | 0x00 ~ 0xFF | Channel 11(2405 MHz) energy level |
| CH12 | Integer | 0x00 ~ 0xFF | Channel 12(2410 MHz) energy level |
| CH13 | Integer | 0x00 ~ 0xFF | Channel 13(2415 MHz) energy level |
| CH14 | Integer | 0x00 ~ 0xFF | Channel 14(2420 MHz) energy level |
| CH15 | Integer | 0x00 ~ 0xFF | Channel 15(2425 MHz) energy level |
| CH16 | Integer | 0x00 ~ 0xFF | Channel 16(2430 MHz) energy level |
| CH17 | Integer | 0x00 ~ 0xFF | Channel 17(2435 MHz) energy level |
| CH18 | Integer | 0x00 ~ 0xFF | Channel 18(2440 MHz) energy level |
| CH19 | Integer | 0x00 ~ 0xFF | Channel 19(2445 MHz) energy level |
| CH20 | Integer | 0x00 ~ 0xFF | Channel 20(2450 MHz) energy level |
| CH21 | Integer | 0x00 ~ 0xFF | Channel 21(2455 MHz) energy level |
| CH22 | Integer | 0x00 ~ 0xFF | Channel 22(2460 MHz) energy level |
| CH23 | Integer | 0x00 ~ 0xFF | Channel 23(2465 MHz) energy level |
| CH24 | Integer | 0x00 ~ 0xFF | Channel 24(2470 MHz) energy level |
| CH25 | Integer | 0x00 ~ 0xFF | Channel 25(2475 MHz) energy level |
| CH26 | Integer | 0x00 ~ 0xFF | Channel 26(2480 MHz) energy level |

#### Network address table update (0x0000-003B)

This command is used to obtain the network addresses of each node currently present in the Zigbee network. This information will be updated in the address list table. When the user wants to obtain the network address information of the current network nodes, they must first use this command (0x0000003B) to obtain the latest network address information of the current network and then use 0x0000003C command to obtain the network address table content.

Please refer to the latter part of the 'Get network address list response (0x0000- 803C)' section, which provides relevant usage examples.

**Command id**

`0x0000-003B`

**Parameter**

None

#### Network address table update response (0x0000-803B)

After the device executes the 0x0000003B command, it will return the status of the command execution, including the Addr_count and Group_count. When Addr_count and Group_count are greater than 0, the user can use this information along with the 0x0000003C command to obtain the network address of each node.

**Command id**

`0x0000-803B`

**Parameter**

| 1 octet | 1 octet | 1 octet |
|---|---|---|
| Status | Addr_count | Group_count |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | 0x00 ~ 0xFF | 0x00: address table update succeeded<br>Others: address table update failed |
| Addr_count | Integer | 0x00 ~ 0xFF | Current number of network nodes. |
| Group_count | Integer | 0x00 ~ 0xFF | Current number of groups, each group<br>can record up to 10 network addresses. |

#### Get network address list (0x0000-003C)

When the user updates the network address table using the 0x0000003B command, they can determine the current number of network nodes based on Addr_count. By using the content of Group_idx along with the command 0x0000- 003C from this section, they can obtain the network address list. Each time this command is executed, up to 10 network addresses can be retrieved. For example, when using the 0x0000003B command to get Addr_count=0x0C and Group_count=0x02, it means there are 12 nodes in the network (with 12 network addresses). You can use the 0x0000003C command with Group_idx=1 to obtain the first 10 network addresses, and use the 0x0000003C command with Group_idx=2 to obtain the remaining 2 network addresses.

**Command id**

`0x0000-003C`

**Parameter**

| 1 octet |
|---|
| Group_idx |

#### Get network address list response (0x0000-803C)

Using this response message, users can retrieve the network address list corresponding to the specified Group_idx.

**Command id**

`0x0000-803C`

**Parameter**

| 1 octet | 1 octet | 1 octet | 2 octets | … | 2 octets |
|---|---|---|---|---|---|
| Status | Addr_count | Group_idx | 1st Addr | … | 10th Addr |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | 0x00 ~ 0xFF | 0x00: network address available.<br>Others: no network address or invalid<br>Group_idx value. |
| Addr_count | Integer | 0x00 ~ 0xFF | Current number of network nodes set<br>with Group_idx. |
| Group_idx | Integer | 0x00 ~ 0xFF | Current Group_idx. |
| 1st Addr | Integer | 0x0000 ~ 0xFFF7 | 1st network address, not valid if the<br>address is 0xFFFF |
| 2nd Addr | Integer | 0x0000 ~ 0xFFF7 | 2nd network address, not valid if the<br>address is 0xFFFF |
| 3rd Addr | Integer | 0x0000 ~ 0xFFF7 | 3rd network address, not valid if the<br>address is 0xFFFF |
| 4th Addr | Integer | 0x0000 ~ 0xFFF7 | 4th network address, not valid if the<br>address is 0xFFFF |
| th<br>5 Addr | Integer | 0x0000 ~ 0xFFF7 | th<br>5 network address, not valid if the<br>address is 0xFFFF |
| 6th Addr | Integer | 0x0000 ~ 0xFFF7 | 6th network address, not valid if the<br>address is 0xFFFF |
| 7th Addr | Integer | 0x0000 ~ 0xFFF7 | 7th network address, not valid if the<br>address is 0xFFFF |
| 8th Addr | Integer | 0x0000 ~ 0xFFF7 | 8th network address, not valid if the<br>address is 0xFFFF |
| 9th Addr | Integer | 0x0000 ~ 0xFFF7 | 9th network address, not valid if the<br>address is 0xFFFF |
| 10th Addr | Integer | 0x0000 ~ 0xFFF7 | 10th network address, not valid if the<br>address is 0xFFFF |

Example of getting the current nodes of network addresses with hex command format:

Step1: send Network address table update command FF FC FC FF 07 3B 00 00 00 00 00 00 BD

Step2: got Network address table update response FF FC FC FF 0A 3B 80 00 00 00 00 00 00 02 01 37 00: status succeeded. 02: 2 nodes on the netowk 01: Group count is 1

Step3: get network address list with Group_idx = 1 FF FC FC FF 08 3C 00 00 00 00 00 00 01 BA

Step4: got network address list response FF FC FC FF 1E 3C 80 00 00 00 00 00 00 02 01 00 00 1C A8 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 6E 00 02 01: network address available, 2 network address included, Group_idx=1 00 00: 1st network address 0x0000 1C A8: 2nd network address 0xA81C

#### Gateway reset (0x0000-0040)

Software reset Gateway.

Please be noted that when constructing this command, the “Address” and “Address mode” fields should set to 0x0000 and 0x00, and it should not contain the “Endpoint” information in the command struct.

**Command id**

`0x0000-0040`

**Parameter**

| 1 octet |
|---|
| MagicNumber = 0x88 |

#### Gateway reset response (0x0000-8040)

The gateway start response is in response to a gateway start. If this command is sent before, a status of FAILURE shall be returned.

**Command id**

`0x0000-8040`

**Parameter**

| 1 octet |
|---|
| Status |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | SUCCESS, FAILURE | The status of the<br>command |

#### Gateway extended address request (0x0000-0041)

The Gateway extended address request command is generated for obtaining gateway its own extended address

Please be noted that when constructing this command, the “Address” and “Address mode” fields should set to 0x0000 and 0x00, and it should not contain the “Endpoint” information in the command struct.

**Command id**

`0x0000-0041`

**Parameter**

None

#### Gateway extended address response (0x0000-8041)

The Gateway extended address response is in response to a Gateway extended address request.

**Command id**

`0x0000-8041`

**Parameter**

| 1 octet | 8 octets |
|---|---|
| Status | Extended Address |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Integer | SUCCESS, FAILURE | The status of the<br>command |

#### Gateway permit join status request (0x0000-0042)

The Gateway permit join status request command is generated for obtaining gateway its own association permit status.

Please be noted that when constructing this command, the “Address” and “Address mode” fields should set to 0x0000 and 0x00, and it should not contain the “Endpoint” information in the command struct.

**Command id**

`0x0000-0042`

**Parameter**

None

#### Gateway permit join status response (0x0000-8042)

The Gateway permit join status response is in response to a Gateway permit join status request.

**Command id**

`0x0000-8042`

**Parameter**

| 1 octet | 1 octet |
|---|---|
| Status | Remaining Time |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Boolean | TRUE, FALSE | The status of<br>association permit |
| Remaining Time | Integer | 0x00~0xFE | The remaining time of<br>association permit |

#### Gateway PanID Channel request (0x0000-0043)

The Gateway PanID Channel request command is generated for obtaining the gateway firmed PanID and Channel information.

Please be noted that when constructing this command, the “Address” and “Address mode” fields should set to 0x0000 and 0x00, and it should not contain the “Endpoint” information in the command struct.

**Command id**

`0x0000-0043`

**Parameter**

None

#### Gateway PanID Channel response (0x0000-8043)

The Gateway PanID Channel response is in response to a Gateway PanID Channel request.

**Command id**

`0x0000-8043`

**Parameter**

| 1 octet | 2 octets | 1 octet |
|---|---|---|
| Status | PanID | Channel number |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Boolean | 0x00/0x01 | 0x00: get information<br>success.<br>0x01: get information<br>fail. |
| PanID | Integer | 0x0000~0xFFFF | Network PanID |
| Channel number | Integer | 0x00 ~ 0xFF | 0x0B: channel 11<br>0x0C: channel 12<br>0x0D: channel 13<br>0x0E: channel 14<br>0x0F: channel 15<br>0x10: channel 16<br>0x11: channel 17<br>0x12: channel 18<br>0x13: channel 19<br>0x14: channel 20<br>0x15: channel 21<br>0x16: channel 22<br>0x17: channel 23<br>0x18: channel 24<br>0x19: channel 25<br>0x1A: channel 26 |

#### Gateway Install Code Set request (0x0000-0044)

The Gateway Install Code Set request command is generated for setting the installation code for the device to be joined in the zigbee network.

Please be noted that when constructing this command, the “Address” and “Address mode” fields should set to 0x0000 and 0x00, and it should not contain the “Endpoint” information in the command struct.

**Command id**

`0x0000-0044`

**Parameter**

| 1 octet | 8 octets | N octets |
|---|---|---|
| IC type | IEEE address | Install code |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| IC type | Integer | 0x00 ~ 0x03 | The install code<br>type |
| IEEE address | Integer | 8 bytes address | The device IEEE<br>address |
| Install code | Integer | 8 ~ 18 bytes | Install code with 2<br>bytes CRC |

IC type:

| | |
|---|---|
| IC type | Description |
| 0x00 | The length of install code is 6 bytes + 2 Bytes CRC = 8 bytes |
| 0x01 | The length of install code is 8 bytes + 2 Bytes CRC = 10 bytes |
| 0x02 | The length of install code is 12 bytes + 2 Bytes CRC = 14 bytes |
| 0x03 | The length of install code is 16 bytes + 2 Bytes CRC = 18 bytes |

IEEE address: The device IEEE address to be joined.

Install code: The user select the random number for the install code value, the length is determined by the IC type, the install code test pattern as below:

| | | |
|---|---|---|
| IC type | Install code value | CRC-16 |
| 0x00 | 0x83, 0xFE, 0xD3, 0x40, 0x7A, 0x93 | 0x2B, 0x70 |
| 0x01 | 0x83, 0xFE, 0xD3, 0x40, 0x7A, 0x93, 0x97, 0x23 | 0x97, 0xFC |
| 0x02 | 0x83, 0xFE, 0xD3, 0x40, 0x7A, 0x93, 0x97, 0x23, 0xA5,<br>0xC6, 0x39, 0xB2 | 0xAD, 0x8B |
| 0x03 | 0x83, 0xFE, 0xD3, 0x40, 0x7A, 0x93, 0x97, 0x23, 0xA5,<br>0xC6, 0x39, 0xB2, 0x69, 0x16, 0xD5, 0x05 | 0xC3, 0xB5 |

The gateway command example: 1. The install code type is 0x00, device IEEE address is 4d:36:6f:30:38:33:39:30 and the install code(8 bytes) is 83 FE D3 40 7A 93 2B 70 The gateway command will be FF FC FC FF 18 44 00 00 00 00 00 00 00 30 39 33 38 30 6F 36 4D 83 FE D3 40 7A 93 2B 70 71 2. The install code type is 0x03, device IEEE address is 4d:36:6f:30:38:33:39:30 and the install code(8 bytes) is 83 FE D3 40 7A 93 97 23 A5 C6 39 B2 69 16 D5 05 C3 B5 The gateway command will be FF FC FC FF 22 44 00 00 00 00 00 00 03 30 39 33 38 30 6F 36 4D 83 FE D3 40 7A 93 97 23 A5 C6 39 B2 69 16 D5 05 C3 B5 1E

#### Gateway Install Code Set response (0x0000-8044)

The Gateway Install Code Set response is in response to a Gateway Install Code Set request.

**Command id**

`0x0000-8044`

**Parameter**

| 1 octet |
|---|
| Status |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | integer | 0x00 ~ 0xFF | 0x00: install code set<br>succeeded, other non 0x00<br>value indicated install code<br>set failed. |

#### Gateway Remove Install Code request (0x0000-0045)

The Gateway Remove Install Code request command is used to remove the install code of specific IEEE address.

Please be noted that when constructing this command, the “Address” and “Address mode” fields should set to 0x0000 and 0x00, and it should not contain the “Endpoint” information in the command struct.

**Command id**

`0x0000-0045`

**Parameter**

8 octet

| | | | |
|---|---|---|---|
| IEEE address |  |  |  |
| Name | Type | Valid Range | Description |
| IEEE address | Integer | 8 bytes address | The device IEEE<br>address |

#### Gateway Remove Install Code response (0x0000-8045)

The Gateway Install Code Remove response is in response to a Gateway Remove Install Code request.

**Command id**

`0x0000-8045`

**Parameter**

| 1 octet |
|---|
| Status |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | integer | 0x00 ~ 0xFF | 0x00: install code set<br>succeeded, other non 0x00<br>value indicated install code<br>set failed. |

#### Gateway Remove All Install Code request (0x0000-0046)

The Gateway Remove All Install Code request command is used to remove install code for all devices.

Please be noted that when constructing this command, the “Address” and “Address mode” fields should set to 0x0000 and 0x00, and it should not contain the “Endpoint” information in the command struct.

**Command id**

`0x0000-0046`

**Parameter**

None

#### Gateway Remove All Install Code response (0x0000-8046)

The Gateway Remove All Install Code response is in response to a Gateway Remove All Install Code request.

**Command id**

`0x0000-8046`

**Parameter**

| 1 octet |
|---|
| Status |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | integer | 0x00 ~ 0xFF | 0x00: install code set<br>succeeded, other non 0x00<br>value indicated install code<br>set failed. |

#### Gateway Standard Time Set (0x0000-0047)

The command is used to set time attribute of Time cluster on gateway, after the time is set, the attribute will automatically increment by one every second.

**Command id**

`0x0000-0047`

**Parameter**

| 4 octet |
|---|
| Time |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Time | integer | 0x00000000<br>~<br>0xFFFFFFFE | This time standard is the<br>number of seconds since 0<br>hrs 0 mins 0 sec on 1st<br>January 2000 UTC |

### 4.4 Device and Network Management Service Status Enumeration Description

| Enumeration | Value | Description |
|---|---|---|
| SUCCESS | 0x00 | The requested operation or<br>transmission was completed<br>successfully. |
| INV_REQUESTTYPE | 0x80 | The supplied request type was<br>invalid. |
| DEVICE_NOT_FOUND | 0x81 | The requested device did not exist<br>on a device following a child<br>descriptor request to a parent. |
| INVALID_EP | 0x82 | The supplied endpoint was equal to<br>0x00 or 0xff. |
| NOT_ACTIVE | 0x83 | The requested endpoint is not<br>described by a simple descriptor. |
| NOT_SUPPORTED | 0x84 | The requested optional feature is<br>not supported on the target device. |
| TIMEOUT | 0x85 | A timeout has occurred with the<br>requested operation. |
| NO_MATCH | 0x86 | The end device bind request was<br>unsuccessful due to a failure to<br>match any suitable clusters. |
| NO_ENTRY | 0x88 | The unbind request was<br>unsuccessful due to the coordinator<br>or source device not having an entry<br>in its binding table to unbind. |
| NO_DESCRIPTOR | 0x89 | A child descriptor was not available<br>following a discovery request to a<br>parent. |
| INSUFFICIENT_SPACE | 0x8a | The device does not have storage<br>space to support the requested<br>operation. |
| NOT_PERMITTED | 0x8b | he device is not in the proper state<br>to support the requested operation. |
| TABLE_FULL | 0x8c | The device does not have table<br>space to support the operation. |
| NOT_AUTHORIZED | 0x8d | The device has rejected the<br>command due to security<br>restrictions. |
| DEVICE_BINDING_TABLE_FULL | 0x8e | The device does not have binding<br>table space to support the<br>operation. |
| INVALID_INDEX | 0x8f | The index in the received command<br>is out of bounds. |

## 5. Over the Air(OTA) Upgrade Management

### 5.1 OTA file upload flow

Following diagram shows how to upload OTA image from server to gateway module. First, file server sends OTA Start Request to gateway module, upon receives the message, gateway module would erase flash bank for the coming OTA image and send OTA Start Response. After server side receive OTA Start Response, it should start sending OTA Image Block Request, image information (file type, manufacture code...etc.) should be include in first packet. Server side must send next block after receiving OTA Image Response. After last block is sent, gateway module would sent OTA end response to server side.

The upload is stop-and-wait, and the gateway checks it as follows:

- **Sequence.** Blocks must be sent in order, starting from block 0. The gateway only accepts the block it expects next. Any other block is rejected with `0xFFFFFFFF` and not written.
- **Retransmission.** If the server gets no response to a block (for example the response frame was corrupted on the UART), it should send the same block again. If the gateway had already stored that block, it acknowledges it again without writing it a second time. The same applies to the last block: the gateway sends OTA Upload End Response again with the same status.
- **Late responses.** The server should ignore a response whose command ID or block number does not match the block it is waiting for. Such a response is a late answer to an earlier retransmission. Do not send the next block or retransmit because of it.
- **Integrity.** The server should include the CRC32 of the whole OTA file in OTA Upload Start Request. After the last block, the gateway compares it with the CRC32 of the data it received. It also checks that the received size equals Image size, that the file starts with the Zigbee OTA file identifier `0x0BEEF11E`, and that the header's total image size equals Image size. Finally it reads the image back from flash. If any check fails, the image is not marked valid: OTA Upload End Response reports `0xFFFFFFFF` and OTA File Insert Request will fail. The server must restart from OTA Upload Start Request.
- **OTA in progress.** While a Zigbee device is downloading the inserted image, the gateway rejects OTA Upload Start Request, OTA Image Block Request and OTA File Insert Request with status `0xFFFFFFFE` (busy). Otherwise these commands would erase or replace the image being served. A device counts as downloading from the moment its Query Next Image Request is accepted. This lasts until it sends Upgrade End Request, until its upgrade is aborted, or until it has requested no block for 60 seconds. To upload a new image anyway, first abort the update with OTA File Remove Request. All other OTA commands are accepted during an update.
- **Inserted image.** OTA Upload Start Request erases the image currently inserted in the OTA server. From then on, no new device is allowed to start an update (OTA File Info Response reports Server Ready `0x00`). This lasts until a new image is uploaded and inserted with OTA File Insert Request.
- **Timing.** OTA Upload Start Response comes after the whole OTA flash bank is erased, and OTA Upload End Response comes after the image is read back. Both can take several seconds, so allow them a longer timeout than the other block responses.

### 5.2 OTA Upgrade command list

Address field, Address mode field are not cared for commands in this section, End point field should be omitted

#### OTA Upload Start Request(0xF000-0000)

The command is used to notify gateway to receive OTA image file. Once gateway receive this command, it will erase flash bank for the coming OTA image.

**Command id**

`0xF000-0000`

**Parameter**

| 4 octet |
|---|
| Image CRC32 (optional) |

| Name | Type | Description |
|---|---|---|
| Image CRC32 | Uint32 | Standard CRC-32 (same as zlib `crc32()`, polynomial 0xEDB88320) of the whole OTA file. Optional: if it is omitted, the gateway skips the end-to-end CRC check. Always send it; without it, corruption on the UART that the per-frame checksum misses is not detected. |

#### OTA Upload Start Response(0xF000-8000)

The command is used to indicate that gateway is ready for receiving OTA image.

**Command id**

`0xF000-8000`

**Parameter**

| 4 octet |
|---|
| Status |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Uint32 | 0x00000000<br>0xFFFFFFFE | 0x00000000: Gateway is<br>ready for receiving OTA<br>image<br>0xFFFFFFFE: busy, a Zigbee device is downloading the inserted image. Nothing was erased. |

#### OTA Image Block Request(0xF000-0001)

The command is used to send OTA image blocks to gateway. The OTA image should be split into many blocks and send by this command. Gateway would store the OTA image in flash.

**Command id**

`0xF000-0001`

**Parameter**

| 2 octet | 2 octet | 4 octet | 4 octet | 4 octet | 4 octet | 2 octet | variable |
|---|---|---|---|---|---|---|---|
| Image type | Manufacture<br>code | File<br>version | Image<br>size | Total<br>blocks | Current<br>block | Data<br>length | Image<br>data |

| Name | Type | Description |
|---|---|---|
| Image type | Uint16 | Image type of the OTA image |
| Manufacture code | Uint16 | Manufacture code of the OTA image |
| File version | Uint32 | File version of the OTA image |
| Image size | Uint32 | Image size of the OTA image |
| Total blocks | Uint32 | Total block number of the OTA image |
| Current block | Uint32 | Current block number of the OTA image |
| Data length | Uint16 | Data size of the OTA image that is being<br>include in this command |
| Image data | Opaque | The OTA image data with length equals to<br>Data length field |

Notice that Current block field should start from 0, ends at Total blocks-1

The gateway checks each block as follows:

- Data length must equal the actual number of image data octets in the frame.
- In block 0, Image size must be non-zero, and Image size + 4 must fit in the OTA flash bank (`0x74000` octets for 1 MB flash). The gateway appends a 4-octet CRC32 after the image.
- Every later block must carry the same Image size and Total blocks as block 0.
- The data received so far must not exceed Image size.

Each frame carries at most 255 octets after the length field, so Data length can be at most 226.

#### OTA Image Block Response(0xF000-8001)

The command is sent by gateway module, indicate OTA Image Block Request status.

**Command id**

`0xF000-8001`

**Parameter**

| 4 octet |
|---|
| Current block |

| Name | Type | Description |
|---|---|---|
| Current block | Uint32 | Current block number of the OTA image (also returned for a retransmitted block the gateway had already stored)<br>0xFFFFFFFF: failed. The block was rejected (out of sequence, invalid length or image information, no OTA Upload Start Request, or out of memory) and not written. The server may retransmit the same block.<br>0xFFFFFFFE: busy, a Zigbee device is downloading. The block was not written. |

#### OTA Upload End Response(0xF000-8002)

The command is sent by gateway module in response to the last OTA Image Block Request. It indicates that OTA Image Block process is finished.

**Command id**

`0xF000-8002`

**Parameter**

| 4 octet |
|---|
| Status |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Uint32 | 0x00000000<br>0xFFFFFFFF | 0x00000000: image received and verified, and stored in flash as a valid image<br>0xFFFFFFFF: verification failed (size, CRC32, OTA file header or flash read-back). The image is not valid. Restart from OTA Upload Start Request. |

Older gateway firmware sends this response without a parameter and does not verify the image.

#### OTA File Insert Request(0xF000-0003)

After the OTA image file has been uploaded to the gateway, the user can use this command to configure the gateway to start the entire OTA server update process, including broadcasting the Image Notify packet and responding to the image request packet from the target device. Once the device receives a notification of an available update, the gateway and the device will automatically begin the OTA update process.

> **Note:** Set the OTA candidate with Insert OTA Candidate Request (`0xF000-0005`) **before** sending this command. The gateway broadcasts Image Notify as soon as the file is inserted, and devices may send Query Next Image Request within milliseconds. A query that arrives before the candidate is set is answered with `NO_IMAGE_AVAILABLE` (`0x98`), and the device will not try again until its next periodic query. The candidate address is `0x0000` (no device allowed) after every gateway reset, so it must be set again after each reset.

**Command id**

`0xF000-0003`

**Parameter**

None

#### OTA File Insert Response (0xF000-8003)

The command is in response to OTA File Insert Request command. After verifying the OTA image used in the gateway, reply with the status to notify the user.

**Command id**

`0xF000-8003`

**Parameter**

| 4 octet |
|---|
| Status |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | Uint32 | 0x00000000<br>~<br>0xFFFFFFFF | 0x00000000: SUCCESS<br>0xFFFFFFFE: busy, a Zigbee device is downloading, or an OTA image upload is in progress. Nothing was changed.<br>0xFFFFFFFF: failed, the OTA image is<br>invalid. |

#### OTA File Remove Request(0xF000-0004)

The user can use this command to remove inserted OTA file.

**Command id**

`0xF000-0004`

**Parameter**

None

#### OTA File Remove response (0xF000-8004)

The command is in response to OTA File Remove Request command.

**Command id**

`0xF000-8004`

**Parameter**

| 4 octet |
|---|
| Status |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Status | integer | 0x00000000 ~<br>0xFFFFFFFF | 0x00000000: succeeded<br>Other value: failed. |

#### Insert OTA Candidate Request (0xF000-0005)

This command allows the user to authorize devices on the Zigbee network to perform firmware updates via OTA (Over-the-Air) upgrade. The candidate address can be set to either a specific unicast address or the broadcast address 0xFFFF. When a unicast address is specified, only the device with that address is permitted to receive the OTA update. When set to 0xFFFF, the OTA Upgrade Server will permit all OTA-capable devices in the network to concurrently initiate the firmware download process.

Send this command before OTA File Insert Request (`0xF000-0003`); see the note there. The candidate setting is not saved to flash and is reset to `0x0000` when the gateway resets.

**Command id**

`0xF000-0005`

**Parameter**

| 2 octet |
|---|
| Candidate Addr |

| Name | Type | Valid<br>Range | Description |
|---|---|---|---|
| Candidate<br>Addr | integer | 0x0000 ~<br>0xFFFF | 0x0000: no OTA candidate assigned<br>0xFFFF: all eligible devices in the network<br>Other value: only for the assigned address. |

#### Insert OTA Candidate response (0xF000-8005)

The command is in response to Insert OTA Candidate Request command.

**Command id**

`0xF000-8005`

**Parameter**

| 1 octet |
|---|
| Status |

| Name | Type | Valid<br>Range | Description |
|---|---|---|---|
| Status | integer | 0x00 ~<br>0xFF | 0x00: succeed<br>Other value: failed |

#### Remove OTA Candidate Request (0xF000-0006)

This command is used to clear the OTA candidate setting; the candidate address will be set to 0x0000.

**Command id**

`0xF000-0006`

**Parameter**

None

#### Remove OTA Candidate Response (0xF000-8006)

The command is in response to Remove OTA Candidate Request command.

**Command id**

`0xF000-8006`

**Parameter**

| 1 octet |
|---|
| Status |

| Name | Type | Valid<br>Range | Description |
|---|---|---|---|
| Status | integer | 0x00 ~<br>0xFF | 0x00: succeed<br>Other value: failed |

#### Get OTA Candidate Request (0xF000-0007)

This command allows the user to query the currently configured OTA candidate address.

**Command id**

`0xF000-0007`

**Parameter**

None

#### Get OTA Candidate Response(0xF000-8007)

The command is in response to Get OTA Candidate Request, indicated the current candidate address.

**Command id**

`0xF000-8007`

**Parameter**

| 2 octet |
|---|
| Candidate Addr |

| Name | Type | Valid<br>Range | Description |
|---|---|---|---|
| Candidate<br>Addr | integer | 0x0000 ~<br>0xFFFF | 0x0000: no OTA candidate assigned<br>0xFFFF: all eligible devices in the network<br>Other value: only for the assigned address. |

#### OTA Update Status Response(0xF000-8008)

While a device is undergoing the OTA update process, the user may receive this command to monitor the current OTA status of the device. The reported status may include the following: 1. The current completion percentage of the image update 2. Whether the update was successful 3. OTA update was aborted by the OTA server 4. OTA update was aborted by the OTA client

**Command id**

`0xF000-8008`

**Parameter**

| 2 octet | 1 octet | 1 octet |
|---|---|---|
| Device Address | Status | Progress Percent |

| Name | Type | Valid<br>Range | Description |
|---|---|---|---|
| Device<br>Address | Integer | 0x0001 ~<br>0xFFF7 | OTA processing device address |
| Status | Integer | 0x00 ~<br>0xFF | 0x00: device in OTA update progress, the<br>progress in percentage show in “Progress<br>Percent” field.<br>0x01: device update succeeded, with the<br>“Progress Percent” field set to 0x64 (100%).<br>0x02: the OTA server has aborted the OTA<br>update for the device.<br>0x03: the OTA update process has been<br>aborted by the OTA client. |
| Progress<br>Percent | Integer | 0x00 ~<br>0xFF | 0x00 ~ 0x64: OTA progress percentage<br>0xFF: OTA process has been aborted<br>Others: reserved |

#### Device Query Image Info(0xF000-8009)

The user can use this command to retrieve the device's current OTA-related information, including the device address, manufacturer code, image type, and image version. When the user identifies that a firmware update is available for the device, they can initiate the OTA update process using the appropriate OTA commands for the target device.

**Command id**

`0xF000-8009`

**Parameter**

| 2 octets | 2 octets | 2 octets | 4 octets |
|---|---|---|---|
| Device Address | Manufacturer Code | Image Type | Image Version |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Device<br>Address | Integer | 0x0001 ~ 0xFFF7 | device address for query<br>image |
| Manufacturer<br>Code | Integer | 0x0000 ~ 0xFFFF | device manufacturer code |
| Image Type | Integer | 0x0000 ~ 0xFFFF | device image type |
| Image<br>Version | Integer | 0x00000000 ~<br>0xFFFFFFFF | device image version |

#### OTA File Info Request(0xF000-000A)

Once the OTA image has been uploaded to the Zigbee gateway, the user can use this command to obtain information about the OTA image currently stored in the gateway and verify the integrity of the OTA image.

**Command id**

`0xF000-000A`

**Parameter**

None

#### OTA File Info response (0xF000-800A)

The Gateway Get OTA File Info response is in response to a Gateway Get OTA File Info command. Providing the OTA image information stored in the zigbee gateway.

**Command id**

`0xF000-800A`

**Parameter**

| 1 octet | 1 octet | 2 octets | 2 octets | 4 octets | 4 octets |
|---|---|---|---|---|---|
| Image<br>Found | Server<br>Ready | Manufacturer<br>Code | Image Type | Image Size | Image<br>Version |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| Image Found | integer | 0x00 ~ 0x01 | 0x00: image not found in zigbee gateway<br>0x01: image found in zigbee gateway |
| Server Ready | integer | 0x00 ~ 0x01 | 0x00: image is not inserted to gateway<br>0x01: gateway is ready for ota process |
| Manufacturer<br>Code | Integer | 0x0000 ~<br>0xFFFE | 0x0000: not valid manufacturer code<br>Others: manufacturer code |
| Image Type | Integer | 0x0000 ~<br>0xFFFE | 0x0000: not valid image type<br>Others: image type |
| Image Size | Integer | 0x00000000<br>~<br>0x00074000 | 0x00000000: not valid image size<br>Others: image size |
| Image Version | Integer | 0x00000000<br>~<br>0xFFFFFFFF | 0x00000000: not valid image version<br>Others: image version |

### 5.3 OTA Command Example

The command IDs used below are the ones defined in section 5.2 and implemented by the firmware.
The frames are network-management style: header, length, command ID (little endian), address,
address mode, parameters, checksum — no endpoint field.

#### Upload the OTA File

1. **OTA Upload Start (`0xF000-0000`)** with the CRC32 of the whole OTA file (`0x1A2B3C4D` in this
   example):

   ```
   FF FC FC FF 0B 00 00 00 F0 00 00 00 4D 3C 2B 1A 36
   ```

   The gateway erases the OTA flash bank and then answers with OTA Upload Start Response, status
   success:

   ```
   FF FC FC FF 0B 00 80 00 F0 00 00 00 00 00 00 00 84
   ```

2. Send OTA Image Block Request (`0xF000-0001`) from block 0 to Total blocks-1. Wait for each
   block's response before sending the next one. For example, the gateway acknowledges block 5:

   ```
   FF FC FC FF 0B 01 80 00 F0 00 00 00 05 00 00 00 7E
   ```

   or rejects it:

   ```
   FF FC FC FF 0B 01 80 00 F0 00 00 00 FF FF FF FF 87
   ```

   If no response arrives within the timeout, send the same block again.

3. The last block is answered by OTA Upload End Response (`0xF000-8002`). Success:

   ```
   FF FC FC FF 0B 02 80 00 F0 00 00 00 00 00 00 00 82
   ```

   Verification failed (the image is not valid; restart from step 1):

   ```
   FF FC FC FF 0B 02 80 00 F0 00 00 00 FF FF FF FF 86
   ```

#### Insert/Remove the OTA File

After the OTA image has been uploaded to the Zigbee module (OTA Upload Start Request followed by
OTA Image Block Requests), first set the OTA target with Insert OTA Candidate (`0xF000-0005`, see
*Add/Remove the OTA Target* below), then issue **OTA File Insert (`0xF000-0003`)** to insert the image.
Once the image is inserted, the Zigbee module broadcasts an `Image_Notify` packet to the Zigbee
network, announcing the availability of the OTA image to eligible devices:

```
FF FC FC FF 07 03 00 00 F0 00 00 00 05
```

If the OTA server's candidate address has been configured — either as a specific address or as
`0xFFFF` — the target device should begin the OTA update process. Always set the candidate before
inserting the file: if a device's Query Next Image Request arrives while the candidate is still
unset, the gateway answers `NO_IMAGE_AVAILABLE` (`0x98`) and the device does not start the update.

**OTA File Remove (`0xF000-0004`)** aborts an update in progress:

```
FF FC FC FF 07 04 00 00 F0 00 00 00 04
```

When this command is executed, the OTA image file is no longer recognised as a valid image on the
server side, and the ongoing OTA process is terminated. Issue OTA File Insert again to restart the
OTA process.

#### Add/Remove the OTA Target

**Insert OTA Candidate (`0xF000-0005`)** designates a specific device on the Zigbee network to
perform an OTA update; its parameter is the 2-octet short address. **Remove OTA Candidate
(`0xF000-0006`)** clears the currently set target device address.

Note that the candidate commands only control which device address is authorised to *begin* the
OTA update. If the device is already undergoing the OTA update procedure and the update needs to be
aborted, OTA File Remove (`0xF000-0004`) must be used to terminate it.

**Get OTA Candidate (`0xF000-0007`)** queries the current configuration:

| Returned value | Meaning |
|---|---|
| `0x0000` | No device is currently allowed to initiate an OTA update |
| `0x0001` - `0xFFF7` | Only the specified device address may begin the OTA update process |
| `0xFFFF` | All eligible devices on the network are allowed to perform the update |

#### OTA Update Status Response Payload

**OTA Update Status Response (`0xF000-8008`)** carries a 4-octet parameter: the device's 2-octet
short address, a status octet and a progress octet.

| Status | Meaning | Progress octet |
|---|---|---|
| `0x00` | Update in progress | Completion in percent |
| `0x01` | Update finished successfully | `0x64` (100%) |
| `0x02` | Update aborted by the server | `0xFF`, not valid |
| `0x03` | Update aborted by the client | `0xFF`, not valid |

#### Starting The OTA Firmware Update Process

The following example shows the sequence of commands involved in the OTA update process, from
start to finish. The target device address is `0xCBA9` and the OTA image has already been uploaded
to the Zigbee module.

1. Prior to initiating the update process, the OTA server regularly receives Device Query Image
   Info (`0xF000-8009`) reports for devices requesting information about the next available OTA
   image:

   ```
   FF FC FC FF 11 09 80 00 F0 00 00 00 A9 CB 7B 00 41 01 01 01 01 01 40
   ```

2. Set up the OTA candidate address with Insert OTA Candidate (`0xF000-0005`). This must be done
   before step 3:

   ```
   FF FC FC FF 09 05 00 00 F0 00 00 00 A9 CB 8D
   ```

3. Insert the OTA file with OTA File Insert (`0xF000-0003`); the OTA update process should now
   start:

   ```
   FF FC FC FF 07 03 00 00 F0 00 00 00 05
   ```

4. During the update process the device reports its status via OTA Update Status Response
   (`0xF000-8008`). For instance, an update completion of 2%:

   ```
   FF FC FC FF 0B 08 80 00 F0 00 00 00 A9 CB 00 02 06
   ```

5. After the device completes the update, the server reports success at 100% complete:

   ```
   FF FC FC FF 0B 08 80 00 F0 00 00 00 A9 CB 01 64 A3
   ```

#### Aborting the OTA Update Process

During the OTA update process the server can abort the ongoing update. The device update can be
re-initiated afterwards by sending the insert command again.

1. During the update process the device reports its status via OTA Update Status Response
   (`0xF000-8008`). For instance, an update completion of 8%:

   ```
   FF FC FC FF 0B 08 80 00 F0 00 00 00 A9 CB 00 08 00
   ```

2. Abort the update with OTA File Remove (`0xF000-0004`):

   ```
   FF FC FC FF 07 04 00 00 F0 00 00 00 04
   ```

3. The user then receives a series of responses from the server:

   A) After the server terminates the update process it continues to receive update requests from
   the device - typically up to three times. Although the OTA process has been stopped, the device
   still attempts to initiate the update. The server sends status `0x02` to notify the user that
   the OTA process has been terminated and that no further image update service will be provided:

   ```
   FF FC FC FF 0B 08 80 00 F0 00 00 00 A9 CB 02 FF 07
   ```

   B) After the device's repeated attempts fail it formally requests termination of the update
   process. The server then issues status `0x03` to notify the user that the OTA update process has
   been officially terminated:

   ```
   FF FC FC FF 0B 08 80 00 F0 00 00 00 A9 CB 03 FF 06
   ```

4. After the update has been terminated, the OTA update process can be initiated again with OTA
   File Insert (`0xF000-0003`). Depending on the device's current state it will either resume or
   restart the update process until completion:

   ```
   FF FC FC FF 07 03 00 00 F0 00 00 00 05
   ```

## 6. Application Service Management

### 6.1 Device Information

#### Get device version info (0x0001-0000)

**Command id**

`0x0001-0000`

**Parameter**

None

#### Get device version info response (0x0001-8000)

**Command id**

`0x0001-8000`

**Parameter**

| 1 octet | 1 octet | 1 octet | 1 octet |
|---|---|---|---|
| ZCLVersion | ApplicationVersion | StackVersion | HWVersion |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| ZCLVersion<br>ZCLVersion | Uint8 | 0x00-0xff<br>0x00-0xff | ZCL version<br>number |
| ApplicationVersion | Uint8 | 0x00-0xff | Application<br>version number |
| StackVersion | Uint8 | 0x00-0xff | Stack version<br>number |
| HWVersion | Uint8 | 0x00-0xff | Hardware version<br>number |

#### Get device manufacture name (0x0001-0001)

**Command id**

`0x0001-0001`

**Parameter**

None

#### Get device manufacture name response (0x0001-8001)

**Command id**

`0x0001-8001`

**Parameter**

| 1 octet | variable |
|---|---|
| String Length | String value |

#### Get device model id (0x0001-0002)

**Command id**

`0x0001-0002`

**Parameter**

None

#### Get device model id response (0x0001-8002)

**Command id**

`0x0001-8002`

**Parameter**

| 1 octet | variable |
|---|---|
| String Length | String value |

#### Get device date code (0x0001-0003)

**Command id**

`0x0001-0003`

**Parameter**

None

#### Get device date code response (0x0001-8003)

**Command id**

`0x0001-8003`

**Parameter**

| 1 octet | variable |
|---|---|
| String Length | String value |

#### Get software build id (0x0001-0004)

**Command id**

`0x0001-0004`

**Parameter**

None

#### Get software build id response (0x0001-8004)

**Command id**

`0x0001-8004`

**Parameter**

| 1 octet | variable |
|---|---|
| String Length | String value |

#### Default Response (0x0001-8800)

**Command id**

`0x0001-8800`

**Parameter**

| 2 octets | 1 octet | 1 octet |
|---|---|---|
| Cluster identifier | Command identifier | Status |

Cluster identifier: The current cluster ID used for the default response.

Command identifier: The ID of the command to be executed within the

corresponding cluster.

Status: The execution status of the command. 0 indicates success; any other

value indicates failure.

#### Read device attributes (0x0002-0000)

**Command id**

`0x0002-0000`

**Parameter**

| 2 octets | 2 octets |
|---|---|
| ClusterID | AttributeIdentifier |

#### Read device attributes response (0x0002-8000)

**Command id**

`0x0002-8000`

**Parameter**

| 2 octet | 2 octet | 1 octet | 1 octet | Variable |
|---|---|---|---|---|
| ClusterID | AttributeIdentifier | Status | AttributeDataType | AttributeData |

AttributeDataType & AttributeData field only be included when Status field is the value of Success. AttributeDataType filed use to indicate the data type of AttributeData field. The definition of AttributeDataType field were list in following table:

| | | | |
|---|---|---|---|
| Data Type | Type | Attribute Data Type | Valid Value |
| Boolean | bool | 0x10 | 0xff |
| Unsigned 8-bit<br>integer | uint8<br>uint8 | 0x20<br>0x20 | 0xff<br>0xff |

| |
|---|
| Unsigned 16-bit<br>uint16<br>0x21<br>0xfff |
| integer |
| Unsigned 32-bit<br>uint32<br>0x23<br>0xffffffff |
| integer |

#### Write device attributes (0x0002-0001)

**Command id**

`0x0002-0001`

**Parameter**

| 2 octets | 2 octets | 1 octets | variable |
|---|---|---|---|
| Cluster ID | Attribute Identifier | Data type | Data value |

Length of data value field depends on data type If data type is 0x41 or 0x42(octet string or character string), the first byte in data value filed is the length of the string If data type is 0x43(long octet), the first two bytes in data value filed is the length of the string

#### Write device attributes response (0x0002-8001)

**Command id**

`0x0002-8001`

**Parameter**

| 1 octets | 2 octets |
|---|---|
| status | Attribute ID |

Attribute id field is omitted if status=0(success)

#### Configure reporting (0x0002-0002)

**Command id**

`0x0002-0002`

**Parameter**

| 2 octets | 2 octets | 1 octets | 2 octets | 2 octets | variable |
|---|---|---|---|---|---|

| |
|---|
| Cluster ID<br>Attribute<br>Identifier<br>Attribute<br>data type<br>Min report<br>interval<br>Max<br>report<br>interval<br>Reportable<br>change |

Reportable change field represent minimum changes to the attributes that would reports. For attributes with analog data type, data length is the same as the attribute data type (e.g. current level). For attributes of 'discrete' data type, this field is omitted (e.g. onoff).

#### Configure reporting response (0x0002-8002)

**Command id**

`0x0002-8002`

**Parameter**

| 2 octets | 1 octets | 1 octets | 2 octets |
|---|---|---|---|
| Cluster id | status | Direction | Attribute ID |

Direction and attribute id field are omitted if status=0(success)

#### Report attribute data (0x0002-8800)

**Command id**

`0x0002-8800`

**Parameter**

| 2 octets | Variable octets | Variable octets | …. | Variable octets |
|---|---|---|---|---|
| Cluster ID | Report 1 | Report 2 | …. | Report n |

Format of the attribute report

| 2 octets | 1 octets | Variable octets |
|---|---|---|
| Attribute identifier | Attribute data type | Attribute data |

#### Read device custom cluster attributes (0x0002-0003)

**Command id**

`0x0002-0003`

**Parameter**

| 2 octets | 2 octets | 2 octets |
|---|---|---|
| ClusterID | AttributeIdentifier | Manufacture code |

The command is used to read custom cluster attribute

#### Write device custom cluster attributes (0x0002-0004)

**Command id**

`0x0002-0004`

**Parameter**

| 2 octets | 2 octets | 2 octets | 1 octets | variable |
|---|---|---|---|---|
| Cluster ID | Attribute<br>Identifier | Manufacture<br>code | Data type | Data value |

Length of data value field depends on data type If data type is 0x41 or 0x42(octet string or character string), the first byte in data value filed is the length of the string If data type is 0x43(long octet), the first two bytes in data value filed is the length of the string

#### APS acknowledge indication (0x0002-8005)

This command is used to determine whether the related ZCL command has been sent to the target device. When the target device receives the ZCL command, it will return this acknowledgment packet to confirm successful receipt of the ZCL command.

**Command id**

`0x0002-8005`

**Parameter**

| 1 octets |
|---|
| Status |

Status: 0 indicates success; any other value indicates failure.

If this acknowledgment message is not received after sending the command, it can be assumed that the device did not successfully receive the command. The upper layer may resend the command based on the situation.

### 6.2 Device Identify

#### Identify (0x0004-0000)

**Command id**

`0x0004-0000`

**Parameter**

| 1 octet | 2 octets |
|---|---|
| DefRspFlg | Identify Time |

#### Identify query (0x0004-0001)

**Command id**

`0x0004-0001`

**Parameter**

None

#### Identify trigger effect (0x0004-0002)

**Command id**

`0x0004-0002`

**Parameter**

| 1 octet | 1 octets | 1 octets |
|---|---|---|
| DefRspFlg | Effect identifier | Effect variant |

| | |
|---|---|
| Effect identifier<br>Effect variant | Effect |
| 0x00<br>0x00(default) | Blink |
| 0x01 | Breathe |
| 0x02 | Okay |
| 0x0b | Channel change |
| 0xfe | Finish effect |
| 0xff | Stop effect |

#### Identify query response (0x0004-8001)

**Command id**

`0x0004-8001`

**Parameter**

| 2 octet |
|---|
| Timeout |

### 6.3 Group Management

#### Add group (0x0005-0000)

**Command id**

`0x0005-0000`

**Parameter**

| 2 octet |
|---|
| Group ID |

#### Add group response (0x0005-8000)

**Command id**

`0x0005-8000`

**Parameter**

| 1 octet | 2 octets |
|---|---|
| Status | Group ID |

#### View group (0x0005-0001)

**Command id**

`0x0005-0001`

**Parameter**

| 2 octet |
|---|
| Group ID |

#### View group response (0x0005-8001)

**Command id**

`0x0005-8001`

**Parameter**

| 1 octet | 2 octets |
|---|---|
| Status | Group ID |

#### Get group membership (0x0005-0002)

**Command id**

`0x0005-0002`

**Parameter**

| 1 octet | variable |
|---|---|
| Group count | Group list |

#### Get group membership response (0x0005-8002)

**Command id**

`0x0005-8002`

**Parameter**

| 1 octet | 1 octet | variable |
|---|---|---|
| Capacity | Group count | Group list |

#### Remove group (0x0005-0003)

**Command id**

`0x0005-0003`

**Parameter**

| 2 octet |
|---|
| Group ID |

#### Remove group response (0x0005-8003)

**Command id**

`0x0005-8003`

**Parameter**

| 1 octet | 2 octets |
|---|---|
| Status | Group ID |

#### Remove all groups (0x0005-0004)

**Command id**

`0x0005-0004`

**Parameter**

| 1 octet |
|---|
| DefRspFlg |

#### Add group if identifying (0x0005-0005)

**Command id**

`0x0005-0005`

**Parameter**

| 1 octet | 2 octets |
|---|---|
| DefRspFlg | Group ID |

### 6.4 Scene Management

#### Add scene (0x0006-0000)

**Command id**

`0x0006-0000`

**Parameter**

The scene name is omitted and set the string length is “0”. For different device, the scene parameter is different. Currently this gateway will support the scene functions of the following devices.

Device ID: 0x0100 On/Off light

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets |
|---|
| On/Off<br>State |

Device ID: 0x0101 Dimmable light

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets | 2 octets | 1 octets | 1 octets |
|---|---|---|---|
| On/Off<br>State | 0x0008<br>(Level) | 0x01<br>(length) | Current<br>Level |

Device ID: 0x0102 Color dimmable light

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets | 2 octets | 1 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| On/Off<br>State | 0x0008<br>(Level) | 0x01<br>(length) | Current<br>Level | 0x0300<br>(Color) | 0x0D<br>(length) |

| 2 octets | 2 octets | 2 octet | 1 octets | 1 octets | 1 octets | 2 octets |
|---|---|---|---|---|---|---|
| CurrentX | CurrentY | Enhanced<br>CurrentHue | Current<br>Saturation | ColorLoop<br>Active | ColorLoop<br>Direction | ColorLoop<br>Time |

| 2 octets |
|---|
| ColorTemperature<br>Mireds |

Device ID: 0x010A On/Off plug-in unit

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

1 octets On/Off State

Device ID: 0x010B Dimmable plug-in unit

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets | 2 octets | 1 octets | 1 octets |
|---|---|---|---|
| On/Off<br>State | 0x0008<br>(Level) | 0x01<br>(length) | Current<br>Level |

#### Add scene response (0x0006-8000)

**Command id**

`0x0006-8000`

**Parameter**

| 1 octet | 2 octets | 1 octet |
|---|---|---|
| Status | Group ID | Scene ID |

#### View scene (0x0006-0001)

**Command id**

`0x0006-0001`

**Parameter**

| 2 octets | 1 octet |
|---|---|
| Group ID | Scene ID |

#### View scene response (0x0006-8001)

**Command id**

`0x0006-8001`

**Parameter**

All devices will receive first status byte and following the different response parameters by different device.

First byte:

| 1 octets |
|---|
| Status |

Device ID: 0x0100 On/Off light

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets |
|---|
| On/Off<br>State |

Device ID: 0x0101 Dimmable light

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets | 2 octets | 1 octets | 1 octets |
|---|---|---|---|
| On/Off<br>State | 0x0008<br>(Level) | 0x01<br>(length) | Current<br>Level |

Device ID: 0x0102 Color dimmable light

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets | 2 octets | 1 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| On/Off<br>State | 0x0008<br>(Level) | 0x01<br>(length) | Current<br>Level | 0x0300<br>(Color) | 0x0D<br>(length) |

| 2 octets | 2 octets | 2 octet | 1 octets | 1 octets | 1 octets | 2 octets |
|---|---|---|---|---|---|---|
| CurrentX | CurrentY | Enhanced<br>CurrentHue | Current<br>Saturation | ColorLoop<br>Active | ColorLoop<br>Direction | ColorLoop<br>Time |

| 2 octets |
|---|
| ColorTemperature<br>Mireds |

Device ID: 0x010A On/Off plug-in unit

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets |
|---|
| On/Off<br>State |

Device ID: 0x010B Dimmable plug-in unit

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets | 2 octets | 1 octets | 1 octets |
|---|---|---|---|
| On/Off<br>State | 0x0008<br>(Level) | 0x01<br>(length) | Current<br>Level |

#### Remove scene (0x0006-0002)

**Command id**

`0x0006-0002`

**Parameter**

| 2 octets | 1 octet |
|---|---|
| Group ID | Scene ID |

#### Remove scene response (0x0006-8002)

**Command id**

`0x0006-8002`

**Parameter**

| 1 octet | 2 octets | 1 octet |
|---|---|---|
| Status | Group ID | Scene ID |

#### Remove all scene (0x0006-0003)

**Command id**

`0x0006-0003`

**Parameter**

| 2 octets |
|---|
| Group ID |

#### Remove all scene response (0x0006-8003)

**Command id**

`0x0006-8003`

**Parameter**

| 1 octet | 2 octets |
|---|---|
| Status | Group ID |

#### Store scene (0x0006-0004)

**Command id**

`0x0006-0004`

**Parameter**

| 2 octets | 1 octet |
|---|---|
| Group ID | Scene ID |

#### Store scene response (0x0006-8004)

**Command id**

`0x0006-8004`

**Parameter**

| 1 octet | 2 octets | 1 octet |
|---|---|---|
| Status | Group ID | Scene ID |

#### Recall scene (0x0006-0005)

**Command id**

`0x0006-0005`

**Parameter**

| 1 octet | 2 octets | 1 octet | 0/2 octets |
|---|---|---|---|
| DefRspFlg | Group ID | Scene ID | Transition Time |

#### Get scene membership (0x0006-0006)

**Command id**

`0x0006-0006`

**Parameter**

| 2 octets |
|---|
| Group ID |

#### Get scene membership response (0x0006-8006)

**Command id**

`0x0006-8006`

**Parameter**

| 1 octet | 1 octet | 2 octet | 0/1 octet | variable |
|---|---|---|---|---|
| Status | Capacity | Group ID | Scene count | Scene list |

#### Enhanced add scene (0x0006-0040)

**Command id**

`0x0006-0040`

**Parameter**

The scene name is omitted and set the string length is “0”. For different device, the scene parameter is different. Currently this gateway will support the scene functions of the following devices.

Device ID: 0x0100 On/Off light

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets |
|---|
| On/Off<br>State |

Device ID: 0x0101 Dimmable light

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets | 2 octets | 1 octets | 1 octets |
|---|---|---|---|
| On/Off<br>State | 0x0008<br>(Level) | 0x01<br>(length) | Current<br>Level |

Device ID: 0x0102 Color dimmable light

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets | 2 octets | 1 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| On/Off<br>State | 0x0008<br>(Level) | 0x01<br>(length) | Current<br>Level | 0x0300<br>(Color) | 0x0D<br>(length) |

| 2 octets | 2 octets | 2 octet | 1 octets | 1 octets | 1 octets | 2 octets |
|---|---|---|---|---|---|---|
| CurrentX | CurrentY | Enhanced<br>CurrentHue | Current<br>Saturation | ColorLoop<br>Active | ColorLoop<br>Direction | ColorLoop<br>Time |

| 2 octets |
|---|
| ColorTemperature<br>Mireds |

Device ID: 0x010A On/Off plug-in unit

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets |
|---|
| On/Off<br>State |

Device ID: 0x010B Dimmable plug-in unit

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets | 2 octets | 1 octets | 1 octets |
|---|---|---|---|
| On/Off<br>State | 0x0008<br>(Level) | 0x01<br>(length) | Current<br>Level |

#### Add scene response (0x0006-8040)

**Command id**

`0x0006-8040`

**Parameter**

| 1 octet | 2 octets | 1 octet |
|---|---|---|
| Status | Group ID | Scene ID |

#### Enhanced view scene (0x0006-0041)

**Command id**

`0x0006-0041`

**Parameter**

| 2 octets | 1 octet |
|---|---|
| Group ID | Scene ID |

#### Enhanced view scene response (0x0006-8041)

**Command id**

`0x0006-8041`

**Parameter**

All devices will receive first status byte and following the different response parameters by different device.

First byte:

| 1 octets |
|---|
| Status |

Device ID: 0x0100 On/Off light

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets |
|---|
| On/Off<br>State |

Device ID: 0x0101 Dimmable light

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets | 2 octets | 1 octets | 1 octets |
|---|---|---|---|
| On/Off<br>State | 0x0008<br>(Level) | 0x01<br>(length) | Current<br>Level |

Device ID: 0x0102 Color dimmable light

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets | 2 octets | 1 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| On/Off<br>State | 0x0008<br>(Level) | 0x01<br>(length) | Current<br>Level | 0x0300<br>(Color) | 0x0D<br>(length) |

| 2 octets | 2 octets | 2 octet | 1 octets | 1 octets | 1 octets | 2 octets |
|---|---|---|---|---|---|---|
| CurrentX | CurrentY | Enhanced<br>CurrentHue | Current<br>Saturation | ColorLoop<br>Active | ColorLoop<br>Direction | ColorLoop<br>Time |

| 2 octets |
|---|
| ColorTemperature<br>Mireds |

Device ID: 0x010A On/Off plug-in unit

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets |
|---|
| On/Off<br>State |

Device ID: 0x010B Dimmable plug-in unit

| 2 octets | 1 octets | 2 octets | 1 octets | 2 octets | 1 octets |
|---|---|---|---|---|---|
| Group ID | Scene ID | Transition<br>Time | 0x00<br>(Name) | 0x0006<br>(On/Off) | 0x01<br>(length) |

| 1 octets | 2 octets | 1 octets | 1 octets |
|---|---|---|---|
| On/Off<br>State | 0x0008<br>(Level) | 0x01<br>(length) | Current<br>Level |

#### Copy scene (0x0006-0042)

**Command id**

`0x0006-0042`

**Parameter**

| 1 octets | 2 octet | 1 octets | 2 octet | 1 octets |
|---|---|---|---|---|
| Mode<br>Mode bit map | Group ID<br>from | Scene ID<br>from | Group ID<br>to | Scene ID<br>to |
| Bit 0 | Copy all scenes |  |  |  |
| Bit 1-7 | Reserved |  |  |  |

### 6.5 On/Off Control

#### Off (0x0007-0000)

**Command id**

`0x0007-0000`

**Parameter**

| 1 octet |
|---|
| DefRspFlg |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |

#### On (0x0007-0001)

**Command id**

`0x0007-0001`

**Parameter**

| 1 octet |
|---|
| DefRspFlg |

| Name | Type | Valid Range | Description |
|---|---|---|---|

| |
|---|
| DefRspFlg<br>Bool<br>0/1<br>Enable/Disable the default response |

#### Toggle (0x0007-0002)

**Command id**

`0x0007-0002`

**Parameter**

| 1 octet |
|---|
| DefRspFlg |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |

#### Off with effect (0x0007-0003)

**Command id**

`0x0007-0003`

Parameter

| 1 octet | 1 octet | 1 octet |
|---|---|---|
| DefRspFlg | Effect identifier | Effect variant |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |
| Effect identifier value |  | Effect variant value | Description |
| 0x00(Delayed All Off) |  | 0x00 | Fade to off in 0.8 seconds |
|  |  | 0x01 | No fade |
|  |  | 0x02 | 50% dim down in 0.8 seconds<br>then fade to off in 12 seconds |
|  |  | 0x03 to 0xff | Reserved |

| | | |
|---|---|---|
| 0x01(Dying Light) | 0x00 | 20% dim up in 0.5s then fade to<br>off in 1 second |
|  | 0x01 to 0xff | Reserved |
| 0x02 to 0xff | Reserved | Reserved |

#### On with recall global scene (0x0007-0004)

**Command id**

`0x0007-0004`

**Parameter**

| 1 octet |
|---|
| DefRspFlg |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |

#### On with timed off (0x0007-0005)

**Command id**

`0x0007-0005`

**Parameter**

| 1 octet | 1 octet | 2 octets | 2 octets |
|---|---|---|---|
| DefRspFlg | On/Off Control | On time | Off Wait time |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |

### 6.6 Level Control

#### Move to level (0x0009-0000)

**Command id**

`0x0009-0000`

**Parameter**

| 1 octet | 1 octet | 2 octets | 1 octet | 1 octet |
|---|---|---|---|---|
| DefRspFlg | Level | Transition time | OptionsMask | OptionsOverride |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device. On receipt of this command, a device SHALL move from its current level to the value given in the Level field. The movement SHALL be as continuous as technically practical, i.e., not a step function, and the time taken to move to the new level SHALL be equal to the value of the Transition time field, in tenths of a second, or as close to this as the device is able.

#### Move (0x0009-0001)

**Command id**

`0x0009-0001`

**Parameter**

| 1 octet | 1 octet | 1 octet | 1 octet | 1 octet |
|---|---|---|---|---|
| DefRspFlg | Move mode | Rate | OptionsMask | OptionsOverride |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |
| Move mode value |  | Description | Action |
| 0x00<br>0x00 |  | Up<br>Up | Increase the device’s level at the rate given in<br>the Rate field. If the level reaches the<br>maximum allowed for the device, stop. |
| 0x01 |  | Down | Decrease the device’s level at the rate given<br>in the Rate field. If the level reaches the |

| |
|---|
| minimum allowed for the device, stop. |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device. The Rate field specifies the rate of movement in units per second. The actual rate of movement SHOULD be as close to this rate as the device is able.

#### Step (0x0009-0002)

**Command id**

`0x0009-0002`

**Parameter**

| 1 octet | 1 octet | 1 octet | 2 octets | 1 octet | 1 octet |
|---|---|---|---|---|---|
| DefRspFlg | Step<br>mode | Step<br>size | Transition<br>time | OptionsMask | OptionsOverride |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |
| Step mode value |  | Description | Action |
| 0x00<br>0x00 |  | Up<br>Up | Increase CurrentLevel by 'Step size' units, or<br>until it reaches the maximum level allowed for<br>the device if this reached in the process. In<br>the latter case, the transition time SHALL be<br>proportionally reduced. |
| 0x01 |  | Down | Decrease CurrentLevel by 'Step size' units, or<br>until it reaches the minimum level allowed for<br>the device if this reached in the process. In<br>the latter case, the transition time SHALL be<br>proportionally reduced. |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device. The Transition time field specifies the time that SHALL be taken to perform the step, in tenths of a second. A step is a change in the CurrentLevel of 'Step size' units.

#### Stop (0x0009-0003)

**Command id**

`0x0009-0003`

**Parameter**

| 1 octet | 1 octet | 1 octet |
|---|---|---|
| DefRspFlg | OptionsMask | OptionsOverride |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device. Upon receipt of this command, any Move to Level, Move or Step command currently in process SHALL be terminated.

#### Move to level (with On/Off) (0x0009-0004)

**Command id**

`0x0009-0004`

**Parameter**

| 1 octet | 1 octet | 2 octets | 1 octet | 1 octet |
|---|---|---|---|---|
| DefRspFlg | Level | Transition time | OptionsMask | OptionsOverride |

Same command usage as “Move to level” command.

#### Move (with On/Off) (0x0009-0005)

**Command id**

`0x0009-0005`

**Parameter**

| 1 octet | 1 octet | 1 octet | 1 octet | 1 octet |
|---|---|---|---|---|
| DefRspFlg | Move mode | Rate | OptionsMask | OptionsOverride |

Same command usage as “Move” command.

#### Step (with On/Off) (0x0009-0006)

**Command id**

`0x0009-0006`

**Parameter**

| 1 octet | 1 octet | 1 octet | 2 octets | 1 octet | 1 octet |
|---|---|---|---|---|---|
| DefRspFlg | Step<br>mode | Step<br>size | Transition<br>time | OptionsMask | OptionsOverride |

Same command usage as “Step” command.

### 6.7 Alarm

#### Alarm Command (0x000A-8000)

The alarm command signals an alarm situation on the sending device.

**Command id**

`0x000A-8000`

**Parameter**

| 1 octet | 2 octet |
|---|---|
| Alarm code | Cluster identifier |

## 7. Lighting Application Service

### 7.1 Color Control

#### Move to hue (0x0021-0000)

**Command id**

`0x0021-0000`

**Parameter**

| 1 octet | octet | 1 octet | 2 octets | 1 octet | 1 octet |
|---|---|---|---|---|---|
| DefRspFlg | Hue | Direction | Transition<br>time | OptionsMask | OptionsOverride |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device. The Hue field specifies the hue to be moved to. The Direction field:

| | |
|---|---|
| Direction | Description |
| 0x00 | Shortest distance |
| 0x01 | Longest distance |
| 0x02 | Up |
| 0x03 | Down |

The Transition Time field specifies, in 1/10ths of a second, the time that SHALL be taken to move to the new hue.

#### Move hue (0x0021-0001)

**Command id**

`0x0021-0001`

**Parameter**

| 1 octet | 1 octet | 1 octet | 1 octet | 1 octet |
|---|---|---|---|---|
| DefRspFlg | Move<br>mode | Rate | OptionsMask | OptionsOverride |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device. The Move mode:

| | | |
|---|---|---|
| Move mode value | Description | Action on Receipt |
| 0x00 | Stop | If moving, stop, else ignore the<br>command (i.e., the command is<br>accepted but has no effect). NB This<br>MAY also be used to stop a Move to<br>Hue command, a Move to Saturation<br>command, or a Move to Hue and<br>Saturation command. |
| 0x01 | Up | Increase the device’s hue at the rate<br>given in the Rate field. If the hue<br>reaches the maximum allowed for the<br>device, then proceed to its minimum<br>allowed value. |
| 0x02 | Reserved |  |
| 0x03 | Down | Decrease the device’s hue at the rate<br>given in the Rate field. If the hue<br>reaches the minimum allowed for the<br>device, then proceed to its maximum<br>allowed value. |

The Rate field specifies the rate of movement in steps per second. A step is a change in the device’s hue of one unit.

#### Step hue (0x0021-0002)

**Command id**

`0x0021-0002`

**Parameter**

| 1 octet | 1 octet | 1 octet | 1 octet | 1 octet | 1 octet |
|---|---|---|---|---|---|
| DefRspFlg | Step<br>mode | Step<br>size | Transition<br>Time | OptionsMask | OptionsOverride |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device. The Step mode:

| | | |
|---|---|---|
| Step mode value | Description | Action on Receipt |
| 0x00 | Reserved |  |
| 0x01 | Up | Increase the device’s hue by one step,<br>in a continuous fashion. If the hue<br>value reaches the maximum value<br>then proceed to the minimum allowed<br>value. |
| 0x02 | Reserved |  |
| 0x03 | Down | Decrease the device’s hue by one<br>step, in a continuous fashion. If the<br>hue value reaches the minimum value<br>then proceed to the maximum allowed<br>value. |

The Step size field specifies, to be added to (or subtracted from) the current value of the device’s hue. The Transition Time field specifies, in 1/10ths of a second, the time that SHALL be taken to perform the step. A step is a change in the device’s hue of ‘Step size’ units.

#### Move to saturation (0x0021-0003)

**Command id**

`0x0021-0003`

**Parameter**

| 1 octet | 1 octet | 2 octet | 1 octet | 1 octet |
|---|---|---|---|---|
| DefRspFlg | Saturation | Transition<br>Time | OptionsMask | OptionsOverride |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device. On receipt of this command, a device set the ColorMode attribute to the value 0x00 and SHALL then move from its current saturation to the value given in the Saturation field. The movement SHALL be continuous, i.e., not a step function, and the time taken to move to the new saturation SHALL be equal to the Transition Time field, in 1/10ths of a second.

#### Move saturation (0x0021-0004)

**Command id**

`0x0021-0004`

**Parameter**

| 1 octet | 1 octet | octet | 1 octet | 1 octet |
|---|---|---|---|---|
| DefRspFlg | Move<br>mode | Rate | OptionsMask | OptionsOverride |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device. The Move mode field:

| | | |
|---|---|---|
| Move mode value | Description | Action on Receipt |
| 0x00 | Stop | If moving, stop, else ignore the command<br>(i.e., the command is accepted but has no<br>affect). NB This MAY also be used to stop a<br>Move to Saturation command, a Move to Hue<br>command, or a Move to Hue and Saturation<br>command. |
| 0x01 | Up | Increase the device’s saturation at the rate<br>given in the Rate field. If the saturation<br>reaches the maximum allowed for the device,<br>stop. |
| 0x02 | Reserved |  |
| 0x03 | Down | Decrease the device’s saturation at the rate<br>given in the Rate field. If the saturation<br>reaches the minimum allowed for the device,<br>stop. |

The Rate field specifies the rate of movement in steps per second. A step is a change in the device’s saturation of one unit.

#### Step saturation (0x0021-0005)

**Command id**

`0x0021-0005`

**Parameter**

| 1 octet | 1 octet | 1 octet | 1 octet | 1 octet | 1 octet |
|---|---|---|---|---|---|
| DefRspFlg | Step<br>mode | Step<br>size | Transition<br>Time | OptionsMask | OptionsOverride |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device. The Step mode field:

| | | |
|---|---|---|
| Step mode value | Description | Action on Receipt |
| 0x00 | Reserved |  |
| 0x01 | Up | Increase the device’s saturation by one step,<br>in a continuous fashion. However, if the<br>saturation value is already the maximum<br>value then do nothing. |
| 0x02 | Reserved |  |
| 0x03 | Down | Decrease the device’s saturation by one step,<br>in a continuous fashion. However, if the<br>saturation value is already the minimum value<br>then do nothing. |

The Step size change to be added to (or subtracted from) the current value of the device’s saturation. The Transition Time field specifies, in 1/10ths of a second, the time that SHALL be taken to perform the step. A step is a change in the device’s saturation of ‘Step size’ units.

#### Move to hue and saturation (0x0021-0006)

**Command id**

`0x0021-0006`

**Parameter**

| 1 octet | octet | 1 octet | 2 octet | 1 octet | 1 octet |
|---|---|---|---|---|---|
| DefRspFlg | Hue | Saturation | Transition<br>Time | OptionsMask | OptionsOverride |

| Name | Type | Valid Range | Description |
|---|---|---|---|

| |
|---|
| DefRspFlg<br>Bool<br>0/1<br>Enable/Disable the default response |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device. On receipt of this command, a device shall move from its current hue and saturation to the values given in the Hue and Saturation fields. The movement SHALL be continuous, i.e., not a step function, and the time taken to move to the new color SHALL be equal to the Transition Time field, in 1/10ths of a second.

#### Move to color (0x0021-0007)

**Command id**

`0x0021-0007`

**Parameter**

| 1 octet | 2 octet | 2 octet | 2 octet | 1 octet | 1 octet |
|---|---|---|---|---|---|
| DefRspFlg | ColorX | ColorY | Transition<br>Time | OptionsMask | OptionsOverride |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device. On receipt of this command, a device SHALL move from its current color to the color given in the ColorX and ColorY fields. The movement SHALL be continuous, i.e., not a step function, and the time taken to move to the new color SHALL be equal to the Transition Time field, in 1/10ths of a second.

#### Move color (0x0021-0008)

**Command id**

`0x0021-0008`

**Parameter**

| 1 octet | 2 octet | 2 octet | 1 octet | 1 octet |
|---|---|---|---|---|
| DefRspFlg | RateX | RateY | OptionsMask | OptionsOverride |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device. The RateX field specifies the rate of movement in steps per second. A step is a change in the device's CurrentX attribute of one unit. The RateY field specifies the rate of movement in steps per second. A step is a change in the device's CurrentY attribute of one unit.

#### Step color (0x0021-0009)

**Command id**

`0x0021-0009`

**Parameter**

| 1 octet | 2 octet | 2 octet | 2 octet | 1 octet | 1 octet |
|---|---|---|---|---|---|
| DefRspFlg | StepX | StepY | Transition<br>Time | OptionsMask | OptionsOverride |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device. The StepX and StepY fields specify the change to be added to the device's CurrentX attribute and CurrentY attribute respectively. The Transition Time field specifies, in 1/10ths of a second, the time that SHALL be taken to perform the color change.

#### Move to color temperature (0x0021-000A)

**Command id**

`0x0021-000A`

**Parameter**

| 1 octet | 2 octet | 2 octet | 1 octet | 1 octet |
|---|---|---|---|---|
| DefRspFlg | Color<br>Temperature<br>Mireds | Transition<br>Time | OptionsMask | OptionsOverride |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device. On receipt of this command, a device SHALL move from its current color to the color given by the Color Temperature Mireds field. The movement SHALL be continuous, i.e., not a step function, and the time taken to move to the new color SHALL be equal to the Transition Time field, in 1/10ths of a second.

#### Stop Move Step (0x0021-0047)

**Command id**

`0x0021-0047`

**Parameter**

| 1 octet | 1 octet | 1 octet |
|---|---|---|
| DefRspFlg | OptionsMask | OptionsOverride |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device. On receipt of this command, any Move to, Move or Step command currently in process SHALL be terminated. The values of the CurrentHue, EnhancedCurrentHue, and CurrentSaturation attributes SHALL be left at their present value upon receipt of the Stop Move Step command, and the RemainingTime attribute SHALL be set to zero.

#### Move color temperature (0x0021-004B)

**Command id**

`0x0021-004B`

**Parameter**

| 1 octet | 1 octets | 2 octets | 2 octets | 2 octets |
|---|---|---|---|---|
| DefRspFlg | Move Mode | Rate | Color<br>Temperature<br>Minimum<br>Mireds | Color<br>Temperature<br>Maximum<br>Mireds |

| 1 octet | 1 octets |
|---|---|
| OptionsMask | OptionsOverride |

| Name | Type | Valid Range | Description |
|---|---|---|---|
| DefRspFlg | Bool | 0/1 | Enable/Disable the default response |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device. The Move mode field:

| | | |
|---|---|---|
| Move mode value | Description | Action on Receipt |
| 0x00 | Stop | If moving, stop the operation, else ignore the<br>command (i.e., the command is accepted but<br>has no effect). |
| 0x01 | Up | Increase the ColorTemperatureMireds<br>attribute (≡ decrease the color temperature in<br>kelvins) at the rate given in the Rate field. If<br>the ColorTemperatureMireds attribute<br>reaches the maximum allowed for the device<br>(via either the Color Temperature Maximum<br>Mireds field or the<br>ColorTempPhysicalMaxMireds attribute), the<br>move operation SHALL be stopped. |
| 0x02 | Reserved |  |
| 0x03 | Down | Decrease the ColorTemperatureMireds<br>attribute (≡ increase the color temperature in<br>kelvins) at the rate given in the Rate field. If<br>the ColorTemperatureMireds attribute<br>reaches the minimum allowed for the device<br>(via either the Color Temperature Minimum<br>Mireds field or the<br>ColorTempPhysicalMinMireds attribute), the<br>move operation SHALL be stopped. |

The Rate field is 16-bits in length and specifies the rate of movement in steps per second. A step is a change in the color temperature of a device by one unit. The Color Temperature Minimum Mireds field is 16-bits in length and specifies a lower bound on the ColorTemperatureMireds attribute. The Color Temperature Maximum Mireds field is 16-bits in length and specifies an upper bound on the ColorTemperatureMireds attribute.

#### Step color temperature (0x0021-004C)

**Command id**

`0x0021-004C`

**Parameter**

| 1 octet | octet | octets | 2 octets | 2 octets | 2 octets |
|---|---|---|---|---|---|
| DefRspFlg | Step<br>Mode | Step<br>Size | Transition<br>Time | Color<br>Temperature<br>Minimum<br>Mireds | Color<br>Temperature<br>Maximum<br>Mireds |

| 1 octet | 1 octets |
|---|---|
| OptionsMask | OptionsOverride |

| Name | Type | Valid Range | Description |
|---|---|---|---|

| |
|---|
| DefRspFlg<br>Bool<br>0/1<br>Enable/Disable the default response |

The OptionsMask & OptionsOverride fields set to “0” as the default value to interpret missing fields from legacy device.

The Step mode field:

| | | |
|---|---|---|
| Step mode value | Description | Action on Receipt |
| 0x00 | Reserved |  |
| 0x01 | Up | Increase the ColorTemperatureMireds<br>attribute (≡ decrease the color temperature in<br>kelvins) by one step. If the<br>ColorTemperatureMireds attribute reaches<br>the maximum allowed for the device (via<br>either the Color Temperature Maximum<br>Mireds field or the<br>ColorTempPhysicalMaxMireds attribute), the<br>step operation SHALL be stopped. |
| 0x02 | Reserved |  |
| 0x03 | Down | Decrease the ColorTemperatureMireds<br>attribute (≡ increase the color temperature in<br>kelvins) by one step. If the<br>ColorTemperatureMireds attribute reaches<br>the minimum allowed for the device (via<br>either the Color Temperature Minimum<br>Mireds field or the<br>ColorTempPhysicalMinMireds attribute), the<br>step operation SHALL be stopped. |

## 8. Sensor Application Cluster Information

### 8.1 Illuminance Measurement (Cluster ID: 0x0400)

**Cluster ID: 0x0400**

**Attribute Set**

| Id | Name | Type | Range | Acc | Def | MO |
|---|---|---|---|---|---|---|
| 0x0000 | MeasuredValue | uint16 | 0x0000-0xffff | RP | 0x0000 | M |
| 0x0001 | MinMeasuredValue | uint16 | 0x0001-0xfffd | R | ms | M |
| 0x0002 | MaxMeasuredValue | uint16 | 0x0002-0xfffe | R | ms | M |
| 0x0003 | Tolerance | uint16 | 0x0000-<br>0x0800 | R | ms | O |
| 0x0004 | LightSensorType | enum8 | 0x00-0xff | R | 0xff | O |

 MeasuredValue MeasuredValue represents the Illuminance in Lux (symbol lx) as follows: MeasuredValue = 10,000 x log Illuminance + 1 Where 1 lx <= Illuminance <=3.576 Mlx, corresponding to a MeasuredValue in the range 1 to 0xfffe. The MeasuredValue attribute can take the following values.  0x0000 indicates a value of Illuminance that is too low to be measured.  MinMeasuredValue ≤ MeasuredValue ≤ MaxMeasuredValue under normal circumstances.  0xffff indicates that the Illuminance measurement is invalid. MeasuredValue is updated continuously as new measurements are made.

 MinMeasuredValue The MinMeasuredValue attribute indicates the minimum value of MeasuredValue that can be measured. A value of 0xffff indicates that this attribute is not defined  MaxMeasuredValue The MaxMeasuredValue attribute indicates the maximum value of MeasuredValue that can be measured. A value of 0xffff indicates that this attribute is not defined  Tolerance The Tolerance attribute SHALL indicate the magnitude of the possible error that is associated with MeasuredValue, using the same units and resolution  LightSensorType The LightSensorType attribute specifies the electronic type of the light sensor. This attribute shall be set to one of the non-reserved values listed in the following table.

| Attribute Value | Description |
|---|---|
| 0x00 | Photodiode |
| 0x01 | CMOS |
| 0x40-0xfe | Reserved for manufacturer specific light sensor types |
| 0xff | Unknown |

### 8.2 Temperature Measurement (Cluster ID: 0x0402)

**Cluster ID: 0x0402**

**Attribute Set:**

| Id | Name | Type | Range | Acc | Def | MO |
|---|---|---|---|---|---|---|
| 0x0000 | MeasuredValue | int16 | MinMeasuredV<br>alue –<br>MaxMeasured<br>Value | RP | non | M |
| 0x0001 | MinMeasuredValue | int16 | 0x954d-0x7ffe | R | non | M |
| 0x0002 | MaxMeasuredValue | int16 | 0x954e-0x7fff | R | non | M |
| 0x0003 | Tolerance | uint16 | 0x0000-<br>0x0800 | R |  | O |

 MeasuredValue MeasuredValue represents the temperature in degrees Celsius as follows: MeasuredValue = 100 x temperature in degrees Celsius. Where -273.15°C <= temperature <= 327.67 ºC, corresponding to a MeasuredValue in the range 0x954d to 0x7fff. The maximum resolution this format allows is 0.01 ºC. A MeasuredValue of 0x8000 indicates that the temperature measurement is unknown MeasuredValue is updated continuously as new measurements are made. MinMeasuredValue and MaxMeasuredValue define the range of the sensor.  MinMeasuredValue The MinMeasuredValue attribute indicates the minimum value of MeasuredValue that is capable of being measured. A MinMeasuredValue of 0x8000 indicates that the minimum value is unknown  MaxMeasuredValue The MaxMeasuredValue attribute indicates the maximum value of MeasuredValue that is capable of being measured. A MaxMeasuredValue of 0x8000 indicates that the maximum value is unknown.  Tolerance The Tolerance attribute SHALL indicate the magnitude of the possible error that is associated with MeasuredValue, using the same units and resolution

### 8.3 Pressure Measurement (Cluster ID: 0x0403)

**Cluster ID: 0x0403**

**Attribute Set:**

| Id | Name | Type | Range | Acc | Def | MO |
|---|---|---|---|---|---|---|
| 0x0000 | MeasuredValue | int16 | MinMeasuredV<br>alue –<br>MaxMeasured<br>Value | RP | 0x8000 | M |
| 0x0001 | MinMeasuredValue | int16 | 0x8001-0x7ffe | R | 0x8000 | M |
| 0x0002 | MaxMeasuredValue | int16 | 0x8002-0x7fff | R | 0x8000 | M |
| 0x0003 | Tolerance | uint16 | 0x0000-<br>0x0800 | R |  | O |

### 8.4 Flow Measurement (Cluster ID: 0x0404)

**Cluster ID: 0x0404**

**Attribute Set:**

| Id | Name | Type | Range | Acc | Def | MO |
|---|---|---|---|---|---|---|
| 0x0000 | MeasuredValue | uint16 | MinMeasuredV<br>alue –<br>MaxMeasured<br>Value | RP | 0xffff | M |
| 0x0001 | MinMeasuredValue | uint16 | 0x0000-0xfffd | R | 0xffff | M |
| 0x0002 | MaxMeasuredValue | uint16 | 0x0001-0xfffe | R | 0xffff | M |
| 0x0003 | Tolerance | uint16 | 0x0000-<br>0x0800 | R |  | O |

 MeasuredValue MeasuredValue represents the pressure in kPa as follows: MeasuredValue = 10 x Pressure Where -3276.7 kPa <= Pressure <= 3276.7 kPa, corresponding to a MeasuredValue in the range 0x8001 to 0x7fff. MinMeasuredValue and MaxMeasuredValue define the range of the sensor A MeasuredValue of 0x8000 indicates that the pressure measurement is unknown MeasuredValue is updated continuously as new measurements are made.  MinMeasuredValue The MinMeasuredValue attribute indicates the minimum value of MeasuredValue that can be measured. A value of 0x8000 means this attribute is not defined  MaxMeasuredValue The MaxMeasuredValue attribute indicates the maximum value of MeasuredValue that can be measured. A value of 0x8000 means this attribute is not defined  Tolerance The Tolerance attribute SHALL indicate the magnitude of the possible error that is associated with MeasuredValue, using the same units and resolution

### 8.5 Relative Humidity Measurement (Cluster ID: 0x0405)

**Cluster ID: 0x0405**

**Attribute Set:**

| Id | Name | Type | Range | Acc | Def | MO |
|---|---|---|---|---|---|---|
| 0x0000 | MeasuredValue | uint16 | MinMeasuredV<br>alue –<br>MaxMeasured<br>Value | RP | 0xffff | M |
| 0x0001 | MinMeasuredValue | uint16 | 0x0000-0x270f | R | 0xffff | M |
| 0x0002 | MaxMeasuredValue | uint16 | 0x0001-<br>0x2710 | R | 0xffff | M |
| 0x0003 | Tolerance | uint16 | 0x0000-<br>0x0800 | R |  | O |

 MeasuredValue MeasuredValue represents the water content in % as follows: MeasuredValue = 100 x water content Where 0% <= water content <= 100%, corresponding to a MeasuredValue in the range 0 to 0x2710. The maximum resolution this format allows is 0.01% MinMeasuredValue and MaxMeasuredValue define the range of the sensor A MeasuredValue of 0xffff indicates that the measurement is unknown MeasuredValue is updated continuously as new measurements are made  MinMeasuredValue The MinMeasuredValue attribute indicates the minimum value of MeasuredValue that can be measured. A value of 0xffff means this attribute is not defined  MaxMeasuredValue The MaxMeasuredValue attribute indicates the maximum value of MeasuredValue that can be measured. A value of 0xffff means this attribute is not defined.  Tolerance The Tolerance attribute SHALL indicate the magnitude of the possible error that is associated with MeasuredValue, using the same units and resolution

### 8.6 Occupancy Sensing (Cluster ID: 0x0406)

**Cluster ID: 0x0406**

**Attribute Set:**

| Id | Name | Type | Range | Acc | Def | MO |
|---|---|---|---|---|---|---|
| 0x0000 | Occupancy | map8 | 0b0000 000x | RP |  | M |
| 0x0001 | OccupancySensorType | enum8 |  | R | ms | M |
| 0x0002 | OccupancySensorType<br>Bitmap | Map8 | 0000 0xxx | R |  | M |

 Occupancy The Occupancy attribute is a bitmap. Bit 0 specifies the sensed occupancy as follows: 1 = occupied, 0 = unoccupied. All other bits are reserved  OccupancySensorType The OccupancySensorType attribute specifies the type of the occupancy sensor. This attribute shall be set to one of the non-reserved values listed in the following table.

| Attribute Value | Description |
|---|---|
| 0x00 | PIR |
| 0x01 | Ultrasonic |
| 0x02 | PIR and ultrasonic |
| 0x03 | Physical contact |

 OccupancySensorTypeBitmap The OccupancySensorTypeBitmap attribute specifies the types of the occupancy sensor, as listed below; a ‘1’ in each bit position indicates this type is implemented.

| Bit | Description |
|---|---|
| Bit0 | PIR |
| Bit1 | Ultrasonic |
| Bit2 | PIR and ultrasonic |

The value of the OccupancySensorTypeBitmap attribute and the OccupancySensorType attribute SHALL be aligned as defined below.

| | | |
|---|---|---|
| Description | OccupancySensorType | OccupancySensorTypeBitmap |
|  | attribute | attribute |
| PIR | 0x00 | 0000 0001 |
| Ultrasonic | 0x01 | 0000 0010 |
| PIR and ultrasonic | 0x02 | 0000 0011 |
| Physical contact<br>and PIR | 0x00 | 0000 0101 |
| Physical contact<br>and ultrasonic | 0x01 | 0000 0110 |
| Physical contact<br>and PIR and<br>ultrasonic | 0x02 | 0000 0111 |

**Cluster ID: 0x0406**

**PIR Configuration Attribute Set**

| Id | Name | Type | Range | Acc | Def | MO |
|---|---|---|---|---|---|---|
| 0x0010 | PIROccupiedToUnoccu<br>piedDelay | uint16 | 0x0000-0xfffe | RW | 0x0000 | O |
| 0x0011 | PIRUnoccupiedToOccu<br>piedDelay | uint16 | 0x0000-0xfffe | RW | 0x0000 | O |
| 0x0012 | PIRUnoccupiedToOccu<br>piedThreshold | uint8 | 0x01-0xfe | RW | 0x01 | O |

 PIROccupiedToUnoccupiedDelay The PIROccupiedToUnoccupiedDelay attribute is 16 bits in length and specifies the time delay, in seconds, before the PIR sensor changes to its unoccupied state after the last detection of movement in the sensed area.  PIRUnoccupiedToOccupiedDelay The PIRUnoccupiedToOccupiedDelay attribute is 16 bits in length and specifies the time delay, in seconds, before the PIR sensor changes to its occupied state after the detection of movement in the sensed area. This attribute is mandatory if the PIRUnoccupiedToOccupiedThreshold attribute is implemented.  PIRUnoccupiedToOccupiedThreshold The PIRUnoccupiedToOccupiedThreshold attribute is 8 bits in length and specifies the number of movement detection events that must occur in the period PIRUnoccupiedToOccupiedDelay, before the PIR sensor changes to its occupied state. This attribute is mandatory if the PIRUnoccupiedToOccupiedDelay attribute is implemented.

**Cluster ID: 0x0406**

**Ultrasonic Configuration Attribute Set**

| Id | Name | Type | Range | Acc | Def | MO |
|---|---|---|---|---|---|---|
| 0x0020 | UltrasonicOccupiedTo<br>UnoccupiedDelay | uint16 | 0x0000-0xfffe | RW | 0x0000 | O |
| 0x0021 | UltrasonicUnoccupiedT<br>oOccupiedDelay | uint16 | 0x0000-0xfffe | RW | 0x0000 | O |
| 0x0022 | UltrasonicUnoccupiedT<br>oOccupiedThreshold | uint8 | 0x01-0xfe | RW | 0x01 | O |

 UltrasonicOccupiedToUnoccupiedDelay The UltrasonicOccupiedToUnoccupiedDelay attribute is 16 bits in length and specifies the time delay, in seconds, before the Ultrasonic sensor changes to its unoccupied state after the last detection of movement in the sensed area.  UltrasonicUnoccupiedToOccupiedDelay The UltrasonicUnoccupiedToOccupiedDelay attribute is 16 bits in length and specifies the time delay, in seconds, before the Ultrasonic sensor changes to its occupied state after the detection of movement in the sensed area. This attribute is mandatory if the UltrasonicUnoccupiedToOccupiedThreshold attribute is implemented.  UltrasonicUnoccupiedToOccupiedThreshold The UltrasonicUnoccupiedToOccupiedThreshold attribute is 8 bits in length and specifies the number of movement detection events that must occur in the period UltrasonicUnoccupiedToOccupiedDelay, before the Ultrasonic sensor changes to its occupied state. This attribute is mandatory if the UltrasonicUnoccupiedToOccupiedDelay attribute is implemented.

**Cluster ID: 0x0406**

**Physical Contact Configuration Set**

| Id | Name | Type | Range | Acc | Def | MO |
|---|---|---|---|---|---|---|
| 0x0030 | PhysicalContactOccupi<br>edToUnoccupiedDelay | uint16 | 0x0000-0xfffe | RW | 0x0000 | O |
| 0x0031 | PhysicalContactUnocc<br>upiedToOccupiedDelay | uint16 | 0x0000-0xfffe | RW | 0x0000 | O |
| 0x0032 | PhysicalContactUnocc<br>upiedToOccupiedThres<br>hold | uint8 | 0x01-0xfe | RW | 0x01 | O |

 PhysicalContactOccupiedToUnoccupiedDelay The PhysicalContactOccupiedToUnoccupiedDelay attribute is 16 bits in length and specifies the time delay, in seconds, before the physical contact occupancy sensor changes to its unoccupied state after detecting the unoccupied event. The value of 0xffff indicates the sensor does not report occupied to unoccupied transition.  PhysicalContactUnoccupiedToOccupiedDelay The PhysicalContactUnoccupiedToOccupiedDelay attribute is 16 bits in length and specifies the time delay, in seconds, before the physical contact sensor changes to its occupied state after the detection of the occupied event. The value of 0xffff indicates the sensor does not report unoccupied to occupied transition.  PhysicalContactUnoccupiedToOccupiedThreshold The PhysicalContactUnoccupiedToOccupiedThreshold attribute is 8 bits in length and specifies the number of movement detection events that must occur in the period PhysicalContactUnoccupiedToOccupiedDelay, before the PIR sensor changes to its occupied state. This attribute is mandatory if the PhysicalContactUnoccupiedToOccupiedDelay attribute is implemented.

## 9. Closures

### 9.1 Door Lock (Cluster ID: 0x0101)

**Cluster ID: 0x0500**

**Basic Information Attribute Set for reference**

| Id | Name | Type | Range | Access | Default | M/O |
|---|---|---|---|---|---|---|
| 0x0000 | LockState | enum8 | All | R | - | M |
| 0x0001 | LockType | enum8 | All | R | - | M |
| 0x0002 | ActuatorEnabled | bool | All | R | - | M |
| 0x0003 | DoorState | enum8 | All | R | - | O |
| 0x0004 | DoorOpenEvents | uint32 | All | RW | - | O |
| 0x0005 | DoorClosedEvents | uint32 | All | RW | - | O |
| 0x0006 | OpenPeriod | uint32 | All | RW | - | O |

**User, PIN, Schedule, Log Information Attribute Set for reference**

| Id | Name | Type | Range | Ac<br>ces<br>s | Default | M/<br>O |
|---|---|---|---|---|---|---|
| 0x0010 | NumberOfLogRecords<br>Supported | uint16 | All | R | - | O |
| 0x0011 | NumberOfTotalUsersS<br>upported | uint16 | All | R | - | O |
| 0x0012 | NumberOfPINUsersSu<br>pported | uint16 | All | R | - | O |
| 0x0013 | NumberOfRFIDUsersS<br>upported | uint16 | All | R | - | O |
| 0x0014 | NumberOfWeekDaySc<br>hedulesSupportedPer<br>User | uint8 | All | R | - | O |
| 0x0015 | NumberOfYearDaySch<br>edulesSupportedPerUs<br>er | uint8 | All | R | - | O |
| 0x0016 | NumberOfHolidaySche<br>dulesSupported | uint8 | All | R | - | O |
| 0x0017 | MaxPINCodeLength | uint8 | All | R | - | O<br>M/<br>O |
| 0x0018 | MinPINCodeLength | uint8 | All | R | - | O |
| 0x0019 | MaxRFIDCodeLength | uint8 | All | R | - | O |
| 0x001A | MinRFIDCodeLength | uint8 | All | R | - | O |

**Operational Settings Attribute Set for reference**

| Id | Name | Type | Range | Acc | Def | MO |
|---|---|---|---|---|---|---|
| 0x0020 | EnableLogging | bool | All | RW | - | O |
| 0x0021 | Language | String<br>(3byte<br>s) | All | RW | - | O |
| 0x0022 | LEDSettings | uint8 | All | RW | - | O |
| 0x0023 | AutoRelockTime | uint32 | All | RW | - | O |
| 0x0024 | SoundVolume | uint8 | All | RW | - | O |
| 0x0025 | OperatingMode | enum8 | All | RW | - | O |
| 0x0026 | SupportedOperatingMo<br>des | map16 | All | R | - | O |
| 0x0027 | DefaultConfigurationRe<br>gister | map16 | All | R | - | O |
| 0x0028 | EnableLocalProgrammi<br>ng | bool | All | RW | - | O |
| 0x0029 | EnableOneTouchLocki<br>ng | bool | All | RW | - | O |
| 0x002A | EnableInsideStatusLE<br>D | bool | All | RW | - | O |
| 0x002B | EnablePrivacyModeBut<br>ton | bool | All | RW | - | O |

**Operational Settings Attribute Set for reference**

| Id | Name | Type | Range | Acc | Def | MO |
|---|---|---|---|---|---|---|
| 0x0030 | WrongCodeEntryLimit | uint8 | All | RW | - | O |
| 0x0031 | UserCodeTemporaryDi<br>sableTime | uint8 | All | RW | - | O |
| 0x0032 | SendPINOverTheAir | bool | ALL | RW | - | O |
| 0x0033 | RequirePINforRemote<br>Operation | bool | ALL | RW | - | O |
| 0x0034 | SecurityLevel | enum8 | ALL | R | - | O |

**Operational Settings Attribute Set for reference**

| Id | Name | Type | Range | Acc | Def | MO |
|---|---|---|---|---|---|---|
| 0x0040 | AlarmMask | map16 | All | RW | - | O |
| 0x0041 | KeypadOperationEvent<br>Mask | map16 | All | RW | - | O |
| 0x0042 | RFOperationEventMas<br>k | map16 | All | RW | - | O |
| 0x0043 | ManualOperationEvent<br>Mask | map16 | All | RW | - | O |
| 0x0044 | RFIDOperationEventM<br>ask | map16 | All | RW | - | O |
| 0x0045 | KeypadProgrammingE<br>ventMask | map16 | All | RW | - | O |
| 0x0046 | RFProgrammingEvent<br>MaskRemoteProgram<br>mingEventM | map16 | All | RW | - | O |
| 0x0047 | RFIDProgrammingEve<br>ntMask | map16 | All | RW | - | O |

#### Lock Door (0x0024-0000)

This command causes the lock device to lock the door. As of HA 1.2, this command includes an optional PIN/RFID code for the lock.

**Command id**

`0x0024-0000`

**Parameter**

| 1 octet | N octets |
|---|---|
| PIN/RFID code length | PIN/RFID code |

Gateway command Example:

Header: 0xFFFCFCFF

Lengthe:0x0F

Command id: 0x00240000

Device address: 0x4721

PIN code: 0x52 0x61 0x66 0x61 0x65 0x6C (String data type, in ASCII code: Rafael) Checksum: ~(0x0F+0x00+0x00+0x24+0x00+0x21+0x47+0x00+0x02+0x06+0x52+0x61+0x66+ 0x61+0x65+0x6C) = 0x11 The gateway command will be: FF FC FC FF 0F 00 00 24 00 21 47 00 02 06 52 61 66 61 65 6C 11

| Field name | Value |
|---|---|
| Header | 0xFF 0xFC 0xFC 0xFF |
| Length | 0x0F |
| Command id | 0x00 0x00 0x24 0x00 |
| Address | 0x21 0x47 |
| Address mode | 0x00 |
| Endpoint | 0x02 |
| PIN/RFID code length | 0x06 |
| PIN/RFID code<br>Checksum | 0x52 0x61 0x66 0x61 0x65 0x6C |

#### Lock Door Response (0x0024-8000)

This command is sent in response to a Lock command with one status byte payload. The Status field SHALL be set to SUCCESS or FAILURE.

**Command id**

`0x0024-8000`

**Parameter**

| 1 octet |
|---|
| Status |

Status: 0x00, SUCCESS

0x01, FAILURE

#### Unlock Door (0x0024-0001)

This command causes the lock device to unlock the door. As of HA 1.2, this command includes an optional PIN/RFID code for the lock.

**Command id**

`0x0024-0001`

**Parameter**

| 1 octet | N octets |
|---|---|
| PIN/RFID code length | PIN/RFID code |

#### Unlock Door Response (0x0024-8001)

This command is sent in response to the unlock command with one status byte payload. The Status field SHALL be set to SUCCESS or FAILURE.

**Command id**

`0x0024-8001`

**Parameter**

| 1 octet |
|---|
| Status |

Status: 0x00, SUCCESS

0x01, FAILURE

#### Toggle (0x0024-0002)

This command causes the lock device to toggle the door lock. As of HA 1.2, this command includes an optional PIN/RFID code for the lock.

**Command id**

`0x0024-0002`

**Parameter**

| 1 octet | N octets |
|---|---|
| PIN/RFID code length | PIN/RFID code |

#### Toggle Response (0x0024-8002)

This command is sent in response to the toggle command with one status byte payload. The Status field SHALL be set to SUCCESS or FAILURE.

**Command id**

`0x0024-8002`

**Parameter**

| 1 octet |
|---|
| Status |

Status: 0x00, SUCCESS

0x01, FAILURE

#### Set PIN Code (0x0024-0005)

Set a PIN into the lock.

**Command id**

`0x0024-0005`

**Parameter**

| 2 octets | 1 octet | 1 octet | 1 octet | N octets |
|---|---|---|---|---|
| User ID | User status | User type | PIN code length | PIN code |

#### Set PIN Code Response (0x0024-8005)

This command is sent in response to the set pin code command with one status byte payload. The Status field SHALL be set to SUCCESS or FAILURE.

**Command id**

`0x0024-8005`

**Parameter**

| 1 octet |
|---|
| Status |

Status: 0x00, SUCCESS

0x01, FAILURE

#### Get PIN Code (0x0024-0006)

Set a PIN into the lock.

**Command id**

`0x0024-0006`

**Parameter**

| 2 octets |
|---|
| User ID |

#### Get PIN Code Response (0x0024-8006)

This command is sent in response to the get pin code command with following payload.

**Command id**

`0x0024-8006`

**Parameter**

| 2 octets | 1 octet | 1 octet | 1 octet | N octets |
|---|---|---|---|---|
| User ID | User status | User type | PIN code length | PIN code |

#### Clear PIN Code (0x0024-0007)

Delete a PIN code associated with a specific user ID from the lock.

**Command id**

`0x0024-0007`

**Parameter**

| 2 octets |
|---|
| User ID |

#### Clear PIN Code Response (0x0024-8007)

This command is sent in response to the clear pin code command with one status byte payload. The Status field SHALL be set to SUCCESS or FAILURE.

**Command id**

`0x0024-8007`

**Parameter**

| 1 octet |
|---|
| Status |

Status: 0x00, SUCCESS

0x01, FAILURE

#### Clear All PIN Code (0x0024-0008)

Delete all PIN codes from the lock.

**Command id**

`0x0024-0008`

**Parameter**

None.

#### Clear All PIN Code Response (0x0024-8008)

This command is sent in response to the clear all pin code command with one status byte payload. The Status field SHALL be set to SUCCESS or FAILURE.

**Command id**

`0x0024-8008`

**Parameter**

| 1 octet |
|---|
| Status |

Status: 0x00, SUCCESS

0x01, FAILURE

#### Operating Event Notification (0x0024-8020)

The door lock server sends out operation event notification when the event is triggered by the various event sources. The specific operation event will only be sent out if the associated bitmask is enabled in the various attributes in the Event Masks Attribute Set.

**Command id**

`0x0024-8020`

**Parameter**

| 1 octet | 1 octet | 2 octet | 1 octet | 4 octet | 0/N octet |
|---|---|---|---|---|---|
| Operation<br>Event<br>Source | Operation<br>Event<br>Code | User ID | PIN | ZigBeeLocalTime | Data |

Operation Event Source: This field indicates where the event was triggered from.

| | |
|---|---|
| Value | Operation Event Source |
| 0x00 | keypad |
| 0x01 | RF |
| 0x02 | Manual |
| 0x03 | RFID |
| 0xFF | Indeterminate |

Operation Event Code: The door lock optionally sends out notifications (if they are enabled) whenever there is a significant opera tional event on the lock. When combined with a source from the Event Source table above, the following operational event codes

| | |
|---|---|
| Value | Code |
| 0x00 | UnknownOrMfgSpecific |
| 0x01 | Lock |
| 0x02 | Unlock |
| 0x03 | LockFailureInvalidPINorID |
| 0x04 | LockFailureInvalidSchedule |
| 0x05 | UnlockFailureInvalidPINorID |
| 0x06 | UnlockFailureInvalidSchedule |
| 0x07 | OneTouchLock |
| 0x08 | KeyLock |
| 0x09 | KeyUnlock |
| 0x0A | AutoLock |
| 0x0B | ScheduleLock |
| 0x0C | ScheduleUnlock |
| 0x0D | Manual Lock (Key or Thumbturn) |
| 0x0E | Manual Unlock (Key or Thumbturn) |
| 0x0F | Non-Access User Operational Event |

User ID: The User ID who performed the event.

PIN: The PIN that is associated with the User ID who performed the event.

LocalTime: The ZigBee LocalTime that indicates when the event is triggered.

Data: The operation event notification command contains a variable string,

which can be used to pass data asso ciated with a particular event. Generally this field will be left empty. However, manufacturer can choose to use this field to store/display manufacturer-specific information.

## 10. Security and Safety

### 10.1 IAS Zone (Cluster ID: 0x0500)

**Cluster ID: 0x0500**

**Attribute Set**

| Id | Name | Type | Range | Access | Default | M/O |
|---|---|---|---|---|---|---|
| 0x0000 | ZoneState | Enum8 | All | R | 0x00 | M |
| 0x0001 | ZoneType | Enum16 | All | R | - | M |
| 0x0002 | ZoneStatus | Map16 | All | R | 0x00 | M |

 Zone State Attribute

| Attribute Value | Meaning |
|---|---|
| 0x00 | Not enrolled |
| 0x01 | Enrolled |

 Zone Type Attribute

| | | | |
|---|---|---|---|
| Value | Zone Type | Alarm1 | Alarm2 |
| 0x0000 | Standard CIE | System Alarm | - |
| 0x000d | Motion sensor | Intrusion indication | Presence<br>indication |
| 0x0015 | Contact switch | 1st portal Open-<br>Close | 2nd portal Open-<br>Close |
| 0x0028 | Fire sensor | Fire indication | - |
| 0x002a | Water sensor | Water overflow<br>indication | - |
| 0x002b | Carbon Monoxide<br>(CO) sensor | CO indication | Cooking indication |
| 0x002c | Personal<br>emergency device | Fall/Concussion | Emergency button |
| 0x002d | Vibration/Movement<br>sensor | Movement indication | Vibration |
| 0x010f | Remote Control | Panic | Emergency |
| 0x0115 | Key fob | Panic | Emergency |
| 0x021d | Keypad | Panic | Emergency |
| 0x0225 | Stand Warning<br>Device | - | - |
| 0x0226 | Glass break sensor | Glass breakage<br>detected | - |
| 0x0229 | Security repeater | - | - |
| 0x8000-<br>0xfffe | Manufacturer<br>specific types | - | - |
| 0xffff | Invalid Zone Type | - | - |

#### Zone Status Change Notification (0x0023-0000)

**Command id**

`0x0023-0000`

**Parameter**

| 2 octets | 1 octet | 1 octet | 2 octets |
|---|---|---|---|
| Zone Status | Extended Status | Zone ID | Delay |

 Zone Status Attribute

| Attribute<br>Bit<br>Number | Meaning | Values |
|---|---|---|
|  | Alarm1 | 1 – opened or alarmed<br>0 – closed or not alarmed |
|  | Alarm2 | 1 – opened or alarmed<br>0 – closed or not alarmed |
|  | Tamper | 1 – Tampered<br>0 – Not tampered |
|  | Battery | 1 – Low battery<br>0 – Battery OK |
|  | Supervision<br>Reports (Note 1) | 1 – Reports<br>0 – Does not report |
|  | Restore reports<br>(Note 2) | 1 – Reports restore<br>0 – Does not report restore |
|  | Trouble | 1 – Trouble/Failure<br>0 – OK |
|  | AC (mains) | 1 – AC/Mains fault<br>0 – AC/Mains OK |
|  | Test | 1 – Sensor is in test mode<br>0 – Sensor is in operation mode |
|  | Battery Defect | 1 – Sensor detects a defective battery<br>0 – Sensor battery is functioning<br>normally |

 The Extended Status field is reserved for additional status information and SHALL be set to zero

## 11. Custom cluster

### 11.1 Custom command (0xFC00-0000)

The command is used to generate manufacture specific command. Note that gateway must support corresponding cluster to make this command works properly

**Command id**

`0xFC00-0000`

**Parameter**

| 2 octets | 2 octets | 1 octet | 1 octet | Variable |
|---|---|---|---|---|
| Cluster Id | Manufacture<br>code | Command ID | Payload length | Payload |

### 11.2 Custom command response (0xFC00-8000)

The command is generated when gateway received manufacture specific command from device. Note that gateway must support corresponding cluster to make this command works properly

**Command id**

`0xFC00-8000`

**Parameter**

| 2 octets | 2 octets | 1 octet | 1 octet | Variable |
|---|---|---|---|---|
| Cluster Id | Manufacture<br>code | Command ID | Payload<br>length | Payload |

## 12. Application Service Management Status Enumeration Description

| Enumeration | Value | Description |
|---|---|---|
| SUCCESS | 0x00 | Operation was successful. |
| FAILURE | 0x01 | Operation was not successful. |
| NOT_AUTHORIZED | 0x7E | The sender of the command does not<br>have authorization to carry out this<br>comma |
| MALFORMED_COMMAND | 0x80 | The command appears to contain the<br>wrong fields, as detected either by the<br>presence of one or more invalid field<br>entries or by there being missing<br>fields. Command not carried out.<br>Implementer has discretion as to<br>whether to return this error or<br>INVALID_FIELD. |
| UNSUP_COMMAND | 0x81 | The specified command is not<br>supported on the device. Command<br>not carried out. |
| INVALID_FIELD | 0x85 | At least one field of the command<br>contains an incorrect value, according<br>to the specification the device is<br>implemented to. |
| UNSUPPORTED_ATTRIBUTE | 0x86 | The specified attribute does not exist<br>on the device. |
| INVALID_VALUE | 0x87 | Out of range error or set to a reserved<br>value. Attribute keeps its old value.<br>Note that an attribute value may be<br>out of range if an attribute is related to<br>another, e.g., with minimum and<br>maximum attributes. See the individual<br>attribute descriptions for specific<br>details |
| READ_ONLY | 0x88 | Attempt to write a read-only attribute. |
| INSUFFICIENT_SPACE | 0x89 | An operation failed due to an<br>insufficient amount of free space<br>available. |
| NOT_FOUND | 0x8B | The requested information (e.g., table<br>entry) could not be found. |
| UNREPORTABLE_ATTRIBUTE | 0x8C | Periodic reports cannot be issued for<br>this attribute. |
| INVALID_DATA_TYPE | 0x8D | The data type given for an attribute is<br>incorrect. Command not carried out. |
| INVALID_SELECTOR | 0x8E | The selector for an attribute is<br>incorrect. |
| TIMEOUT | 0x94 | The exchange was aborted due to<br>excessive response time. |
| ABORT | 0x95 | Failed case when a client or a server<br>decides to abort the upgrade process. |
| INVALID_IMAGE | 0x96 | Invalid OTA upgrade image (ex. failed<br>signature validation or signer<br>information check or CRC check) |
| WAIT_FOR_DATA | 0x97 | Server does not have data block<br>available yet |
| NO_IMAGE_AVAILABLE | 0x98 | No OTA upgrade image available for<br>the client |
| REQUIRE_MORE_IMAGE | 0x99 | The client still requires more OTA<br>upgrade image files to successfully<br>upgrade |
| NOTIFICATION_PENDING | 0x9A | The command has been received and<br>is being processed |
| UNSUPPORTED_CLUSTER | 0xC3 | The cluster is not supported |
