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
# PURPOSE: Host-side test implements rejoining in ZR part of Echo test.
#

# ZR joins to ZC and starts to rejoin every 3 seconds

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
from time import sleep

logger = logging.getLogger(__name__)

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY : self.nwk_discovery_complete,
        })

        self.update_indication_switch({
            ncp_hl_call_code_e.NCP_HL_NWK_JOINED_IND: self.nwk_rejoin_complete,
            ncp_hl_call_code_e.NCP_HL_NWK_JOIN_FAILED_IND : self.nwk_rejoin_failed
        })

        self.required_packets = [ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,
                                 ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,
                                 ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,
                                 ncp_hl_call_code_e.NCP_HL_NWK_JOINED_IND,
                                 ]

        self.extpanid = ncp_hl_ext_panid_t()
        self.mask = 0

    def begin(self):   
        self.host.ncp_req_nwk_discovery(0, self.channel_mask, 5)

    
    def nwk_discovery_complete(self, rsp, rsp_len):
        self.rsp_log(rsp)
        dh = ncp_hl_nwk_discovery_rsp_hdr_t.from_buffer(rsp.body)
        logger.debug("nwk discovery: network_count %d", dh.network_count)
        for i in range (0, dh.network_count):
            dsc = ncp_hl_nwk_discovery_dsc_t.from_buffer(rsp.body, sizeof(dh) + i * sizeof(ncp_hl_nwk_discovery_dsc_t))
            logger.debug("nwk #{}: extpanid {} panid {:#x} nwk_update_id {} page {} channel {} stack_profile {} permit_joining {} router_capacity {} end_device_capacity {}".format(
                i, dsc.extpanid, dsc.panid,
                dsc.nwk_update_id, dsc.page, dsc.channel,
                self.host.nwk_dsc_stack_profile(dsc.flags),
                self.host.nwk_dsc_permit_joining(dsc.flags),
                self.host.nwk_dsc_router_capacity(dsc.flags),
                self.host.nwk_dsc_end_device_capacity(dsc.flags)))
        # In our test we have only 1 network, so join to the first one
        dsc = ncp_hl_nwk_discovery_dsc_t.from_buffer(rsp.body, sizeof(dh))
        # Join thru association to the pan and channel just found
        self.extpanid = ncp_hl_ext_panid_t(dsc.extpanid.b8)
        self.mask = (1 << dsc.channel)
        self.host.ncp_req_nwk_join_req(dsc.extpanid, 0, dsc.page, (1 << dsc.channel), 5,
                                       ncl_hl_mac_capability_e.NCP_HL_CAP_ROUTER|ncl_hl_mac_capability_e.NCP_HL_CAP_ALLOCATE_ADDRESS,
                                       0)

    def on_nwk_join_complete(self, body):
        sleep(3)
        self.host.ncp_req_zdo_rejoin(self.extpanid, self.mask, 0)

    def nwk_rejoin_complete(self, ind, ind_len):
        logger.info("nwk_rejoin_complete.")
        body = ncp_hl_nwk_nlme_join_rsp_t.from_buffer(ind.body)

    def nwk_rejoin_failed(self, ind, ind_len):
        status = ncp_hl_status_t.from_buffer(ind.body, 0)
        self.rsp_log(rsp)

    def on_data_ind(self, dataind):
        self.send_packet_back(dataind)


def main():
    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)
    fh = logging.FileHandler("zbs_rejoin_zr.log")
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
