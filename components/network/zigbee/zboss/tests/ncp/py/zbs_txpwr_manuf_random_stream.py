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
# PURPOSE: Host-side sample for Manufacture mode - random stream.
#

# Sample starts from setting NCP to a manufacture mode
# 1. Host sets and gets channel
# 2. Host sets and gets power
# 3. Host starts to transmit a random stream for 5 sec.
# 4. Host stops a random stream.
# 5. Points 2..5 are repeated 6 times with different Tx power levels
# 6. Host stops a manufacture mode.
#
# Different power levels can be seen in SmartRF studio -> Continuous RX -> Channel 19 -> Pollin Interval 1s
# The graph looks something like this:                  
#              
#                                                                          /̅̅̅̅̅\
#                                                        /̅̅̅̅̅\          /       \
#                                        /̅̅̅̅̅\        /       \        /         \
#                          /̅̅̅̅̅\      /       \      /         \      /           \
#              /̅̅̅̅̅\    /       \    /         \    /           \    /             \
# ___/̅̅̅̅̅\__/       \__/         \__/           \__/             \__/               \_____


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
log_file_name = "zbs_txpwr_manuf_random_stream.log"

#
# Test itself
#

CHANNEL = 19
CHANNEL_MASK = 0x80000  # channel 19
PACKET = bytes([0x01, 0x05, 0x03, 0x05, 0x01, 0x07])
BEACON_REQ = bytes([0x03, 0x08, 0x06, 0xff, 0xff, 0xff, 0xff, 0x07])

STREAM_DURATION = 5

#reset options
erase_nvram_on_start = not TestRunner().ncp_on_nsng()

class Test(BaseTest):
    def __init__(self, channel_mask):     
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION : self.get_module_version_rsp,
            ncp_hl_call_code_e.NCP_HL_NCP_RESET : self.ncp_reset_resp,
            ncp_hl_call_code_e.NCP_HL_MANUF_MODE_START : self.manuf_mode_start_rsp,
            ncp_hl_call_code_e.NCP_HL_MANUF_MODE_END : self.manuf_mode_end_rsp,
            ncp_hl_call_code_e.NCP_HL_MANUF_SET_CHANNEL : self.manuf_set_channel_rsp,
            ncp_hl_call_code_e.NCP_HL_MANUF_GET_CHANNEL : self.manuf_get_channel_rsp,
            ncp_hl_call_code_e.NCP_HL_MANUF_SET_POWER : self.manuf_set_power_rsp,
            ncp_hl_call_code_e.NCP_HL_MANUF_GET_POWER : self.manuf_get_power_rsp,
            ncp_hl_call_code_e.NCP_HL_MANUF_START_STREAM_RANDOM : self.manuf_start_stream_rsp,
            ncp_hl_call_code_e.NCP_HL_MANUF_STOP_STREAM_RANDOM : self.manuf_stop_stream_rsp,
        })

        self.update_indication_switch({
        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,                   NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,                       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_MODE_START,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_SET_CHANNEL,               NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_GET_CHANNEL,               NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_SET_POWER,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_GET_POWER,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_START_STREAM_RANDOM,       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_STOP_STREAM_RANDOM,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_SET_POWER,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_GET_POWER,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_START_STREAM_RANDOM,       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_STOP_STREAM_RANDOM,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_SET_POWER,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_GET_POWER,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_START_STREAM_RANDOM,       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_STOP_STREAM_RANDOM,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_SET_POWER,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_GET_POWER,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_START_STREAM_RANDOM,       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_STOP_STREAM_RANDOM,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_SET_POWER,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_GET_POWER,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_START_STREAM_RANDOM,       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_STOP_STREAM_RANDOM,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_SET_POWER,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_GET_POWER,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_START_STREAM_RANDOM,       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_MANUF_STOP_STREAM_RANDOM,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET                        NCP_HL_STATUS.RET_OK],
                                ]

        self.nwk_key_disabled = True

        self.reset_state = ncp_hl_rst_state_e.TURNED_ON

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)
        self.desired_tx_power = -30

    def get_module_version_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_module_ver_t)
        self.host.ncp_req_manuf_mode_start()

    def ncp_reset_resp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if erase_nvram_on_start == True:
            if self.reset_state == ncp_hl_rst_state_e.TURNED_ON:
                self.reset_state = ncp_hl_rst_state_e.NVRAM_ERASE
                self.host.ncp_req_reset(ncp_hl_reset_opt_e.NVRAM_ERASE)
            elif self.reset_state == ncp_hl_rst_state_e.NVRAM_ERASE:
                self.reset_state = ncp_hl_rst_state_e.NVRAM_HAS_ERASED
                logger.info("NVRAM has been erased: status %d", rsp.hdr.status_code)
                self.host.ncp_req_get_module_version()
        else:
            self.host.ncp_req_get_module_version()

    def manuf_mode_start_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        sleep(1)
        self.host.ncp_req_manuf_set_channel(CHANNEL)

    def manuf_set_channel_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        sleep(1)
        self.host.ncp_req_manuf_get_channel()

    def manuf_get_channel_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_uint8_t)
        sleep(1)
        self.set_tx_power()

    def manuf_get_power_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_int8_t)
        if rsp.hdr.status_code == 0:
            body = ncp_hl_int8_t.from_buffer_copy(rsp.body)
            logger.info("get TX power returns {} dBm".format(body.int8))
        sleep(1)
        self.host.ncp_req_manuf_start_stream(0x5555)

    def manuf_start_stream_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        sleep(STREAM_DURATION)
        self.host.ncp_req_manuf_stop_stream()

    def manuf_stop_stream_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        sleep(1)
        self.set_tx_power()

    def manuf_mode_end_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

    def set_tx_power(self):
        if self.desired_tx_power < -20:
            self.desired_tx_power = -20
        elif self.desired_tx_power == -20:
            self.desired_tx_power = -10
        elif self.desired_tx_power == -10:
            self.desired_tx_power = 0
        elif self.desired_tx_power == 0:
            self.desired_tx_power = 5
        elif self.desired_tx_power == 5:
            self.desired_tx_power = 14
        elif self.desired_tx_power == 14:
            self.desired_tx_power = 20
        else:
            self.desired_tx_power = 1
        if self.desired_tx_power == 1:
            self.host.ncp_req_manuf_mode_end()
        else:
            logger.info("* set Tx power to %d dBm", self.desired_tx_power)
            self.host.ncp_req_manuf_set_power(self.desired_tx_power)

    def manuf_set_power_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        sleep(1)
        self.host.ncp_req_manuf_get_power()


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
