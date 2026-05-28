# What is the ot-cli-mtd-sleep example?

The ot-cli-mtd-sleep example demonstrates a Thread `MTD (Minimal Thread Device)`, which includes `SED (Sleepy End Device)` and `SSED (Synchronized Sleepy End Devices)`. In a Thread network, the Thread MTD can only act as a `Child (End Device)`, making it suitable for `power-saving` applications.

Since ot-cli-mtd-sleep has wake/sleep functionality, you should set the network parameters directly in the code. Refer to the `otrInitUser` function in `app_task.c` for details. Users need to configure the network parameters before flashing the code to the device.

```c
/*set network key*/
otThreadSetNetworkKey(instance, (const otNetworkKey*)tempStr);

/*set channel*/
otLinkSetChannel(instance, 8);

/*set panid*/
otLinkSetPanId(instance, 0x1b95);
sprintf(tempStr, "OpenThread-%x", 0x0bfe);

/*set network name*/
otThreadSetNetworkName(instance, (const char*)tempStr);
````

The ot-cli-mtd-sleep can be configured to use either the 2.4GHz or Sub-GHz bands for physical layer transmission, and it supports Thread UDP for application development.

    
