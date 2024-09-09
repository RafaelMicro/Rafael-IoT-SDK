# Smart Energy devices samples

This directory contains samples of next SE devices:
* Metering devices (Electric and Gas)
* In-home Display device
* Energy Service Interface device

All devices support Key Establishment cluster (CryptoSuite 1 and CryptoSuite 2).

## Cryptosuites support in the tests

ZBOSS itself supports both cryptosuite 1 and 2.
But since applications uses certificates got from SE specification which has different BDAs for CS1 and CS2,
it is impossible to support both cryptosuites at once.
So tests must be compiled supporting either cryptosuite 1 or 2.
To change cryptosuite cd scripts and run switch_to_cs1.sh or switch_to_cs2.sh then
cd .. and type 'make rebuild'.

## How to start the test

To run SE test:
 $ cd scripts
 $ ./TEST_NAME

After test completed, use wireshark to explore .pcap files.

## Metering device

Metering devices are Electricity and Gas meter. They act as ZigBee End Devices,
generates new measurement once a second and accumulates it in Metering cluster attributes.

> ZSE spec, subclause D.3.2.2.1.1
>    CurrentSummationDelivered represents the most recent summed value of Energy, Gas,
>    or Water delivered and consumed in the premises. CurrentSummationDelivered is updated
>    continuously as new measurements are made.
>
> ZSE spec, subclause D.3.2.2.5.1:
>    InstantaneousDemand is updated continuously as new measurements are made.
>    The frequency of updates to this field is specific to the metering device, but
>    should be within the range of once every second to once every 5 seconds.

Devices are running as a Metering cluster server: it supports all mandatory attributes and provides
access to these attributes via Read Attributes command.

InstantaneousDemand is generated once a second and its value is accumulated in
CurrentSummationDelivered attribute.

Metering devices are configured to automatically report CurrentSummationDelivered and MeteringStatus
attributes within 30-45 sec interval.

Metering devices support Keep-Alive cluster and do Trust Center Keep-Alive check automatically.

## In-Home Display device

In-Home Display acts as ZigBee Router. After successfull join/restart already joined and then once
every ~3 hrs it starts SE Service Discovery procedure:
- find any device with complimentary server role cluster which suggests "asynchronous event
commands" (Metering, Price, DRLC clusters)
- if multiple-commodity network mode enabled - for Metering and Price clusters check commodity type
- bind this cluster on remote device as: source is remote device, destination is In-Home Display
- etablish partner link keys with remote device (via Trust Center)
After Service Discovery remote device should send "asynchronous event commands" (like attribute reporting, DRLC commands etc) to In-Home Display automatically.

For Metering cluster In-Home Display additionally discover:
* Unit of Measure
* Summation Formatting
These attributes are used to display metering measurements.

In-Home Display supports Keep-Alive cluster and does Trust Center Keep-Alive check automatically.

## Energy Service Interface device

Energy Service Interface acts as ZigBee Coordinator and Trust Center. Besides general Trust Center
functionality (Key Establishment, Keep-Alive etc) it supports Time, Price and DRLC server roles.

## General scheme of 3 device combination work:
- Energy Service Interface (ESI) starts SE network
- Metering device joins to network, do Key Establishment
- In-Home Display (IHD) joins to network, do Key Establishment and Service Discovery:
-- bind DRLC and Price clusters from ESI
-- bind Metering cluster from Metering device
-- establish partner link keys with Metering device
- Metering device reports Metering data to IHD (encrypted with partner key)
- IHD periodically send Get Current Price command, ESI answers with Publish Price
- IHD periodically restarts Service Discovery (once every ~3 hrs)
- ESI periodically send DRLC events to IHD
- IHD and Metering device periodically do Trust Center Keep-Alive check

## CC2652 demo scenario:
>> Startup
0. (Wireshark) To allow Wireshark to decrypt data add APS key generated from installcode.
For default installcode
   966b9f3ef98ae605 (CRC 9708)
it is
   ca 48 26 7e f8 a7 6b 6b 40 91 04 41 90 b4 fa f1
1. Flash ESI sample on 1st CC2652 board. Configure remote device's addresses that will be joined
(IHD, Metering device) and set certificates/installcodes for them (using production config).
2. Power on ESI board with pressed Button 1 and Button 2 - perform NVRAM erase on startup.
Red Led turned on for ~1 sec indicates that NVRAM will be erased.
- Energy Service Interface (ESI) starts SE network
3. Flash IHD sample on 2st CC2652 board. Configure Trust Center certificate and installcode
(using production config).
4. Power on IHD board with pressed Button 1 and Button 2 - perform NVRAM erase on startup.
Red Led turned on for ~1 sec indicates that NVRAM will be erased.
5. Flash Metering sample on 3st CC2652 board. Configure Trust Center certificate and installcode
(using production config).
6. Power on Metering board with pressed Button 1 and Button 2 - perform NVRAM erase on startup.
Red Led turned on for ~1 sec indicates that NVRAM will be erased.

>> Joining to the network
7. When all devices are ready, press Button 1 on ESI device to open network for joining.
Green Led blinks slow during network is open for joining.
8. Press Button 1 on Metering device - start joining.
Green Led blinks slow on start, then turned on for ~2 sec indicating that join is successfull.
- Metering device joins to network, do Key Establishment
9. (Trace) Get APS key generated from CBKE from ESI or Metering device text trace
314     871/13.378560  zse_kec_cluster.c:1484  TC side of CBKE - APS key: df:64:72:08:e9:43:f7:88:5b:cc:74:b5:ff:06:ab:af
or
235	 6594/101.283840	zse_kec_cluster.c:2335	Client side of CBKE - APS key: 69:2b:ef:71:5c:e0:c6:e6:88:35:b5:a3:50:10:b8:d8
10. Press Button 1 on IHD - start joining.
Green Led blinks slow on start, then turned on for ~2 sec indicating that join is successfull.
- In-Home Display (IHD) joins to network, do Key Establishment
11. (Trace) Get APS key generated from CBKE from ESI or IHD text trace.
12. (Wireshark) Check ZCL Key Establishment packets - Initiate, Ephmeral Data and Confirm
Data packets exchange between Trust Center and joining device (IHD or Metering).
13. (Wireshark) After successfull Key Establishment, IHD and Metering devices discover Trust Center polling method (Keep-Alive cluster) and reads Keep-Alive base and jitter values.

>> First Service Discovery and Partner Link keys exchange
14. (Wireshark) After successfull Key Establishment and Trust Center polling method establishment, IHD starts Service Discovery procedure:
- bind DRLC and Price clusters from ESI
- bind Metering cluster from Metering device
Green Led blinks fast during Service Discovery.
15. (Wireshark) IHD establish partner link keys with Metering device:
- IHD sends Bind Request to Metering device, Metering device responds with success Bind Response
- IHD sends Request Key to Trust Center (partner - Metering device)
- Trust Center sends Transport Key with partner key for IHD and Metering device

>> Normal work
16. (Wireshark, Trace) IHD periodically sends Get Current Price command, ESI answers with Publish Price:
 4065	 6949/106.736640	se_ihd_zr.c:548		>> ihd_dev_cmd_get_current_price, param=10
 4068	 6949/106.736640	se_ihd_zr.c:565		<< ihd_dev_cmd_get_current_price
 4077	 6955/106.828800	se_ihd_zr.c:461		ZB_ZCL_PRICE_PUBLISH_PRICE_CB_ID
 4078	 6955/106.828800	se_ihd_zr.c:344		PublishPrice:
 4080	 6955/106.828800	se_ihd_zr.c:347		price=4
17. (Wireshark) IHD periodically repeats Service Discovery (once every ~3 hrs).
18. (Wireshark, Trace) ESI periodically sends DRLC events to IHD:
 4212	 10206/156.764160	se_ihd_zr.c:468		ZB_ZCL_DRLC_LOAD_CONTROL_EVENT_CB_ID
 4254	 10530/161.740800	se_ihd_zr.c:477		ZB_ZCL_DRLC_CANCEL_LOAD_CONTROL_EVENT_CB_ID
 4301	 10855/166.732800	se_ihd_zr.c:486		ZB_ZCL_DRLC_CANCEL_ALL_LOAD_CONTROL_EVENTS_CB_ID
 4303	 10855/166.732800	se_ihd_zr.c:442		cancel all events from binded drlc, schedule event_status
 4456	 13462/206.776320	se_ihd_zr.c:468		ZB_ZCL_DRLC_LOAD_CONTROL_EVENT_CB_ID
19. (Wireshark) IHD and Metering device periodically polls Trust Center (Keep-Alive check).