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
# PURPOSE: Host-side test implements ZR part of test with
# diagnostics the counter aps_unauthorized_key
#
# ZC - zbs_tcsw_zc
# ZR join network and initiates CBKE procedure
# Then ZR sends Simple Descriptor Requests
# After ZC stops responding, ZR do rejoin,
# detect that TC Swapped-out and initiates CBKE
# procedure with a "new" ZC
# 

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
from threading import Timer

logger = logging.getLogger(__name__)
log_file_name = "zbs_tcsw_zr.log"

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19

class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION: self.get_module_version_rsp_se,
            ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC: self.af_simple_desc_set,
            ncp_hl_call_code_e.NCP_HL_ZDO_SIMPLE_DESC_REQ: self.zdo_simple_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_REJOIN: self.rejoin_rsp,
            ncp_hl_call_code_e.NCP_HL_SECUR_START_KE: self.secur_cbke_rsp,
        })

        self.update_indication_switch({
        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,               NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,                   NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL_MASK,     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_PAN_ID,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_ROLE,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_START_POLL,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,               NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,               NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_REMOTE_CMD_IND,          NCP_HL_STATUS.RET_OK], # Node Descriptor Request from ZC
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_START_KE,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_SIMPLE_DESC_REQ,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_SIMPLE_DESC_REQ,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_REJOIN,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_REMOTE_CMD_IND,          NCP_HL_STATUS.RET_OK], # Node Descriptor Request from ZC
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_START_KE,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_SIMPLE_DESC_REQ,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_SIMPLE_DESC_REQ,         NCP_HL_STATUS.RET_OK],
                                 ]

        self.ch_mask = 0
        self.extpanid = ncp_hl_ext_panid_t()

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

    def get_module_version_rsp_se(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_module_ver_t)
        self.host.ncp_req_get_local_ieee_addr(0)

    def begin(self):
        self.set_simple_desc_ep1_se()
        self.set_ep = 1

    def on_nwk_join_complete(self, body):
        self.extpanid = ncp_hl_ext_panid_t(body.extpanid.b8)
        self.ch_mask = (1 << body.channel)
        self.host.ncp_req_secur_start_ke(2, 0x0000)

    def zdo_simple_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            self.t = Timer(5, self.send_sd_req)
            self.t.start()
        else:
            sts = status_id(rsp.hdr.status_category, rsp.hdr.status_code)
            if sts == NCP_HL_STATUS.ZDP_TIMEOUT:
                self.t.cancel()
                self.host.ncp_req_zdo_rejoin(self.extpanid, self.ch_mask, 0) # unsecure rejoin

    def send_sd_req(self):
        self.host.ncp_req_zdo_simple_desc(0, 1)

    def on_nwk_get_short_by_ieee_rsp(self, short_addr):
        self.host.ncp_req_zdo_node_desc(short_addr)

    def rejoin_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_uint8_t)
        flags = rsp.body.arr[0]
        if flags == 1:
            logger.debug("TC Swap-out attribute arrived")
            if rsp.hdr.status_code == 0:
                self.host.ncp_req_secur_start_ke(2, 0x0000)

    def secur_cbke_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            self.t = Timer(2, self.send_sd_req)
            self.t.start()

    def set_simple_desc_ep2(self):
        cmd = ncp_hl_set_simple_desc_t()
        cmd.hdr.endpoint = 20
        cmd.hdr.profile_id = 0x0109  # SE
        cmd.hdr.device_id = 0x1234
        cmd.hdr.device_version = 9
        cmd.hdr.in_clu_count = 1
        cmd.hdr.out_clu_count = 1
        cmd.clusters[0] = 0x0800  # ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT srv
        cmd.clusters[1] = 0x0800  # ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT cli
        self.set_ep = 2
        self.host.ncp_req_set_simple_desc(cmd)

    def af_simple_desc_set(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if self.set_ep == 1:
            self.set_simple_desc_ep2()
        else:
            self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)


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
