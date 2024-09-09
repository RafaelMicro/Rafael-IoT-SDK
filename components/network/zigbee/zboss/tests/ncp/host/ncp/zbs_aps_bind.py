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
# PURPOSE: Host-side test implements APS bind/unbind requests in ZR role.
#

# ZR starts sending packets to endpoint #2 resides locally and gets UNBOUND error.
# Then ZR binds endpoints #1 and #2 together and receives packet intended to ep #2.
# ZR unbinds ep #1 and #2 and gets UNBOUND error again.


from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner

logger = logging.getLogger(__name__)

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_APSME_BIND: self.apsme_bind_rsp,
            ncp_hl_call_code_e.NCP_HL_APSME_UNBIND: self.apsme_unbind_rsp,
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
                                 ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,
                                 ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,
                                 ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ncp_hl_call_code_e.NCP_HL_APSME_BIND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSME_UNBIND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ]

    def begin(self):
        self.set_simple_desc_ep(1)
        self.set_ep = 1

    def af_simple_desc_set(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if self.set_ep == 1:
            self.set_simple_desc_ep(2)
            self.set_ep = 2
        elif self.set_ep == 2:
            self.set_ep = 3
            self.host.ncp_req_nwk_discovery(0, self.channel_mask, 5)

    def on_nwk_join_complete(self, body):
        self.short_addr = ncp_hl_nwk_addr_t(body.short_addr.u16)
        self.send_data()

    def on_data_ind(self, dataind):
        if self.set_ep == 4:
            self.set_ep = 5
            self.host.ncp_req_apsme_unbind_request(ncp_hl_ieee_addr_t(self.ieee_addr), 0x01, 0x0003,
                                                   ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT,
                                                   self.short_addr, 0x02)

    def on_data_conf(self, conf):
            if self.set_ep == 3:
                self.set_ep = 4
                self.host.ncp_req_apsme_bind_request(ncp_hl_ieee_addr_t(self.ieee_addr), 0x01, 0x0003,
                                                     ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT,
                                                     self.short_addr, 0x02)

    def apsme_bind_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.send_data()

    def apsme_unbind_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.send_data()

    def send_data(self):
        params = ncp_hl_data_req_param_t()
        params.addr_mode = ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT
        params.profileid = 0x0104
        params.clusterid = 0x0003
        params.src_endpoint = 1
        params.tx_options = ncp_hl_tx_options_e.NCP_HL_TX_OPT_SECURITY_ENABLED | ncp_hl_tx_options_e.NCP_HL_TX_OPT_ACK_TX
        params.use_alias = 0
        params.alias_src_addr = 0
        params.alias_seq_num = 0
        params.radius = 30
        data = c_ubyte * 128
        self.host.ncp_req_apsde_data_request(params, data(1, 2, 3, 4, 5, 6, 7, 8, 9, 10), 10)


def main():
    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)
    fh = logging.FileHandler("zbs_aps_bind.log")
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
