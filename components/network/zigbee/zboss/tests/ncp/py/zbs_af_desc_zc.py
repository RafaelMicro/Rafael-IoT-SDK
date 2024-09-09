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

logger = logging.getLogger(__name__)
log_file_name = "zbs_af_desc_zc.log"

#
# Test itself
#
CHANNEL_MASK = 0x80000    # channel 19
PAN_ID = 0x6D41
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
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZC)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ: self.zdo_node_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC: self.af_simple_desc_set,
            ncp_hl_call_code_e.NCP_HL_AF_DEL_SIMPLE_DESC: self.af_del_ep_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_BIND_REQ: self.zdo_bind_ep_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_UNBIND_REQ: self.zdo_unbind_ep_rsp,
            ncp_hl_call_code_e.NCP_HL_AF_SET_NODE_DESC: self.af_set_node_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_AF_SET_POWER_DESC: self.af_set_power_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_SIMPLE_DESC_REQ: self.zdo_simple_desc_rsp,
        })

        self.update_indication_switch({
            ncp_hl_call_code_e.NCP_HL_ZDO_DEV_ANNCE_IND : self.zdo_dev_annce_ind,
            ncp_hl_call_code_e.NCP_HL_NWK_LEAVE_IND : self.nwk_leave_ind
        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,           NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,               NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR,     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK, NCP_HL_STATUS.RET_OK],
                                 # FIXME: sequence of initialization commands was changed in base_test.
                                 # Expected packets need to be updated
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_NWK_KEY,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_NWK_KEYS,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_FORMATION,           NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_PERMIT_JOINING_REQ,  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_JOIN_USES_IC,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_ADD_IC,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_ROLE,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL_MASK, NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_PAN_ID,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_DEL_SIMPLE_DESC,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ,       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_SET_NODE_DESC,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ,       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_SET_POWER_DESC,       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ,       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_DEV_ANNCE_IND,       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_GET_IEEE_BY_SHORT,   NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_GET_NEIGHBOR_BY_IEEE,NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_GET_SHORT_BY_IEEE,   NCP_HL_STATUS.RET_OK],
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

        self.short_addr = ncp_hl_nwk_addr_t(0x0000) # ZC
        self.next_action = action_e.IDLE
        self.apsde_data_req_count = 0
        self.apsde_data_req_max = 1

        self.child_short_addr = ncp_hl_nwk_addr_t()
        self.child_long_addr = ncp_hl_ieee_addr_t()
        self.child_ep_num = 1

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

    def find_ep(self, ep_num):
        return next(ep for ep in self.endpoints if ep.hdr.endpoint == ep_num)

    def begin(self):
        self.host.ncp_req_set_nwk_key(self.nwk_key, 0)

    # FIXME: on_get_pan_id_rsp is not invoked in base_test anymore.
    # Test procedure is to be revised.
    def on_get_pan_id_rsp(self):
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
        self.host.ncp_req_zdo_node_desc(0x0000)

    def af_set_node_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.next_action = action_e.AF_SET_POWER_DESC
        self.host.ncp_req_zdo_node_desc(0x0000)

    def af_set_power_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.next_action = action_e.IDLE
        self.host.ncp_req_zdo_node_desc(0x0000)

    #
    # The following functions are called after receiving device announce
    #

    def on_nwk_get_short_by_ieee_rsp(self, short_addr):
        self.next_action = action_e.ZDO_BIND_EP

        self.host.ncp_req_zdo_node_desc(short_addr)

    def zdo_node_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_zdo_node_desc_t)
        if self.next_action == action_e.IDLE:
            pass
        elif self.next_action == action_e.AF_SET_NODE_DESC:
            self.host.ncp_req_af_set_node_desc(ncp_hl_role_e.NCP_HL_ZC, ncl_hl_mac_capability_e.NCP_HL_CAP_DEVICE_TYPE
                                                                        | ncl_hl_mac_capability_e.NCP_HL_CAP_POWER_SOURCE
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
            self.host.ncp_req_zdo_bind_ep(self.child_short_addr,
                                          self.child_long_addr, self.child_ep_num, 0x0003,
                                          self.ieee_addr, MY_EP2_NUM)
        else:
            logger.error("Unsolicited ZDO NODE DESC response!")

    def zdo_bind_ep_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_zdo_unbind_ep(self.child_short_addr,
                self.child_long_addr, self.child_ep_num, 0x0003,
                self.ieee_addr, MY_EP2_NUM)

    def zdo_unbind_ep_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_zdo_simple_desc(self.child_short_addr.u16, 1)

    def zdo_simple_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_set_simple_desc_hdr_t)

    def apsde_data_conf(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_apsde_data_conf_t)

    #
    # -- INDICATIONS --
    #

    def zdo_dev_annce_ind(self, ind, ind_len):
        annce = self.ind_rx(ind, ncp_hl_dev_annce_t)
        self.ind_log(ind, ncp_hl_dev_annce_t)
        self.child_short_addr = ncp_hl_nwk_addr_t(annce.short_addr)
        self.child_long_addr = ncp_hl_ieee_addr_t(annce.long_addr)
        self.host.ncp_req_nwk_get_ieee_by_short(annce.short_addr)

    def on_data_ind(self, dataind):
        self.send_packet_back(dataind, False)

    def nwk_leave_ind(self, ind, ind_len):
        self.ind_log(ind, ncp_hl_nwk_leave_ind_t)


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
