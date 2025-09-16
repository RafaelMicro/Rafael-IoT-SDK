
# miu-router Example

## What is the miu-router Example?

The `miu-router` example corresponds to the OpenThread FTD (Full Thread Device). On top of this, it integrates SubG for automatic mesh networking and related applications. This example demonstrates how to configure network parameters, use commands, and establish communication between devices.

---

## Network Parameter Settings

You can find the default network configuration in `app_task.c` inside the `otdatasetInit` function. The example uses the following structure:

```c
AppNetworkConfig netconfig = {
    .networkName = "Rafael Miu",
    .extPanId = {0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00},
    .networkKey = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                   0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff},
    .meshLocalPrefix = {0xfd, 0x00, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00},
    .pskc = {0x74, 0x68, 0x72, 0x65, 0x61, 0x64, 0x6a, 0x70,
             0x61, 0x6b, 0x65, 0x74, 0x65, 0x73, 0x74, 0x00},
    .channel = 1,
    .panId = 0xabcd
};
```

If this is your first time running the device, it's recommended to reset flash using the following command to ensure the default parameters are used:

```bash
ot factoryreset
```

Otherwise, the device will retain previously saved parameters in flash.

---

## Getting Started

### Device 1

Upon booting, you will see the following output showing the default configuration:

```
Mesh It Up Router
Band               : SubG_915M
Data Rate          : 300K
Active Timestamp   : 1
Channel            : 1
Wake-up Channel    : 0
Ext PAN ID         : 000db80000000000
Mesh Local Prefix  : fd00:0db8:0000:0000::/64
Network Key        : 00112233445566778899aabbccddeeff
Network Name       : Rafael Miu
Link Mode          : 1, 1, 1
PAN ID             : 0xabcd
Extaddr            : 5600000000000000
UDP PORT           : 0x162e
```

Then check the network role:

```
Current role     : detached
```

Use the following command to promote the device to a leader:

```bash
ot state leader
```

Once successful:

```
Current role       : leader
Rloc16             : 4000
Extend Address     : 5600000000000000
RLOC IPv6 Address  : fd00:db8:0:0:0:ff:fe00:4000
Mesh IPv6 Address  : fd00:db8:0:0:5600:0:0:0
local IPv6 Address : fe80:0:0:0:5400:0:0:0
```

---

### Device 2

Booting will display similar information:

```
Mesh It Up Router
Band               : SubG_915M
Data Rate          : 300K
Active Timestamp   : 1
Channel            : 1
Wake-up Channel    : 0
Ext PAN ID         : 000db80000000000
Mesh Local Prefix  : fd00:0db8:0000:0000::/64
Network Key        : 00112233445566778899aabbccddeeff
Network Name       : Rafael Miu
Link Mode          : 1, 1, 1
PAN ID             : 0xabcd
Extaddr            : 0200000000000000
UDP PORT           : 0x162e
```

Initially:

```
Current role     : detached
```

After forming a mesh network:

```
Current role       : child
Rloc16             : 4002
Extend Address     : 0200000000000000
RLOC IPv6 Address  : fd00:db8:0:0:0:ff:fe00:4002
Mesh IPv6 Address  : fd00:db8:0:0:200:0:0:0
local IPv6 Address : fe80:0:0:0:0:0:0:0
```

---

## UDP Communication

You can send data from Device 1 to Device 2 using UDP:

```bash
app udp send fd00:db8:0:0:200:0:0:0 -x 123456
```

Or trigger LED actions:

```bash
app udp send fd00:db8:0:0:200:0:0:0 -c app led toggle
```

### Available Application Commands

```bash
app help
app udp send <ipv6> -x <hex data>
app udp send <ipv6> -c <string data>
app udp port
app led <on/off/toggle/flash>
```

---

## Setting Network via CLI

You can also configure the network manually using the OpenThread CLI.

```bash
ot dataset panid 0x1234
ot dataset commit active
```

These settings will be stored in flash memory and automatically loaded after a reboot.

> 📚 **For more advanced usage:**  
> Refer to the full OpenThread [dataset CLI documentation](../../../../components/network/mesh-it-up/miu_openthread/src/cli/README_DATASET.md) for additional commands like `networkkey`, `channel`, `extpanid`, etc.

> ❗ **Reminder:**  
> When using OpenThread [commands](../../../../components/network/mesh-it-up/miu_openthread/src/cli#readme), remember to prefix them with `ot`.  
> For example: `ot state`, `ot dataset`, `ot factoryreset`
