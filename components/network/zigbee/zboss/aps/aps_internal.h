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
/* PURPOSE: APS layer private header file
*/

#ifndef APS_INTERNAL_H
#define APS_INTERNAL_H 1

/*! \addtogroup ZB_APS */
/*! @{ */


/* See: 1.4.1.2. Endpoints 241-254 are reserved for use by the Device application or
   common application function agreed within the Zigbee Alliance*/
#define EP_RESERVED_START_ID       240U
#define EP_RESERVED_STOP_ID        254U
#define EP_DEVICE_EP               0U
#define EP_BROADCAST               255U

/**
   Handle packet duplicates: keep dups cache.

   @param src_addr - short address of the incoming packet
   @param aps_counter - aps_counter field from the packet
   @param is_unicast - is delivery mode of the incoming packet is unicast
   @return 1 if this is a dup, 0 otherwhise.
 */
zb_aps_dup_tbl_ent_t *aps_check_dups(zb_uint16_t src_addr, zb_uint8_t aps_counter, zb_bool_t is_unicast);

/**
   Update duplicate table and run aging

   @param ent - pointer to entry of duplicate table to update expire clock counter
   @return nothing
 */
void aps_update_entry_clock_and_start_aging(zb_aps_dup_tbl_ent_t *ent);

/**
   Clear all dups entries for that address.

   To be used after remote device leave or rejoin.

   @param src_addr - device address
 */
void aps_clear_dups(zb_uint16_t src_addr);

/**
   Clear entire APS dups table.

   To be used after local leave.
 */
void aps_clear_all_dups(void);

/**
   Parser APS command and invoker APS command handlers

   @param param - buffer index with APS command data
   @param keypair_i - index of APS key used to decrypt APS packet of -1 if used default TCLK
   @param key_id - the key used to protect the frame

   @return nothing
 */
void zb_aps_in_command_handle(zb_uint8_t param, zb_uint16_t keypair_i
                             , zb_secur_key_id_t key_id
                             );


#ifdef ZB_COORDINATOR_ROLE
/**
   Reaction on incoming UPDATE-DEVICE

   Issue UPDATE-DEVICE.indication

   @param param - buffer index with update device data
   @return nothing
 */
void zb_aps_in_update_device(zb_uint8_t param);
#endif /* ZB_COORDINATOR_ROLE */

/*! @} */

#endif /* APS_INTERNAL_H */
