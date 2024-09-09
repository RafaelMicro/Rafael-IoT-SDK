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
# PURPOSE: Host-side test implements ZC part of Key Establishment.
#
# SE join and key establishment - ZC role.

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner

logger = logging.getLogger(__name__)
log_file_name = "zbs_se_key_est_zc_cs1.log"

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19
PAN_ID = 0x6D46  # Arbitrary selected


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZC)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION: self.get_module_version_rsp_se,
            ncp_hl_call_code_e.NCP_HL_ZDO_PERMIT_JOINING_REQ: self.nwk_permit_joining_complete,
            ncp_hl_call_code_e.NCP_HL_SECUR_ADD_IC: self.secur_add_ic_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ: self.zdo_node_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC: self.af_simple_desc_set,
        })

        self.update_indication_switch({
            ncp_hl_call_code_e.NCP_HL_SECUR_CBKE_SRV_FINISHED_IND: self.cbke_srv_finished_ind,
        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,                      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL_MASK,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_PAN_ID,                     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_ROLE,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_ADD_IC,                   NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_NWK_KEY,                    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_NWK_KEYS,                   NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_FORMATION,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_PERMIT_JOINING_REQ,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_DEV_ANNCE_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_GET_IEEE_BY_SHORT,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_GET_NEIGHBOR_BY_IEEE,       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_GET_SHORT_BY_IEEE,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_CBKE_SRV_FINISHED_IND,    NCP_HL_STATUS.RET_OK],
                                 ]

        self.use_cs = 1 # The difference between this test and zbs_se_key_est_zc_cs2 (another one in log name below)

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

    def on_get_module_version_rsp_se(self):
        self.host.ncp_req_set_zigbee_channel_mask(0, self.channel_mask)

    def begin(self):
        self.host.ncp_req_secur_add_ic(self.zr_ieee, INSTALL_CODE)

    def secur_add_ic_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_set_nwk_key(self.nwk_key, 0)

    def nwk_permit_joining_complete(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.set_simple_desc_ep1_se()
        self.set_ep = 1

    def on_nwk_get_short_by_ieee_rsp(self, short_addr):
        self.host.ncp_req_zdo_node_desc(short_addr)

    def zdo_node_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            self.rsp_log(rsp, ncp_hl_zdo_node_desc_t)

    def af_simple_desc_set(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if self.set_ep == 1:
            self.set_simple_desc_ep2_se()
            self.set_ep = 2

    def cbke_srv_finished_ind(self, ind, ind_len):
        self.ind_log(ind, ncp_hl_secur_cbke_srv_finished_ind_t)

    def on_dev_annce_ind(self, annce):
        self.host.ncp_req_nwk_get_ieee_by_short(annce.short_addr)


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
