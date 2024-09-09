#/* ZBOSS Zigbee software protocol stack
# *
# * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
# * http://www.dsr-zboss.com
# * http://www.dsr-corporation.com
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
# PURPOSE: Host-side sample for Manufacture mode - receives every packets.
#

# Sample starts from setting NCP to a manufacture mode
# 1. Host sets and gets channel
# 2. Host sets and gets power
# 3. Host starts RX test mode in which all received packets will be sent to Host.
# 4. Host stops RX test mode after 10 packets.
# 5. Host stops a manufacture mode.

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

#
# Test itself
#

CHANNEL = 19
CHANNEL_MASK = 0x80000  # channel 19
PACKET = bytes([0x01, 0x05, 0x03, 0x05, 0x01, 0x07])
BEACON_REQ = bytes([0x03, 0x08, 0x06, 0xff, 0xff, 0xff, 0xff, 0x07])

#reset options
erase_nvram_on_start = not ncp_on_nsng()

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
            ncp_hl_call_code_e.NCP_HL_MANUF_START_TEST_RX : self.manuf_start_test_rx_rsp,
            ncp_hl_call_code_e.NCP_HL_MANUF_STOP_TEST_RX : self.manuf_stop_test_rx_rsp,
        })

        self.ind_switch = {
            ncp_hl_call_code_e.NCP_HL_MANUF_RX_PACKET_IND : self.manuf_rx_packet_ind,
        }

        self.required_packets = []

        self.reset_state = ncp_hl_rst_state_e.TURNED_ON

        self.state = 0
        self.pkt_cnt = 0

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
        self.host.ncp_req_manuf_set_power(-10)

    def manuf_set_power_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        sleep(1)
        self.host.ncp_req_manuf_get_power()

    def manuf_get_power_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_int8_t)
        sleep(1)
        self.host.ncp_req_manuf_start_test_rx()

    def manuf_start_test_rx_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

    def manuf_rx_packet_ind(self, ind, ind_len):
        logger.info("%d manuf_rx_packet_ind", self.pkt_cnt)
        rx_pkt = ncp_hl_rx_pkt_t.from_buffer(ind.body, 0)
        data4dump = list(rx_pkt.data)[:(rx_pkt.data_len-1)]
        logger.info("lqi {:#x} rssi {:#d} data {}".format(rx_pkt.lqi, rx_pkt.rssi, list(map(lambda x: hex(x), data4dump))))
        self.pkt_cnt += 1
        if self.pkt_cnt == 10:
            self.host.ncp_req_manuf_stop_test_rx()

    def manuf_stop_test_rx_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        sleep(1)
        self.host.ncp_req_manuf_mode_end()

    def manuf_mode_end_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

def main():
    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)
    fh = logging.FileHandler("zbs_manuf_rx_pkt.log")
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
