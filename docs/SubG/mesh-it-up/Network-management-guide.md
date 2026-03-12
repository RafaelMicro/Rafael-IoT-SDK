# Network Management Guidance

## 1. Overview

This document describes the application-layer mechanism for maintaining and managing the network topology **after a Thread network has been successfully formed**.

**Important Prerequisite:** The management mechanism detailed herein operates on the premise that all devices (including the Leader, Routers, and Children) have already completed the standard **Thread commissioning process** and have successfully joined the network. This document **does not cover** the initial network commissioning procedure.

The core of this mechanism is to manage devices within the network through application-layer control commands (`app ctrl command`). The primary functions include **Enrollment**, **Status Update**, **Device Limitation**, and **Forced Removal (Kick)**.

### 1.1. Transport Protocol

All management packets described in this document, specifically the `ctrl_packet_t`, are transmitted using the **User Datagram Protocol (UDP)** as the transport layer protocol.

* **Communication Method:** These packets are carried as the payload of UDP datagrams exchanged between nodes.
* **Port:** All devices participating in this management mechanism must listen for and transmit on a predefined, common UDP port to ensure commands are received correctly.
* **Positioning:** This is a custom application-layer management protocol that runs on top of the Thread network stack.

### 1.2. Core Roles and Flow

* **Leader:** Maintains a comprehensive Network Management Table, which is accessible to administrators for monitoring and operational tasks. The Leader periodically synchronizes its Child Table.
* **Router:** Periodically aggregates its own status and its Child Table, sending this information to the Leader via an `Enroll Update` packet.
* **All Nodes (Router/Child):** Upon a role change (e.g., a Child is promoted to a Router), the device will immediately send an `Enroll Request` to ensure the Leader has the most current network topology.

### Figure 1: Network Management Flow Overview
  <p align="center">
    <img src="../../picture/miu-network management-flow.png" alt="Flow Overview" width="600"/>
  </p>
  
## 2. Base Packet Format

All management packets are based on the following `ctrl_packet_t` structure and are encapsulated within the UDP payload.

```c
typedef struct {
    uint8_t start;       /* Start byte, always 0xAA */
    uint8_t seq;         /* Sequence number for packet tracking */
    uint8_t flags;       /* Flags for extending functionality */
    uint8_t cmd;         /* Command ID, defines the specific operation */
    uint16_t len;        /* Payload length of the 'data' field */
    uint8_t data[MAX_DATA_LEN]; /* The actual management data payload */
    uint8_t crc;         /* CRC checksum */
} __attribute__((packed)) ctrl_packet_t;
```

The data field of the ctrl_packet_t is populated with one of the management structures below, corresponding to the cmd.

## 3. Network Management Commands
### 3.1. Enroll Request
* **Purpose:** Sent to the Leader by a device to register its information when it first joins the network or when its role changes (e.g., Child to Router).
* **Triggers:**
* Upon a device role change.
* When a device reboots and rejoins the network.
* **Payload Structure (net_mgm_enroll_req_t):**

```c
typedef struct {
    uint8_t role;                               /* The device's current role (Router/Child) */
    uint16_t parent;                             /* RLOC16 of its parent */
    uint16_t self_rloc;                          /* Its own RLOC16 */
    uint8_t self_extaddr[OT_EXT_ADDRESS_SIZE];   /* Its own EUI-64 extended address */
    int8_t rssi;                                 /* Received Signal Strength Indicator (RSSI) from its parent */
    uint32_t version;                            /* Firmware or application version number */
} __attribute__((packed)) net_mgm_enroll_req_t;
```

## 3.2. Enroll Response
* **Purpose:** Sent by the Leader in response to an Enroll Request or Enroll Update. It confirms the processing status and provides network configuration.
* **Payload Structure (net_mgm_enroll_resp_t):**

```c
typedef struct {
    int status;                 /* Processing status (e.g., 0 for success, others for error codes) */
    uint16_t provision_time;    /* (In seconds) The remaining time the receiving Router is permitted to keep its provisioning (steering) active for new devices to join. It should be disabled after this time expires. */
} __attribute__((packed)) net_mgm_enroll_resp_t;
```
The "provision_time" field is used to control network expansion. By setting this value, the Leader can indirectly limit the total number of devices in the network by managing how long Routers can accept new nodes.

## 3.3. Enroll Update
* **Purpose:** Periodically sent by a Router to the Leader to report its own status and provide a summary of all its connected Child nodes. This allows the Leader to maintain a synchronized view of the entire network topology.
* **Triggers:** This update is sent periodically by Router nodes.
* **Payload Structure (net_mgm_enroll_update_t):**

```c
/* Describes information for a single Child node */
typedef struct {
    uint16_t rloc;      /* RLOC16 of the Child node */
    int8_t rssi;        /* RSSI of the signal received from this Child */
} __attribute__((packed)) net_mgm_child_update_info_t;

/* Main structure for the Enroll Update */
typedef struct {
    uint16_t parent;                    /* RLOC16 of its parent */
    uint16_t self_rloc;                 /* The Router's own RLOC16 */
    int8_t self_rssi_from_parent;       /* RSSI of the signal received from its parent */
    uint16_t child_cnt;                 /* The number of attached Child nodes */
    net_mgm_child_update_info_t
        children[64];                   // Follows OPENTHREAD_CONFIG_MLE_MAX_CHILDREN
} __attribute__((packed)) net_mgm_enroll_update_t;
```

## 3.4. Kick
* **PPurpose:** Actively sent by the Leader to a specific device, instructing it to leave the network after a specified duration. This can be used to remove malfunctioning or unauthorized devices.
* **PTriggers:** Issued by an administrator or a central controller via the Leader.
* **PPayload Structure (net_mgm_kick_t):**

```c
typedef struct {
    uint16_t leave_time;    /* (In seconds) The time within which the device must leave the network after receiving this command. A value of 0 means leave immediately. */
} __attribute__((packed)) net_mgm_kick_t;
```