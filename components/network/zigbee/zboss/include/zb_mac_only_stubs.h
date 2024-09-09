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
/* PURPOSE: Stubs (empty functions) to build MAC-only stack
*/

#ifndef ZB_MAC_ONLY_STUBS_H
#define ZB_MAC_ONLY_STUBS_H 1

/*! \addtogroup ZB_MAC */
/*! @{ */

#ifdef ZB_MAC_ONLY_STACK


/**
   \par How to use mac-only stubs

   That stubs are created to be able to compile mac-only stack.
   The problem is that MAC calls some callbacks by name (that are .confirm and
   .indication).

   Test which uses only MAC layer, must have defines for functions which test
   re-declares and then include this header file.
 */

void zb_nwk_init()
{
}


void zb_aps_init()
{
}

void zdo_secur_init()
{
}

void zb_zdo_init()
{
}

void secur_nwk_generate_keys()
{
}

zb_ret_t zb_nwk_secure_frame(zb_bufid_t src, zb_uint_t mac_hdr_size, zb_bufid_t dst)
 {
  ZVUNUSED(src);
  ZVUNUSED(dst);
  ZVUNUSED(mac_hdr_size);
  return 0;
}

zb_ret_t zb_aps_secure_frame(zb_bufid_t src, zb_uint_t mac_hdr_size, zb_bufid_t dst, zb_bool_t is_tunnel)
{
  ZVUNUSED(src);
  ZVUNUSED(dst);
  ZVUNUSED(is_tunnel);
  ZVUNUSED(mac_hdr_size);
  return 0;
}

void zb_zdo_check_fails(zb_uint8_t param)
{
  if ( param )
  {
    zb_buf_free(param);
  }
}

#ifndef USE_ZB_MLME_RESET_CONFIRM
void zb_mlme_reset_confirm(zb_uint8_t param)
{
  zb_buf_free(param);
}
#endif


#ifndef USE_ZB_MLME_SET_CONFIRM
void zb_mlme_set_confirm(zb_uint8_t param)
{
  zb_buf_free(param);
}
#endif


#ifndef USE_ZB_MLME_START_CONFIRM
void zb_mlme_start_confirm(zb_uint8_t param)
{
  zb_buf_free(param);
}
#endif


#ifndef USE_ZB_MCPS_DATA_CONFIRM
void zb_mcps_data_confirm(zb_uint8_t param)
{
  zb_buf_free(param);
}
#endif

#ifndef USE_ZB_MLME_BEACON_NOTIFY_INDICATION
void zb_mlme_beacon_notify_indication(zb_uint8_t param)
{
  zb_buf_free(param);
}
#endif

#ifndef USE_ZB_MLME_POLL_CONFIRM
void zb_mlme_poll_confirm(zb_uint8_t param)
{
  zb_buf_free(param);
}
#endif

#ifndef USE_ZB_MLME_ORPHAN_INDICATION
void zb_mlme_orphan_indication(zb_uint8_t param)
{
  zb_buf_free(param);
}
#endif

#ifndef USE_ZB_MLME_ASSOCIATE_CONFIRM
void zb_mlme_associate_confirm(zb_uint8_t param)
{
  zb_buf_free(param);
}
#endif

#ifndef USE_ZB_MLME_ASSOCIATE_INDICATION
void zb_mlme_associate_indication(zb_uint8_t param)
{
  zb_buf_free(param);
}
#endif

#ifndef USE_ZB_MLME_SCAN_CONFIRM
void zb_mlme_scan_confirm(zb_uint8_t param)
{
  zb_buf_free(param);
}
#endif

#ifndef USE_ZB_MCPS_DATA_INDICATION
void zb_mcps_data_indication(zb_uint8_t param)
{
  zb_buf_free(param);
}
#endif

#ifndef USE_ZB_MCPS_POLL_INDICATION
void zb_mcps_poll_indication(zb_uint8_t param)
{
  zb_buf_free(param);
}
#endif


#ifndef USE_ZB_MLME_COMM_STATUS_INDICATION
void zb_mlme_comm_status_indication(zb_uint8_t param)
{
  zb_buf_free(param);
}
#endif


#ifndef USE_ZB_MLME_PURGE_CONFIRM
void zb_mlme_purge_confirm(zb_uint8_t param)
{
  zb_buf_free(param);
}
#endif

#ifndef USE_ZB_MLME_DUTY_CYCLE_MODE_INDICATION
void zb_mlme_duty_cycle_mode_indication(zb_uint8_t param)
{
  zb_buf_free(param);
}
#endif

void zb_aps_secur_init()
{
}

void zb_nwk_neighbor_init()
{
}

void zb_nwk_nib_init(zb_bool_t is_first)
{
  (void)is_first;
}

void zb_check_and_handle_oom(zb_time_t t)
{
  ZVUNUSED(t);
}

void zb_mlme_get_power_information_table_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_mlme_set_power_information_table_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_zse_init(void)
{
}

void zb_mcps_purge_indirect_queue_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}

#endif  /* ZB_MAC_ONLY_STACK */


#endif /* ZB_MAC_ONLY_STUBS_H */

/*! @}  */
