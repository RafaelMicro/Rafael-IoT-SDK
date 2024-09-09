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
# PURPOSE: Host-side test implements APS add group/remove group requests in ZR role.
#

# ZR adds endpoint #1 to groups 1111, 2222 and 3333. ZC sends packets to these groups sequentially.
# ZR receives packets intended to each group and removes ep from group 3333.
# ZR receives packet for groups 1111 and 2222 and removes ep from all groups.
# ZR doesn't receive any packets.

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner

logger = logging.getLogger(__name__)

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19

TEST_GROUP1  = 0x1111
TEST_GROUP2  = 0x2222
TEST_GROUP3  = 0x3333
TEST_EP      = 1


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_APSME_ADD_GROUP: self.apsme_add_group_rsp,
            ncp_hl_call_code_e.NCP_HL_APSME_RM_GROUP: self.apsme_rm_group_rsp,
            ncp_hl_call_code_e.NCP_HL_APSME_RM_ALL_GROUPS: self.apsme_rm_all_groups_rsp,
            ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC: self.af_simple_desc_set,
        })

        self.update_indication_switch({
        })

        self.required_packets = [ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,
                                 ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,
                                 ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,
                                 ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,
                                 ncp_hl_call_code_e.NCP_HL_APSME_ADD_GROUP,
                                 ncp_hl_call_code_e.NCP_HL_APSME_ADD_GROUP,
                                 ncp_hl_call_code_e.NCP_HL_APSME_ADD_GROUP,
                                 ncp_hl_call_code_e.NCP_HL_APSME_RM_GROUP,
                                 ncp_hl_call_code_e.NCP_HL_APSME_RM_ALL_GROUPS
                                 ]

        self.pkt_cnt = 0
        self.apsde_data_req_max = 15
        self.ignore_apsde_data = True
        
    def begin(self):   
        self.set_simple_desc_ep(1)
        self.set_ep = 1

    def af_simple_desc_set(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_nwk_discovery(0, self.channel_mask, 5)

    def on_nwk_join_complete(self, body):
        self.host.ncp_req_apsme_add_group(ncp_hl_nwk_addr_t(TEST_GROUP1), TEST_EP)

    def on_data_ind(self, dataind):
        self.send_packet_back(dataind)
        self.pkt_cnt += 1
        if self.pkt_cnt == 6:
            self.host.ncp_req_apsme_rm_group(ncp_hl_nwk_addr_t(TEST_GROUP3), TEST_EP)
        elif self.pkt_cnt == 10:
            self.host.ncp_req_apsme_rm_all_groups(TEST_EP)

    def apsme_add_group_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if self.set_ep == 1:
            self.set_ep = 2
            self.host.ncp_req_apsme_add_group(ncp_hl_nwk_addr_t(TEST_GROUP2), TEST_EP)
        elif self.set_ep == 2:
            self.set_ep = 3
            self.host.ncp_req_apsme_add_group(ncp_hl_nwk_addr_t(TEST_GROUP3), TEST_EP)

    def apsme_rm_group_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

    def apsme_rm_all_groups_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)


def main():
    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)
    fh = logging.FileHandler("zbs_aps_group.log")
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
