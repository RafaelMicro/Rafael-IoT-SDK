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
/* PURPOSE:
*/

#ifndef ZB_OSIF_OTA_STUB_H
#define ZB_OSIF_OTA_STUB_H 1

void *zb_osif_ota_open_storage(void);
zb_bool_t zb_osif_ota_fw_size_ok(zb_uint32_t image_size);
zb_uint32_t zb_osif_ota_get_erase_portion(void);
void zb_osif_ota_erase_fw(void *dev, zb_uint_t offset, zb_uint32_t size);
void zb_osif_ota_write(void *dev, zb_uint8_t *data, zb_uint_t off, zb_uint_t size, zb_uint32_t image_size);
void zb_osif_ota_mark_fw_ready(void *dev, zb_uint32_t size, zb_uint32_t revision);
void zb_osif_ota_mark_fw_absent(void);
void zb_osif_ota_mark_fw_updated(void);
void zb_osif_ota_close_storage(void *dev);
zb_bool_t zb_osif_ota_verify_integrity(void *dev, zb_uint32_t raw_len);

/* WARNING: Works with absolute address! */
void zb_osif_ota_read(void *dev, zb_uint8_t *data, zb_uint32_t addr, zb_uint32_t size);

zb_bool_t zb_osif_ota_verify_integrity_async(void *dev, zb_uint32_t raw_len);
zb_uint8_t zb_erase_fw(zb_uint32_t address, zb_uint32_t pages_count);
zb_uint8_t zb_write_fw(zb_uint32_t address, zb_uint8_t *buf, zb_uint16_t len);
void Hash16_Calc(zb_uint32_t pBuffer, zb_uint32_t BufferLength, zb_uint8_t *hash16);

#endif /* ZB_OSIF_OTA_STUB_H */
