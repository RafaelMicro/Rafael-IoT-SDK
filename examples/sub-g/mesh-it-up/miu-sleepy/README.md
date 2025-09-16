
# miu-sleep Example

## What is the miu-sleep Example?

The `miu-sleep`  example corresponds to the OpenThread MTD (Minimal Thread Device). On top of this, it integrates SubG for automatic mesh networking and related applications. This example demonstrates how to configure network parameters and observe communication behavior for sleepy devices.

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
Note: Since `miu-sleep` is a sleepy device, it does not provide command-line configuration. All settings should be defined in the code as shown above.

## Boot Log Example

Upon boot, the device displays the following information:
```
Mesh It Up Sleepy
Band               : SubG_915M
Data Rate          : 300K
Active Timestamp   : 1
Channel            : 1
Wake-up Channel    : 1
Ext PAN ID         : 000db80000000000
Mesh Local Prefix  : fd00:0db8:0000:0000::/64
Network Key        : 00112233445566778899aabbccddeeff
Network Name       : Rafael Miu
Link Mode          : 0, 0, 0
PAN ID             : 0xabcd
Extaddr            : 0200000000000000
UDP PORT           : 0x162e
```

The device initially joins the network with the role:

Current role     : detached

Once it successfully joins the network, the role will change:
```
Current role       : child
Rloc16             : 4002
Extend Address     : 0200000000000000
RLOC IPv6 Address  : fd00:db8:0:0:0:ff:fe00:4002
Mesh IPv6 Address  : fd00:db8:0:0:200:0:0:0
local IPv6 Address : fe80:0:0:0:0:0:0:0
```
This demonstrates the successful operation and role transition of a sleepy end device in the MIU network.

## Application Behavior
As a sleepy device, miu-sleepy does not provide CLI commands for network configuration. Most of its behavior is defined directly in the application logic.

Users can manually trigger transmissions using hardware buttons.