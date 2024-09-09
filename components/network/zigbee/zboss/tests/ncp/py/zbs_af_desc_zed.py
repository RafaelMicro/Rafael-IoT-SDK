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
# PURPOSE: Trivial Host-side test for AF descriptor requests.
#
# Setup simple descriptor, receive APS

from enum import Enum
from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
import time

logger = logging.getLogger(__name__)
log_file_name = "zbs_af_desc_zed.log"

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19
MY_EP1_NUM = 1
MY_EP2_NUM = 20


class action_e(Enum):
    IDLE                    = 0
    AF_SET_SIMPLE_DESC_EP2  = 1
    AF_DEL_EP1              = 2
    AF_SET_NODE_DESC        = 3
    AF_SET_POWER_DESC       = 4
    ZDO_BIND_EP             = 5
    DONE                    = 6

class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZED)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE: self.set_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_SET_ED_TIMEOUT: self.set_ed_timeout_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_ED_TIMEOUT: self.get_ed_timeout_rsp,
            ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN: self.nwk_join_complete,
            ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ: self.zdo_node_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC: self.af_simple_desc_set,
            ncp_hl_call_code_e.NCP_HL_AF_DEL_SIMPLE_DESC: self.af_del_ep_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_BIND_REQ: self.zdo_bind_ep_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_UNBIND_REQ: self.zdo_unbind_ep_rsp,
            ncp_hl_call_code_e.NCP_HL_AF_SET_NODE_DESC: self.af_set_node_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_AF_SET_POWER_DESC: self.af_set_power_desc_rsp,

        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,           NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,               NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR,     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK, NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL_MASK, NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_PAN_ID,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_ROLE,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE,     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ED_TIMEOUT,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ED_TIMEOUT,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,           NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,           NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_DEL_SIMPLE_DESC,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ,       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_SET_NODE_DESC,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ,       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_SET_POWER_DESC,       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ,       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_BIND_REQ,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_UNBIND_REQ,          NCP_HL_STATUS.RET_OK],
                                 ]

        self.endpoints = [
            # Let it be on/off server. Can test it with our standalone on_off_switch_zed.c
            ncp_hl_make_simple_desc(MY_EP1_NUM,    # endpoint
                                    0x104,      # profile_id - HA & ZB 3.0
                                    0x1234,     # device_id
                                    8,          # device_version
                                    [0x0003,    # srv - ZB_ZCL_CLUSTER_ID_IDENTIFY
                                     0x0000,    # srv - ZB_ZCL_CLUSTER_ID_BASIC
                                     0x0006,    # srv - ZB_ZCL_CLUSTER_ID_ON_OFF
                                     0x0004,    # srv - ZB_ZCL_CLUSTER_ID_GROUPS
                                     0x0005],   # srv - ZB_ZCL_CLUSTER_ID_SCENES
                                    []),
            # Let it be IHD
            ncp_hl_make_simple_desc(MY_EP2_NUM,    # endpoint
                                    0x0109,     # profile_id - SE
                                    0x1234,     # device_id
                                    9,          # device_version
                                    [0x0000,    # srv - ZB_ZCL_CLUSTER_ID_BASIC
                                     0x0003],   # srv - ZB_ZCL_CLUSTER_ID_IDENTIFY
                                    [0x0707,    # cli - ZB_ZCL_CLUSTER_ID_CALENDAR
                                     0x0702,    # cli - ZB_ZCL_CLUSTER_ID_METERING
                                     0x0700,    # cli - ZB_ZCL_CLUSTER_ID_PRICE
                                     0x0703,    # cli - ZB_ZCL_CLUSTER_ID_MESSAGING
                                     0x0003]),  # cli - ZB_ZCL_CLUSTER_ID_IDENTIFY
            ]

        self.next_action = action_e.IDLE

        self.update_indication_switch({
        })

        self.short_addr = ncp_hl_nwk_addr_t().u16
        self.ep_num = 1

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

    def find_ep(self, ep_num):
        return next(ep for ep in self.endpoints if ep.hdr.endpoint == ep_num)

    def begin(self):
       self.host.ncp_req_set_rx_on_when_idle(ncp_hl_on_off_e.OFF)

    def set_ed_timeout_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_get_ed_timeout()

    def get_ed_timeout_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_ed_to_t)
        self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)

    def set_rx_on_when_idle_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_set_ed_timeout(10) # default = 8

    def nwk_join_complete(self, rsp, rsp_len):
        self.rsp_log(rsp)
        body = ncp_hl_nwk_nlme_join_rsp_t.from_buffer_copy(rsp.body)
        if rsp.hdr.status_code == 0:
            logger.debug("joined ok short_addr {:#x} extpanid {} page {} channel {} enh_beacon {} mac_if {}".format(
                body.short_addr, body.extpanid,
                body.page, body.channel, body.enh_beacon, body.mac_iface_idx))
            self.short_addr = body.short_addr.u16
        self.next_action = action_e.AF_SET_SIMPLE_DESC_EP2
        self.host.ncp_req_set_simple_desc(self.find_ep(MY_EP1_NUM))

    def af_simple_desc_set(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if self.next_action == action_e.AF_SET_SIMPLE_DESC_EP2:
            self.next_action = action_e.AF_DEL_EP1
            self.host.ncp_req_set_simple_desc(self.find_ep(MY_EP2_NUM))
        elif self.next_action == action_e.AF_DEL_EP1:
            self.host.ncp_req_af_del_ep(MY_EP1_NUM)
        else:
            logger.error("Unsolicited AF SIMPLE DESC SET response!")

    def af_del_ep_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.next_action = action_e.AF_SET_NODE_DESC
        self.host.ncp_req_zdo_node_desc(self.short_addr)

    def af_set_node_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.next_action = action_e.AF_SET_POWER_DESC
        self.host.ncp_req_zdo_node_desc(self.short_addr)

    def af_set_power_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.next_action = action_e.ZDO_BIND_EP
        self.host.ncp_req_zdo_node_desc(self.short_addr)

    def on_nwk_get_short_by_ieee_rsp(self, short_addr):
        self.next_action = action_e.ZDO_BIND_EP
        self.host.ncp_req_zdo_node_desc(short_addr)

    def zdo_node_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_zdo_node_desc_t)
        if self.next_action == action_e.IDLE:
            pass
        elif self.next_action == action_e.AF_SET_NODE_DESC:
            self.host.ncp_req_af_set_node_desc(ncp_hl_role_e.NCP_HL_ZC, ncl_hl_mac_capability_e.NCP_HL_CAP_POWER_SOURCE
                                                                        | ncl_hl_mac_capability_e.NCP_HL_CAP_RX_ON_WHEN_IDLE
                                                                        | ncl_hl_mac_capability_e.NCP_HL_CAP_ALLOCATE_ADDRESS, 0xFACE)
        elif self.next_action == action_e.AF_SET_POWER_DESC:
            self.host.ncp_req_af_set_power_desc(ncp_hl_current_power_mode_e.NCP_HL_CUR_PWR_MODE_SYNC_ON_WHEN_IDLE,
                                                ncp_hl_power_srcs_e.NCP_HL_PWR_SRCS_CONSTANT
                                                | ncp_hl_power_srcs_e.NCP_HL_PWR_SRCS_RECHARGEABLE_BATTERY
                                                | ncp_hl_power_srcs_e.NCP_HL_PWR_SRCS_DISPOSABLE_BATTERY,
                                                ncp_hl_power_srcs_e.NCP_HL_PWR_SRCS_CONSTANT,
                                                ncp_hl_power_source_level.NCP_HL_PWR_SRC_LVL_100)
        elif self.next_action == action_e.ZDO_BIND_EP:
            self.host.ncp_req_zdo_bind_ep(ncp_hl_nwk_addr_t(self.short_addr),
                                          ncp_hl_ieee_addr_t(self.ieee_addr), self.ep_num, 0x0003,
                                          ncp_hl_ieee_addr_t(self.ic_ieee_addr), MY_EP2_NUM)
        else:
            logger.error("Unsolicited ZDO NODE DESC response!")

    def zdo_bind_ep_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_zdo_unbind_ep(ncp_hl_nwk_addr_t(self.short_addr),
                ncp_hl_ieee_addr_t(self.ieee_addr), self.ep_num, 0x0003,
                ncp_hl_ieee_addr_t(self.ic_ieee_addr), MY_EP2_NUM)

    def zdo_unbind_ep_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

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
