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
# PURPOSE: Trivial Host-side test for ZDO descriptor requests.
#
# Request Simple descriptor, Active EP, Match descriptors

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner

logger = logging.getLogger(__name__)

#
# Test itself
#

CHANNEL_MASK = 0x80000            # channel 19

INSTALL_CODE = bytes([0x83, 0xFE, 0xD3, 0x40, 0x7A, 0x93, 0x97, 0x23,
                      0xA5, 0xC6, 0x39, 0xB2, 0x69, 0x16, 0xD5, 0x05,
                      0xC3, 0xB5])
IC_IEEE_ADDR = bytes([0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
PAN_ID = 0x6D48


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZC)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_ZDO_NWK_ADDR_REQ: self.zdo_nwk_addr_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_IEEE_ADDR_REQ: self.zdo_ieee_addr_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_POWER_DESC_REQ: self.zdo_power_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ: self.zdo_node_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_SIMPLE_DESC_REQ: self.zdo_simple_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_ACTIVE_EP_REQ: self.zdo_active_ep_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_MATCH_DESC_REQ: self.zdo_match_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_NWK_GET_IEEE_BY_SHORT: self.nwk_get_ieee_by_short_rsp,
        })

        self.update_indication_switch({
        })

        self.required_packets = [ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,
                                 ncp_hl_call_code_e.NCP_HL_SET_PAN_ID,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,
                                 ncp_hl_call_code_e.NCP_HL_SET_NWK_KEY,
                                 ncp_hl_call_code_e.NCP_HL_GET_NWK_KEYS,
                                 ncp_hl_call_code_e.NCP_HL_NWK_FORMATION,
                                 ncp_hl_call_code_e.NCP_HL_NWK_PERMIT_JOINING,
                                 ncp_hl_call_code_e.NCP_HL_SECUR_ADD_IC,
                                 ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_ROLE,
                                 ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL_MASK,
                                 ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL,
                                 ncp_hl_call_code_e.NCP_HL_GET_PAN_ID,
                                 ncp_hl_call_code_e.NCP_HL_ZDO_DEV_ANNCE_IND,
                                 ncp_hl_call_code_e.NCP_HL_NWK_GET_IEEE_BY_SHORT,
                                 ncp_hl_call_code_e.NCP_HL_NWK_GET_NEIGHBOR_BY_IEEE,
                                 ncp_hl_call_code_e.NCP_HL_NWK_GET_SHORT_BY_IEEE,
                                 ncp_hl_call_code_e.NCP_HL_ZDO_IEEE_ADDR_REQ,
                                 ncp_hl_call_code_e.NCP_HL_ZDO_IEEE_ADDR_REQ,
                                 ncp_hl_call_code_e.NCP_HL_ZDO_NWK_ADDR_REQ,
                                 ncp_hl_call_code_e.NCP_HL_ZDO_NWK_ADDR_REQ,
                                 ncp_hl_call_code_e.NCP_HL_ZDO_POWER_DESC_REQ,
                                 ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ,
                                 ncp_hl_call_code_e.NCP_HL_ZDO_SIMPLE_DESC_REQ,
                                 ncp_hl_call_code_e.NCP_HL_ZDO_ACTIVE_EP_REQ,
                                 ncp_hl_call_code_e.NCP_HL_ZDO_MATCH_DESC_REQ,
                                 ]

    def begin(self):
        self.host.ncp_req_set_nwk_key(self.nwk_key, 0)

    def zdo_ieee_addr_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            addr_rsp = ncp_hl_zdo_addr_rsp_t.from_buffer_copy(rsp.body, 0)
            logger.info("zdo_ieee_addr_rsp {:{opt}}".format(addr_rsp, opt=self.zdo_addr_opt()))
            if self.zdo_addr_req_type == ncl_hl_zdo_addr_req_type_e.NCP_HL_ZDO_SINGLE_DEV_REQ:
                self.zdo_addr_req_type = ncl_hl_zdo_addr_req_type_e.NCP_HL_ZDO_EXTENDED_REQ
                self.host.ncp_req_zdo_ieee_addr(self.short_addr, self.short_addr, self.zdo_addr_req_type, 0)
            else:
                self.zdo_addr_req_type = ncl_hl_zdo_addr_req_type_e.NCP_HL_ZDO_SINGLE_DEV_REQ
                self.host.ncp_req_zdo_nwk_addr(self.short_addr, addr_rsp.ieee_addr_remote_dev, self.zdo_addr_req_type,
                                               0)

    def zdo_nwk_addr_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            addr_rsp = ncp_hl_zdo_addr_rsp_t.from_buffer_copy(rsp.body, 0)
            logger.info("zdo_nwk_addr_rsp {:{opt}}".format(addr_rsp, opt=self.zdo_addr_opt()))
            if self.zdo_addr_req_type == ncl_hl_zdo_addr_req_type_e.NCP_HL_ZDO_SINGLE_DEV_REQ:
                self.zdo_addr_req_type = ncl_hl_zdo_addr_req_type_e.NCP_HL_ZDO_EXTENDED_REQ
                self.host.ncp_req_zdo_nwk_addr(self.short_addr, addr_rsp.ieee_addr_remote_dev, self.zdo_addr_req_type,
                                               0)
            else:
                self.host.ncp_req_zdo_power_desc(self.short_addr.u16)

    def zdo_power_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            power_desc = ncp_hl_zdo_power_desc_t.from_buffer_copy(rsp.body, 0)
            logger.info("power desc: {}".format(power_desc))
            self.host.ncp_req_zdo_node_desc(self.short_addr.u16)

    def zdo_node_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_zdo_node_desc_t)
        if rsp.hdr.status_code == 0:
            self.host.ncp_req_zdo_simple_desc(self.short_addr.u16, 1)
                                         
    def zdo_simple_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_set_simple_desc_hdr_t)
        if rsp.hdr.status_code == 0:
            self.host.ncp_req_zdo_active_ep_desc(self.short_addr.u16) 

    def zdo_active_ep_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_active_ep_desc_t)
        if rsp.hdr.status_code == 0:
            cmd = ncp_hl_match_desc_t()
            cmd.hdr.addr_of_interest = self.short_addr.u16
            cmd.hdr.profile_id = 0x0109
            cmd.hdr.in_clu_count = 3
            cmd.hdr.out_clu_count = 2
            cmd.clusters[0] = 1000
            cmd.clusters[1] = 1001
            cmd.clusters[2] = 1002
            cmd.clusters[3] = 2000
            cmd.clusters[4] = 3
            self.host.ncp_req_zdo_match_desc(cmd)   

    def zdo_match_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_match_desc_rsp_t)

    def nwk_get_short_by_ieee_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            self.short_addr = ncp_hl_nwk_addr_t.from_buffer_copy(rsp.body, 0)
            logger.info("nwk_get_short_by_ieee_rsp short_addr {:#x}".format(self.short_addr))
        self.zdo_addr_req_type = ncl_hl_zdo_addr_req_type_e.NCP_HL_ZDO_SINGLE_DEV_REQ
        self.host.ncp_req_zdo_ieee_addr(self.short_addr, self.short_addr, self.zdo_addr_req_type, 0)

    def on_dev_annce_ind(self, annce):
        self.host.ncp_req_nwk_get_ieee_by_short(annce.short_addr)

    def nwk_leave_ind(self, ind, ind_len):
        leave_ind = ncp_hl_nwk_leave_ind_t.from_buffer(ind.body, 0)
        logger.info("nwk_leave_ind: ieee {} rejoin {}".format(list(map(hex, list(leave_ind.ieee_addr))),
                                                              leave_ind.rejoin))


def main():
    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)
    fh = logging.FileHandler("zbs_zdo_desc.log")
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
