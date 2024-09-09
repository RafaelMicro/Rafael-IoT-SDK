#!/usr/bin/env python3.7

#/* ZBOSS Zigbee software protocol stack
# *
# * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
# * www.dsr-zboss.com
# * www.dsr-corporation.com
# * All rights reserved.
# *
# * This is unpublished proprietary source code of DSR Corporation
# * The copyright notice does not evidence any actual or intended
# * publication of such source code.
# *
# * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
# * Corporation
# *
# * Commercial Usage
# * Licensees holding valid DSR Commercial licenses may use
# * this file in accordance with the DSR Commercial License
# * Agreement provided with the Software or, alternatively, in accordance
# * with the terms contained in a written agreement between you and
# * DSR.
#
# PURPOSE: Host-side sample for Locking reading NWK and APS keys from NCP.
#
# ATTENTION! Locking reading the keys makes it impossible to debug and
# rewrite the part of flash. Only full erasing of the flash is possible.
#

import sys
import threading
import platform
from enum import IntEnum
from ctypes import *
import logging
from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
from time import sleep

logger = logging.getLogger(__name__)
log_file_name = "zbs_block_reading_keys_zed.log"

#
# Test itself
#
CHANNEL_MASK = 0x80000  # channel 19

class Test(BaseTest):
    def __init__(self, channel_mask):     
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZED)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_GET_LOCK_STATUS : self.get_lock_status_rsp,
            ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE: self.set_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_RX_ON_WHEN_IDLE: self.get_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_NWK_KEYS: self.get_nwk_keys_rsp,
            ncp_hl_call_code_e.NCP_HL_NWK_GET_IEEE_BY_SHORT: self.nwk_get_ieee_by_short_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_APS_KEY_BY_IEEE: self.get_aps_key_by_ieee_rsp,
            ncp_hl_call_code_e.NCP_HL_SECUR_GET_KEY_IDX: self.ncp_req_secur_get_key_idx_rsp,
            ncp_hl_call_code_e.NCP_HL_SECUR_GET_KEY: self.ncp_req_secur_get_key_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_REJOIN: self.ncp_rejoin_rsp,
        })

        self.required_packets = [# call ids before locking reading keys
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,                      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL_MASK,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_PAN_ID,                     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_ROLE,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_START_POLL,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_RX_ON_WHEN_IDLE,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCK_STATUS,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_NWK_KEYS,                   NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_GET_IEEE_BY_SHORT,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_APS_KEY_BY_IEEE,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_GET_KEY_IDX,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_GET_KEY,                  NCP_HL_STATUS.RET_OK],
                                 # call ids after locking reading keys
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,                      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL_MASK,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_PAN_ID,                     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_ROLE,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_START_POLL,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_RX_ON_WHEN_IDLE,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_REJOIN,                     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCK_STATUS,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_NWK_KEYS,                   NCP_HL_STATUS.RET_OPERATION_FAILED],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_GET_IEEE_BY_SHORT,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_APS_KEY_BY_IEEE,            NCP_HL_STATUS.RET_OPERATION_FAILED],
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_GET_KEY_IDX,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_GET_KEY,                  NCP_HL_STATUS.RET_OPERATION_FAILED],
                                ]

        self.update_indication_switch({
        })

        self.get_lock_satus_n = 0

        self.nwk_key_disabled = True  #disable asking nwk_key after test finished
        self.reset_state = ncp_hl_rst_state_e.TURNED_ON

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

    def begin(self):
        self.host.ncp_req_set_rx_on_when_idle(ncp_hl_on_off_e.OFF)

    def set_rx_on_when_idle_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_get_rx_on_when_idle()

    def get_rx_on_when_idle_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_uint8_t)
        if self.get_lock_satus_n == 0:
            self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)
        else:
            self.host.ncp_req_zdo_rejoin(self.extpanid, self.mask, 0)

    def ncp_rejoin_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_get_lock_status()
    
    def on_nwk_join_complete(self, body):
        body_j = ncp_hl_nwk_nlme_join_rsp_t.from_buffer_copy(body)
        self.extpanid = ncp_hl_ext_panid_t(body_j.extpanid.b8)
        self.mask = (1 << body_j.channel)
        self.host.ncp_get_lock_status()

    def get_lock_status_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            val = ncp_hl_uint8_t.from_buffer_copy(rsp.body).uint8
            strval = "FALSE" if val == 0 else "TRUE"
            logger.info("* lock status: %s", strval)
        # 1st call, protected by 'locking reading keys'
        self.host.ncp_req_get_nwk_keys()

    def get_nwk_keys_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_nwk_keys_t)
        # Get long address of ZC to ask TCLK
        self.host.ncp_req_nwk_get_ieee_by_short(0)

    def nwk_get_ieee_by_short_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_ieee_addr_t)
        if rsp.hdr.status_code == 0:
            ieee_addr = ncp_hl_ieee_addr_t.from_buffer_copy(rsp.body, 0)
            self.zc_ieee = ieee_addr
            # 2nd call, protected by 'locking reading keys'
            self.host.ncp_req_get_aps_key_by_ieee(ieee_addr.b8)

    def get_aps_key_by_ieee_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_aps_key_t)
        logger.info("ZC ieee {}".format(self.zc_ieee))
        self.host.ncp_req_secur_get_key_idx(self.zc_ieee)

    def ncp_req_secur_get_key_idx_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_uint16_t)
        if rsp.hdr.status_code == 0:
            key_idx = ncp_hl_uint16_t.from_buffer_copy(rsp.body)
            # 3rd call, protected by 'locking reading keys'
            self.host.ncp_req_secur_get_key(key_idx.uint16)

    def ncp_req_secur_get_key_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_lk_t)
        if self.get_lock_satus_n == 0:
            self.get_lock_satus_n += 1
            # don't erase flash on reboot
            self.erase_nvram_on_start = False
            # self.host.ncp_req_reset(ncp_hl_reset_opt_e.NO_OPTIONS)
            self.host.ncp_req_reset(ncp_hl_reset_opt_e.BLOCK_READING_KEYS)


def main():
    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)
    fh = logging.FileHandler(log_file_name)
    fh.setLevel(loggerLevel)
    fh.setFormatter(loggerFormatter)
    logger.addHandler(fh)

    try:
        test = Test(CHANNEL_MASK)
        logger.info("Running test")
        so_name = TestRunner().get_ll_so_name()
        test.init_host(so_name)
        test.run()
    except KeyboardInterrupt:
        return
    finally:
        logger.removeHandler(fh)


if __name__ == "__main__":
    main()
