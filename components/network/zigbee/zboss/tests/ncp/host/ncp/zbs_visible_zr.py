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
# PURPOSE: Sample with visible requests in ZR role.
#

# ZR tries to join to ZC changing MAC visibility on each failed attempt.

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
from time import sleep

logger = logging.getLogger(__name__)

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19

delay = 1


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY: self.nwk_discovery_complete,
            ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN:self.nwk_join_complete,
            ncp_hl_call_code_e.NCP_HL_ADD_VISIBLE_DEV: self.add_visible_dev_rsp,
            ncp_hl_call_code_e.NCP_HL_ADD_INVISIBLE_SHORT: self.add_invisible_short_rsp,
            ncp_hl_call_code_e.NCP_HL_RM_INVISIBLE_SHORT: self.rm_invisible_short_rsp,
        })

        self.update_indication_switch({
        })

        self.required_packets = [ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,
                                 ncp_hl_call_code_e.NCP_HL_ADD_VISIBLE_DEV,
                                 ncp_hl_call_code_e.NCP_HL_ADD_INVISIBLE_SHORT,
                                 ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,
                                 ncp_hl_call_code_e.NCP_HL_RM_INVISIBLE_SHORT,
                                 ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,
                                 ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,
                                 ncp_hl_call_code_e.NCP_HL_ADD_VISIBLE_DEV,
                                 ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,
                                 ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,
                                 ]

        self.zc_ieee = ncp_hl_ieee_addr_t()
        self.step = 0

    def begin(self):  
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
        dh = ncp_hl_nwk_discovery_rsp_hdr_t.from_buffer(rsp.body)
        for i in range(0, dh.network_count):
            dsc = ncp_hl_nwk_discovery_dsc_t.from_buffer(rsp.body, sizeof(dh) + i * sizeof(ncp_hl_nwk_discovery_dsc_t))
            self.rsp_log(rsp, ncp_hl_nwk_discovery_dsc_t)
        # In our test we have only 1 network, so join to the first one
        dsc = ncp_hl_nwk_discovery_dsc_t.from_buffer(rsp.body, sizeof(dh))
        # Join thru association to the pan and channel just found
        if rsp.hdr.status_code == 0xEA: # MAC_NO_BEACON
            self.host.ncp_req_rm_invisible_short(0)
        else:
            self.zc_ieee = dsc.extpanid
            self.host.ncp_req_nwk_join_req(dsc.extpanid, 0, dsc.page, (1 << dsc.channel), 5,
                                       ncl_hl_mac_capability_e.NCP_HL_CAP_ROUTER|ncl_hl_mac_capability_e.NCP_HL_CAP_ALLOCATE_ADDRESS,
                                       0)
   
    def nwk_join_complete(self, rsp, rsp_len):
        self.rsp_log(rsp)
        body = ncp_hl_nwk_nlme_join_rsp_t.from_buffer(rsp.body)
        if rsp.hdr.status_code == 0:
            self.rsp_log(rsp, ncp_hl_nwk_nlme_join_rsp_t)
        elif rsp.hdr.status_code == 0xEB: # MAC_NO_DATA
            self.host.ncp_req_add_visible_dev(self.zc_ieee)

    def on_data_ind(self, dataind):
        self.send_packet_back(dataind)


def main():
    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)
    fh = logging.FileHandler("zbs_visible_zr.log")
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
