/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
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
/*  PURPOSE: NCP Low level protocol documentation
*/
#ifndef ZB_NCP_LL_DOC_H
#define ZB_NCP_LL_DOC_H 1


/**
 *  @mainpage ZBOSS API License
 *
 * @par ZBOSS Zigbee 3.0 stack
 * Zigbee 3.0 stack, also known as ZBOSS v3.0 (R) Zigbee stack is available
 * under the terms of the DSR Commercial License.
 *
 * ZBOSS is a registered trademark of DSR Corporation AKA Data Storage
 * Research LLC.
 *
 * @par Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and DSR.
 */

/**
   @defgroup NCP_HOST_LIB NCP Low level protocol library basics
   @defgroup NCP_HOST_MEM NCP Low level protocol library memory requirements
   @defgroup NCP_HOST_API NCP Host-side Low level protocol library API reference
   @defgroup NCP_HOST_TYPES ZBOSS internal typedefs & macro API reference
   @defgroup NCP_TRANSPORT_API Host transport API for Low Level protocol library
 */

/**
@addtogroup NCP_HOST_LIB
@{

@par NCP Host library

In NCP SDK DSR provides a C language library which implements a logic of Low Level of ZBOSS NCP protocol.
NCP LL library is provides in source form.
The library consists of 2 layers:
@li logic, which is same for all platforms
@li i/o and platform support which is platform-specific

Application at Host communicates with logic layer API only.

@section ll_logic_layer Logic layer and its API for the application.

@par LL logic layer functionality

LL protocol Logic layer implements following functionality: framing, acknowlagdes, retransmission, fragmentation/defragmentation of long messages.
That layer is transport-agnostic: its code can be used with any OS/platform and physical transport (SPI, UART, USB etc).

@par LL Logic layer {lack of} threads of execution

Logic layer of LL library is designed to have no any internal thread of execution: no thread/task inside LL library,
no block at i/o. All execution flow, blocking, time must be done in the application.

Host application inside its execution flow must periodically call LL library to give it a time quant, transfer and receive data.
NCP LL library quant function is called ncp_host_ll_quant().

Note that call to ncp_host_ll_quant() not always results to complete receive or transmit.
Sometimes that call is necessary for internal LL library processing, such as receiving part of a long fragmented message.
ncp_host_ll_quant() at return assigns value of its @ref ncp_host_ll_quant alarm_timeout_ms parameter, so application knows about maximum delay before ncp_host_ll_quant() must be called again: must call it "now" + alarm_timeout_ms.
It is ok to call ncp_host_ll_quant() earlier than that time.

ncp_host_ll_quant() is not reentrant not interrupt- or thread-safe.
It is ok to call it from different threads, but only one ncp_host_ll_quant() must be executed at a time. That is Host application task to do all necessary synchronization.
ncp_host_ll_quant() must not be called from the interrupt context or from Unix signal handler.

ncp_host_ll_quant() also must not be called from @ref ncp_ll_call_me_cb_t call_me_cb functions (see below).


@par call_me_cb

I/o layer of LL library can utilize ISR (at MCU), or some i/o event waiting thread (at Linux).
For example, when working via SPI i/o layer, Slave informs Master (which is Host) about the fact that it wants to transmit by asserting HOST_INT line.
At Host side i/o layer (ISR for GPIO line change at MCU, select() call for Linux) detects the fact of HOST_INT line assert and initiates data recv.
Send/recv in i/o layer at MCU is to be done in interrupt handlers as well.
But, since LL library logic can't be called from the interrupt context, and LL library logic hasn't its own execution thread, LL library just informs the Host application about an event.
It uses @ref ncp_ll_call_me_cb_t call_me_cb function call to inform Host application that something happened, so ncp_host_ll_quant() must be called immediately.

Note that @ref ncp_ll_call_me_cb_t call_me_cb function can be called from different contexts: either from ISR, or i/o thread, or ncp_host_ll_quant().
Host application must not do any significant processing there. There are no any arguments or other additional information @ref ncp_ll_call_me_cb_t call_me_cb can provide.
Its call is just a signal for the Host application: call ncp_host_ll_quant() immediately from the safe execution context (usually some kind of main loop or some thread/task in the Host application).

@par Using ncp_host_ll_quant() for rx/tx

LL protocol library interface is quite simple: it consists of 2 calls and 1 callback.

At start time application must initialize NCP LL library by calling ncp_host_ll_proto_init() providing @ref ncp_ll_call_me_cb_t call_me_cb function.

When Host application wants to send some data, it calls ncp_host_ll_quant() providing data in tx_buf and its size in tx_size.
Host application must check for return status of ncp_host_ll_quant(). If NCL LL library has no resources for transmit, it returns ZBNCP_RET_BUSY.
ncp_host_ll_quant() returns ZBNCP_RET_OK if previous transmit completed or there was no previous transmit.
If previous transmit is still pending, ncp_host_ll_quant() returns ZBNCP_RET_BLOCKED.
ZBNCP_RET_TX_FAILED returned if transport layer failed to transmit a packet.
ZBNCP_RET_NO_ACK returned if opposite side hasn't acknowledged transmitted packet.

Possible returning values for RX case:
ZBNCP_RET_OK means that NCP LL library is ready for the next transmission
ZBNCP_RET_BLOCKED means that NCP LL library is busy with transmission of current packet

Possible returning values for TX case:
ZBNCP_RET_OK means that the packet passed via (tx_buf, tx_size) parameters was taken by NCP LL library, the transmission was started, and NCP LL library is ready for the next transmission. For e.g., the packet is less than one fragment and can be sent in one SPI transaction.
ZBNCP_RET_BLOCKED means that NCP LL library started to process and transmit the packet passed via (tx_buf, tx_size) parameters, but NCP LL library isn't ready for transmission of new packets. For e.g. the packet has a size of two fragments and now NCP LL library is busy with their transmission.
ZBNCP_RET_BUSY means that NCP LL library is busy with the transmission of the current packet and the packet passed via (tx_buf, tx_size) parameters cannot be sent at that moment.
ZBNCP_RET_ERROR means packet passed via (tx_buf, tx_size) parameters is bigger than internal NCP LL library buffer.

When Host application is ready to receive some data (normally it must always be ready), Host passes buffer for rx in @ref ncp_host_ll_quant rx_buf, its size in @ref ncp_host_ll_quant rx_buf_size.
If ncp_host_ll_quant() completed receive operation, it sets @ref ncp_host_ll_quant received_bytes parameter. If @ref ncp_host_ll_quant received_bytes is not zero, Host app must handle received packet.

@par tx/rx and fragmentation

NCP LL library implements fragmentation/defragmentation inside.

If application wants to send large data which is to be fragmented by NCL LL library, application provides that data in the single buffer.

NCP LL library handles received data fragmentation itself, and delivers to the Host app full received buffer.

NCP LL library can buffer only one big message.

@section ll_io_layer i/o layer: how to implement it

NCL LL library included into this ZBOSS NCP SDK has i/o layer for SPI physical transport at Raspberry PI.

To implement another physical transport (UART, USB etc) or to port SPI transport to another platform (OS, HW), it is necessary to create new i/o layer.
The procedure is described here.

\image html LL_init_sequence.png "Initialization"
\image latex LL_init_sequence.png "Initialization" width=12cm

@par Implement ncp_host_transport_create()

i/o layer must implement ncp_host_transport_create() which fills @ref zbncp_transport_ops_t data structure by pointers to i/o functions implemented in i/o layer.

@snippet zb_ncp_tr_spidev.c ncp_host_transport_create

@par Implement 3 transport operations

Implement 3 functions for 3 transport operations: init, receive, send.
That operations starts appropriate asynchronous operations of init, receive, send.

@snippet zb_ncp_tr_spidev.c ncp_tr_spidev_init

@snippet zb_ncp_tr_spidev.c ncp_tr_spidev_recv

@snippet zb_ncp_tr_spidev.c ncp_tr_spidev_send

Init function has zbncp_transport_cb_t *cb parameter which i/o layer must remember and call its callbacks to inform NCP LL logic layer about i/o state change.
It must be called in following cases:

@li call cb.init when HW init is done. @snippet zb_ncp_tr_spidev.c linux_spi_init_complete
@li call cb.send when data send is complete. @snippet zb_ncp_tr_spidev.c linux_spi_send_complete
@li call cb.recv when some data received by i/o layer. @snippet zb_ncp_tr_spidev.c linux_spi_recv_complete

@par Call pairs

After NCP LL logic registered 3 i/o layer callbacks (init, rx, tx) by calling ncp_host_transport_create(),
NCP LL logic calls i/o layer by calling the callback. That call is normally asynchronous, so i/o layer needs a way to do return of that call.
It does so by calling the appropriate callback from 'cb' data structure passed by LL logic to i/o during init.

There are 3 call pairs:

@li NCP LL logic initiates i/o layer init init by @ref zbncp_transport_ops_t init function ptr.
When i/o layer init is complete, i/o layer notifies LL logic by calling cb.init.

@li NCP LL logic initiates i/o layer Send  by @ref zbncp_transport_ops_t send function ptr.
That call is normally asynchronous: it may put data to some internal buffer, queue etc.
When data is actually sent, the i/o layer notifies LL logic by calling cb.send.

@li When NCP LL logic is ready to receive new packet, it informs i/o layer by @ref zbncp_transport_ops_t recv function ptr.
That call is normally asynchronous: it remembers buffer ptr and some somewhere in i/o layer.
When data is actually received, the i/o layer notifies LL logic by calling cb.recv.


@li When NCL LL is ready to receive next frame, it gives to Host app a memory buffer by calling its recv callback
@li When rx completed, Host app calls cb.recv
@li Do not block for rx between that calls because you do not know when NCP will send next packet
@Li when NCL LL wants to send a frame, it calls i/o layer's send callback
@li When tx completed, Host i/o calls cb.send
@li Host app can block in send between that calls. It looks safe (while ineffective) but was not tested in out Linux implementation.


@par HOST_INT GPIO line handler (if any)

HOST_INT line is required not for all physical transports.
For instant, it required for SPI but not required for UART.
In case of SPI it used by Slave to inform Master that Slave has something to send.

At MCU the handler of HOST_INT GPIO line might be ISR. In Linux it is a thread waiting in select() call.

Using of HOST_INT interrupt is specific for i/o layer.
It is up to i/o layer to decide what to do when Host got that interrupt.
Usually SPI Recv will be initiated if Host is not already busy sending via SPI.
There is no need to immediately inform LL logic lager aout a HOST_INT. LL logic knows nothing about a host interrupt.
Instead, i/o layer receives a packet then calls @ref zbncp_transport_cb_t cb.recv.

i/o can be implemented using interrupts, or polling mode, or Linux threads. NCP LL librart does not care about i/o implementation.

@par Send logic for SPI transport.

Send is straightforward: the whole packet, including signature, header, body crc,
body is send in the single SPI transfer from Host (Master) to NCP (Slave).

i/o layer at host does not try to handle any races between HOST_INT line assert and TX start.

\image html LL_tx_sequence.png "Transmission"
\image latex LL_tx_sequence.png "Transmission" width=12cm

@par Recv logic for SPI transport.

Recv in case of SPI is bit more complex that Send because Host needs to know packet length.
The logic is following:

\image html LL_rx_sequence.png "Reception"
\image latex LL_rx_sequence.png "Reception" width=12cm

@li Host receives packet signature and header - all of fixed size.
@li Host verifies that signature is ok, header is ok (header CRC matches its contents). If header is bad, skip that packet.
@li If, according to the packet length field, no body exists, we are done - pass packet up into NCP LL logic layer by calling cb.recv
@li if, according to the packet length field, there is body, start more SPI clocks to receive rest of the packet.
@li After entire packet received, pass packet up into NCP LL logic layer by calling cb.recv. No need to verify data CRC - LL logic layer does it itself.

@par ACK receiving procedure

LL logic is implemented as two state machines - for RX and TX procedures. Having sent a packet, TX state maching starts waiting for ACK, during that time RX state machine processes received data.
To handle the packet RX goes through several states, like reading packet header from transport buffer, checking the header, requesting body and so on. Each similar step takes one run of ncp_ll_quant().
After receiving an ACK, RX machine checks for TX machine to be in state waiting for ACK, if it's so, RX machine closes the pending packet. Since changing from sending packet to waiting an ACK state
requires only one quant and parsing the ACK in RX state maching requires several quants, there is no problem to call ncp_ll_quant() after receiving the ACK on transport layer.


@}
*/

/**
@addtogroup NCP_HOST_MEM
@{

@section ll_mem_usage NCP LL library memory requirements

@par RAM

RAM consumption can be tuned by changing following constants:
@li @ref ZBNCP_BIG_BUF_SIZE - maximum size of fragmented packet. Now 1600b
@li @ref ZBNCP_LL_PKT_SIZE_MAX - maximum size of fragment to be transmitted by transport (SPI etc). Now 256b

RAM used by NCP LL library (without i/o layer):
@li 2 buffers (rx and tx) per 1 fragment - 2 * 256 = 512
@li big packet buffer for rx packet reassembly = 1600b
@li big packet buffer for tx packet divide = 2 * 1600b = 3200b
@li extra 732 bytes of internal data structuers

Total RAM usage: 6044b.

@par Flash

Code size depends on architecture and compiler.

Code used by NCP LL library (without i/o level) at TI CC1352 Cortex MCU compiled by ARM ccs compiler is 3342 bytes.

@}
*/

#endif  /* ZB_NCP_LL_DOC_H */
