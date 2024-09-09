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
# PURPOSE: Sample with visible requests in ZED role.
#

# ZED tries to join to ZC changing MAC visibility on each failed attempt.

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
from time import sleep

logger = logging.getLogger(__name__)
log_file_name = "zbs_visible_zed.log"

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19

delay = 1


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZED)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE: self.set_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_RX_ON_WHEN_IDLE: self.get_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY: self.nwk_discovery_complete,
            ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN:self.nwk_join_complete,
            ncp_hl_call_code_e.NCP_HL_ADD_VISIBLE_DEV: self.add_visible_dev_rsp,
            ncp_hl_call_code_e.NCP_HL_ADD_INVISIBLE_SHORT: self.add_invisible_short_rsp,
            ncp_hl_call_code_e.NCP_HL_RM_INVISIBLE_SHORT: self.rm_invisible_short_rsp,
        })

        self.update_indication_switch({
        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL_MASK,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_ROLE,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_RX_ON_WHEN_IDLE,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ADD_VISIBLE_DEV,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ADD_INVISIBLE_SHORT,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,              NCP_HL_STATUS.MAC_NO_BEACON],
                                 [ncp_hl_call_code_e.NCP_HL_RM_INVISIBLE_SHORT,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,              NCP_HL_STATUS.MAC_NO_DATA],
                                 [ncp_hl_call_code_e.NCP_HL_ADD_VISIBLE_DEV,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,              NCP_HL_STATUS.RET_OK],
                                 ]

        self.zc_ieee = ncp_hl_ieee_addr_t()
        self.step = 0

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

    def begin(self):
        self.host.ncp_req_set_rx_on_when_idle(ncp_hl_on_off_e.OFF)

    def set_rx_on_when_idle_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_get_rx_on_when_idle()

    def get_rx_on_when_idle_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            val = ncp_hl_uint8_t.from_buffer_copy(rsp.body, 0).uint8
            strval = "OFF" if val == ncp_hl_on_off_e.OFF else "ON"
            logger.info("rx_on_when_idle: %s", strval)
        self.host.ncp_req_add_visible_dev(self.ieee_addr)

    def add_visible_dev_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if self.step == 0:
            self.step = 1
            self.host.ncp_req_add_invisible_short(0)
        else:
            sleep(delay)
            self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5) 

    def add_invisible_short_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        sleep(delay)
        self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)

    def rm_invisible_short_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        sleep(delay)
        self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)

    def nwk_discovery_complete(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_nwk_discovery_rsp_hdr_t)
        dh = ncp_hl_nwk_discovery_rsp_hdr_t.from_buffer_copy(rsp.body)
        for i in range(0, dh.network_count):
            dsc = ncp_hl_nwk_discovery_dsc_t.from_buffer_copy(rsp.body, sizeof(dh) + i * sizeof(ncp_hl_nwk_discovery_dsc_t))
            self.rsp_log(rsp, ncp_hl_nwk_discovery_dsc_t)
        # In our test we have only 1 network, so join to the first one
        dsc = ncp_hl_nwk_discovery_dsc_t.from_buffer_copy(rsp.body, sizeof(dh))
        # Join thru association to the pan and channel just found
        if rsp.hdr.status_code == 0xEA: # MAC_NO_BEACON
            self.host.ncp_req_rm_invisible_short(0)
        else:
            self.zc_ieee = dsc.extpanid
            self.host.ncp_req_nwk_join_req(dsc.extpanid, 0, dsc.page, (1 << dsc.channel), 5,
                                       ncl_hl_mac_capability_e.NCP_HL_CAP_ALLOCATE_ADDRESS,
                                       0)
   
    def nwk_join_complete(self, rsp, rsp_len):
        self.rsp_log(rsp)
        body = ncp_hl_nwk_nlme_join_rsp_t.from_buffer_copy(rsp.body)
        if rsp.hdr.status_code == 0:
            self.rsp_log(rsp, ncp_hl_nwk_nlme_join_rsp_t)
        elif rsp.hdr.status_code == 0xEB: # MAC_NO_DATA
            self.host.ncp_req_add_visible_dev(self.zc_ieee)

    def on_data_ind(self, dataind):
        self.send_packet_back(dataind, False)


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
