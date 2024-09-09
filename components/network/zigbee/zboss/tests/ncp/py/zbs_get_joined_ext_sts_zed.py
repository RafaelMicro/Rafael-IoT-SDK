#!/usr/bin/env python3.7

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
# PURPOSE: Host-side test implements ZED role, works out the functions of receiving
# PIM_PARENT_NO_ACK_IND indication and inspect the extended status in GET_JOINED response
#
# It join to test network, then you need to restart the parent after appearing in the log
# "shutdown parent and start it again, to continue test" message to cause the situation
# `joined no parent` and wait for successful rejoin
#
# Tested with standalone esi_device_ncp as ZC
#
# run sequence:
#
# nsng
# esi_device_ncp
# python3 zbs_get_joined_ext_sts_zed.py
# ncp_fw
# after message "shutdown parent and start it again, to continue test"
# kill esi_device_ncp (Ctrl-C or killall esi_device_ncp)
# and immediately start esi_device_ncp again
#

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
from time import sleep

logger = logging.getLogger(__name__)
log_file_name = "zbs_get_joined_ext_sts_zed.log"

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19

LONG_POLL_INTERVAL = 24 # 24 = 6seconds * 4 quarterseconds
FAST_POLL_INTERVAL = 4  # 4 = 1second * 4 quarterseconds
LONG_POLL_PKT_N    = 5  # number of packets for receiving in long polling to change state
FAST_POLL_PKT_N    = 5  # number of packets for receiving in fast polling to change state
STOP_POLL_TIME     = 10 # on that time all polling will be stopped


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZED)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE: self.set_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_RX_ON_WHEN_IDLE: self.get_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY: self.nwk_discovery_complete,
            ncp_hl_call_code_e.NCP_HL_GET_JOINED: self.get_joined_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_REJOIN: self.zdo_rejoin_rsp,
            ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN: self.nwk_join_complete_local,
        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_JOINED,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_JOINED,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PARENT_LOST_IND,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_JOINED,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_REJOIN,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_RX_ON_WHEN_IDLE,        NCP_HL_STATUS.RET_OK],
                                 ]

        self.update_indication_switch({
            ncp_hl_call_code_e.NCP_HL_PARENT_LOST_IND: self.pim_parent_no_ack_ind,
            # DL: it's indications not come up, does anyone know why?
            ncp_hl_call_code_e.NCP_HL_NWK_JOINED_IND: self.nwk_rejoin_complete,
            ncp_hl_call_code_e.NCP_HL_NWK_JOIN_FAILED_IND: self.nwk_rejoin_failed
        })

        # get_joined_req_cycl_ctrl variable to control the looped GET_JOINED request
        #   0 - loop stopped
        #   1 - continue fast, selfmade new GET_JOINED request
        #   2 - continue slow, selfmade new GET_JOINED request
        #   3 - take one step and then stop by setting get_joined_req_cycl_ctrl to 0 yourself
        # to start the loop, assign the value to get_joined_req_cycl_ctrl and call host.ncp_req_get_joined()
        self.get_joined_req_cycl_ctrl = 0

        self.join_complete_cnt = 0
        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

    def begin(self):
        self.host.ncp_req_set_rx_on_when_idle(ncp_hl_on_off_e.OFF)

    def set_rx_on_when_idle_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)
        
    def get_rx_on_when_idle_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

    def nwk_discovery_complete(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_nwk_discovery_rsp_hdr_t)
        dh = ncp_hl_nwk_discovery_rsp_hdr_t.from_buffer_copy(rsp.body)
        for i in range(0, dh.network_count):
            dsc = ncp_hl_nwk_discovery_dsc_t.from_buffer_copy(rsp.body, sizeof(dh) + i * sizeof(ncp_hl_nwk_discovery_dsc_t))
            self.rsp_log(rsp, ncp_hl_nwk_discovery_dsc_t)
        if dh.network_count > 0:
            if self.join_complete_cnt > 0: # joined previously
                self.host.ncp_req_zdo_rejoin(self.extpanid, self.mask, 0)
            else:
                # In our test we have only 1 network, so join to the first one
                dsc = ncp_hl_nwk_discovery_dsc_t.from_buffer_copy(rsp.body, sizeof(dh))
                self.extpanid = ncp_hl_ext_panid_t(dsc.extpanid.b8)
                self.mask = (1 << dsc.channel)
                # Join thru association to the pan and channel just found
                self.host.ncp_req_nwk_join_req(dsc.extpanid, 0, dsc.page, (1 << dsc.channel), 5, 
                                               ncl_hl_mac_capability_e.NCP_HL_CAP_ALLOCATE_ADDRESS, 0)
            # start ncp_req_get_joined() in loop while on_nwk_join_complete() is not arrived
            self.get_joined_req_cycl_ctrl = 1
            self.host.ncp_req_get_joined()
        else:
            self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)

    def nwk_join_complete_local(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            self.rsp_log(rsp, ncp_hl_nwk_nlme_join_rsp_t)
            logger.info("shutdown parent and start it again, to continue test")
            self.get_joined_req_cycl_ctrl = 3 # to stop ncp_req_get_joined() loop on next step
            self.join_complete_cnt = self.join_complete_cnt + 1
        else:
            # DL: sometimes NCP_HL_NWK_NLME_JOIN returns MAC_NO_DATA, does anyone know why?
            self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)
   
    def pim_parent_no_ack_ind(self, ind, ind_len):
        # here if parent is lost
        self.ind_log(ind, ncp_hl_uint8_t)
        # start ncp_req_get_joined in loop while nwk_join_complete_local() with status 0 is not arrived
        self.get_joined_req_cycl_ctrl = 1
        self.host.ncp_req_get_joined()
        # rejoin after successful nwk discovery
        self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)

    def zdo_rejoin_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.get_joined_req_cycl_ctrl = 3
        if rsp.hdr.status_code == 0:
            self.host.ncp_req_get_rx_on_when_idle() # to reveive the last required_packets and stop the test
        else:
            self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)

    def nwk_rejoin_complete(self, ind, ind_len):
        self.ind_log(ind)
        self.get_joined_req_cycl_ctrl = 3
        logger.info("nwk_rejoin_complete_ind.")

    def nwk_rejoin_failed(self, ind, ind_len):
        self.ind_log(ind)
        self.get_joined_req_cycl_ctrl = 3
        status = ncp_hl_status_t.from_buffer_copy(ind.body, 0)
        logger.info("nwk_rejoin_failed_ind. status %d", status.status_code)

    def get_joined_rsp(self, rsp, rsp_len):
        if rsp.hdr.status_code == 0:
            jrsp = ncp_hl_get_joined_t.from_buffer_copy(rsp.body)
            logger.info("GET_JOINED rsp: joined %u, extended sts %u", jrsp.joined, jrsp.ext_sts)
            if jrsp.ext_sts == 3:               # if joined and has no parent
                self.get_joined_req_cycl_ctrl = 3	# after one step - stop ncp_req_get_joined() in loop
        else:
            self.rsp_log(rsp)
        if self.get_joined_req_cycl_ctrl > 0:
            if self.get_joined_req_cycl_ctrl == 1:
                sleep(0.2)
            else:
                sleep(3)
            self.host.ncp_req_get_joined()
            # check to stop the loop after last one step
            if self.get_joined_req_cycl_ctrl == 3:
                self.get_joined_req_cycl_ctrl = 0

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
