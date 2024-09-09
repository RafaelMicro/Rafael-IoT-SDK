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
# PURPOSE: Trivial Host-side test for ZDO descriptor requests.
#
# Request Simple descriptor, Active EP, Match descriptors

from enum import Enum
from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
import time

logger = logging.getLogger(__name__)
log_file_name = "zbs_zdo_desc_zed.log"

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19

class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZED)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE: self.set_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_POWER_DESC_REQ: self.zdo_power_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ: self.zdo_node_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_SIMPLE_DESC_REQ: self.zdo_simple_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_ACTIVE_EP_REQ: self.zdo_active_ep_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_MATCH_DESC_REQ: self.zdo_match_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_IEEE_ADDR_REQ: self.zdo_ieee_addr_rsp,
        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL_MASK,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_PAN_ID,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_ROLE,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_GET_IEEE_BY_SHORT,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_GET_NEIGHBOR_BY_IEEE,   NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_GET_SHORT_BY_IEEE,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_IEEE_ADDR_REQ,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_POWER_DESC_REQ,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_ACTIVE_EP_REQ,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_SIMPLE_DESC_REQ,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_MATCH_DESC_REQ,         NCP_HL_STATUS.RET_OK],
                                 ]

        self.update_indication_switch({
        })

        self.zc_short_addr = ncp_hl_nwk_addr_t(0x0000)

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

    def begin(self):
       self.host.ncp_req_set_rx_on_when_idle(ncp_hl_on_off_e.OFF)

    def set_rx_on_when_idle_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)

    def nwk_join_complete(self, rsp, rsp_len):
        self.rsp_log(rsp)
        body = ncp_hl_nwk_nlme_join_rsp_t.from_buffer_copy(rsp.body)
        if rsp.hdr.status_code == 0:
            logger.debug("joined ok short_addr {:#x} extpanid {} page {} channel {} enh_beacon {} mac_if {}".format(
                body.short_addr, body.extpanid,
                body.page, body.channel, body.enh_beacon, body.mac_iface_idx))
        logger.info("nwk_join_complete done")
        self.host.ncp_req_nwk_get_ieee_by_short(self.zc_short_addr.u16)

    def zdo_ieee_addr_rsp(self, rsp, rsp_len):
        logger.info("nwk_join_complete done")
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            self.short_addr = ncp_hl_nwk_addr_t.from_buffer_copy(rsp.body, 0)
            logger.info("nwk_get_short_by_ieee_rsp short_addr {:#x}".format(self.short_addr))
        self.host.ncp_req_zdo_power_desc(self.zc_short_addr.u16)

    def zdo_power_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            power_desc = ncp_hl_zdo_power_desc_t.from_buffer_copy(rsp.body, 0)
            logger.info("power desc: {}".format(power_desc))
            self.host.ncp_req_zdo_node_desc(self.zc_short_addr.u16)

    def zdo_node_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_zdo_node_desc_t)
        self.host.ncp_req_zdo_active_ep_desc(self.zc_short_addr.u16)

    def zdo_active_ep_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_active_ep_desc_t)
        body = ncp_hl_active_ep_desc_t.from_buffer_copy(rsp.body)
        ep = body.active_ep_list[0]
        self.host.ncp_req_zdo_simple_desc(self.zc_short_addr.u16, ep)

    def zdo_simple_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_set_simple_desc_hdr_t)
        if rsp.hdr.status_code == 0:
            cmd = ncp_hl_match_desc_t()
            cmd.hdr.addr_of_interest = self.zc_short_addr.u16
            cmd.hdr.profile_id = 0x7b
            cmd.hdr.in_clu_count = 3
            cmd.hdr.out_clu_count = 2
            cmd.clusters[0] = 1000
            cmd.clusters[1] = 1001
            cmd.clusters[2] = 1002
            cmd.clusters[3] = 2000
            cmd.clusters[4] = 2001
            self.host.ncp_req_zdo_match_desc(cmd)

    def nwk_get_short_by_ieee_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            self.short_addr = ncp_hl_nwk_addr_t.from_buffer_copy(rsp.body, 0)
            logger.info("nwk_get_short_by_ieee_rsp short_addr {:#x}".format(self.short_addr))
        self.zdo_addr_req_type = ncl_hl_zdo_addr_req_type_e.NCP_HL_ZDO_SINGLE_DEV_REQ
        self.host.ncp_req_zdo_ieee_addr(self.short_addr, self.short_addr, self.zdo_addr_req_type, 0)

    def zdo_match_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        body = ncp_hl_match_desc_rsp_t.from_buffer_copy(rsp.body)
        data4dump = list(body.match_list)[:body.match_len]
        logger.info("match_len {} match_list {}".format(body.match_len, list(data4dump)))

    def apsde_data_conf(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_apsde_data_conf_t)

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
