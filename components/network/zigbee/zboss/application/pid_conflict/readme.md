    ZBOSS Zigbee software protocol stack

    Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
    www.dsr-zboss.com
    www.dsr-corporation.com
    All rights reserved.

    This is unpublished proprietary source code of DSR Corporation
    The copyright notice does not evidence any actual or intended
    publication of such source code.

    ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
    Corporation

    Commercial Usage
    Licensees holding valid DSR Commercial licenses may use
    this file in accordance with the DSR Commercial License
    Agreement provided with the Software or, alternatively, in accordance
    with the terms contained in a written agreement between you and
    DSR.


PAN ID conflict resolution
==========================

Summary
-------

This set of applications demonstrates user defined option to enable
or disable PAN ID conflict resolution.
The set contains 5 applications:

  - 2x Zigbee Coordinator:
      ZC1 - The ZC which starts an NWK and is capable of handling
            PAN ID conflicts, both by receiving a NWK Report or
            by itself (when it realizes of the PAN ID conflict event).
      ZC2 - The ZC which initiates a NWK with the same PAN ID of an
            existing NWK in the vicinity.
  - 3x Zigbee Routers:
      ZR1 - A router which joins ZC1's NWK. It is not capable to handle
            PAN ID conflict. It is expressively disabled to, which is
            unnecessary (since PAN ID conflict is disabled by default),
            but used here for demonstration purposes.
      ZR2 - The same as ZR1, but capable of handling PAN ID conflicts.
      ZR3 - A router used to trigger PAN ID conflict. No other purpose.

This sample shows a possible scenario for PAN ID conflict and its resolution,
if enabled. Therefore, it is possible to see and extrapolate benefits/consequences for cases when resolution is enabled or disabled.


The application set structure
------------------------------

 - Makefile
 - zc1.c - *Application of a Zigbee Coordinator capable of resolving PAN ID conflicts*
 - zc2.c - *Application of a Zigbee Coordinator which uses 'a priori' knowledge to start a NWK with the same PAN ID of an already existing one in its vicinity*
 - zr1.c - *Application of a Zigbee Router not capable of resolving PAN ID conflicts*
 - zr2.c - *Application of a Zigbee Router capable of resolving PAN ID conflicts*
 - zr3.c - *Application of a Zigbee Router used to trigger the PAN ID conflict.*
 - readme.md - *This file*
 - runng.sh - *Script for running setup on Network Simulator*
 - nodes_location.cfg - *Node location featuring possibility to 'hide' devices from each other: a away of selecting which device becomes aware of the PAN ID conflict first*
 - pid_conflict_common.h - *Small set of common definitions for all apps*
 

Applications behavior
---------------------

- NWK Formation:
- Zigbee Coordinator ZC1 creates network on the 21 channel. PAN ID is set to 0x1aaa.
- ZR1 and ZR2 join ZC1, fulfilling ZC1's capacity, which is defined to 2 children. Both ZR1 and ZR2 carry no children capacity. 
- In order to assure that the NWK's operation is correct, ZC1 sends 'Buffer Test
  Request', for which it gets response from the devices in PAN (ZR1 and ZR2).
- This, will also be used for a later stage, after the conflict and resolution take effect, in order to demonstrate the NWK state for devices that support or do
  not support PAN ID conflict resolution.
  
- PAN ID conflict:
- ZC2 boots with predefined PAN ID equal to 0x1bbb. A few moments later, it changes
  PAN ID to 0x1aaa (the same as the already existing NWK in its vicinity).
- ZR3 boots, tries to join some NWK which triggers the PAN ID conflict report
  by ZR2.

- PAN ID conflict resolution:
- ZC1 selects and new PAN ID and updates it by means of NWK Update command.
- ZR1 does not handle the conflict, therefore does not update to new PAN ID.
- ZR2 does handle the conflict, updating to new PAN ID.

- After resolution:
- Like before the conflict, ZC1 broadcasts PAN devices the 'Buffer Test Request'.
- ZR1 does not answer the request.
- ZR2 does.
