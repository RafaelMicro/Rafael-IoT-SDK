/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * http://www.dsr-zboss.com
 * http://www.dsr-corporation.com
 * All rights reserved.
 *
 * This is unpublished proprietary source code of DSR Corporation
 * The copyright notice does not evidence any actual or intended
 * publication of such source code.
 *
 * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
 * Corporation
 *
 * Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and
 * DSR.
 */

# platform_cc2538

## The goal of adding CC2538 platform into ZOI

CC2538 support was added into ZBOSS long time ago, out of ZOI codebase.
ZOI probmary reference platform is Network Simulator which is great tool but not the real radio.
We have in ZOI support for modern TI platforms, but it requires an RTOS and works via sophysticated intermediate radio layer provided by TI SDK.
It may or may not be a good example for HW vendor who wants to add new platform into ZOI.
CC2538 is an example of a bare-metal platform which has radio controlled by directly accessing radio registers.

So the goal of this port into ZOI is to use CC2538 platform as a reference of radio which controlled directly by using registers.


## ZBOSS ZOI platform for TI CC2538

### Features

- All Zigbee roles
- Zigbee 3.0 and SE - _SE to be verified_
- Green power infrastructure (GPPB, GPCB) - _to be verified!_
- Green Power Device (debug implementation based on ZBOSS MAC - not for production use) - _to be verified!_
- Debug trace over UART
- Support of LEDs and buttons at SmartRF06 devboard

### Limitations

- No power saving implemented: neither radio off nor MCU put asleep at sleepy ZED
- Injected LEAVE feature is implemented using a "sticky pending bit": parent always set Pending bit ON in MAC ACKs to POLL packets.

#### About "sticky pending bit"

Sleepy ZED in Zigbee periodically polls parent. Parent sends MAC ACK to POLL packet wirh or without Pending bit.
If Pending bit is not set, ZED goes asleep immediately,
If Pending bit is set, ZED waits for data packet from the Parent, or for empty Data packet.
CC2538 radio has a feature of auto-sending MAC ACKs. It also has data structures which allows Parent to set Pending bit
only if Parent has some data to send to ZED.
But that is not enough.
According to Zigbee >= r21, Parent must also set Pending bit to 1 to be able to send "injected LEAVE" when unknown (or forgotten)
Child polls.
So, Parent must send Pending bit if it has some data for that Child or if it does not know that Child.
The later part of logic is absent at CC2538.
In the current implementation ZBOSS at CC2538 always set Pending bit 1 and then, if nothing is to be sent, sends empty Data packet to ZED.
that is legal behavior, but according to our experiments it decreases ZED battery lifetime ~ 1.5 times.

ZBOSS has pure soft implementation of Pending bit definition logic. See nsng as an example of the implementation.
But its usage at CC2538 requires switching off auto MAC ACK tx and sending MAC ACKs manually.
Maybe, we will do it somewhere in time if CC2538 platform will be used in some real project. But not now.


### Known issues

- CC2538 without an external antenna has low LQI in received packets. As a result, joining device (ZR or ZED) may bot want
to join. Use an antenna or put devices close to each other.
- Debug trace over serial sometimes garbaged

### TODO

- Implement official ZB 3.0 SDK
- Implement SE SDK
- Implement a sniffer based in 2538

### Compiler

Can be compiled either by IAR (quite old build, Windows only, lot of $$$) or gcc (Free, but no debugger).
TODO: migrate to TI CCS at Windows/Linux (if ever need it).
See additional information in doc/setup.txt.
