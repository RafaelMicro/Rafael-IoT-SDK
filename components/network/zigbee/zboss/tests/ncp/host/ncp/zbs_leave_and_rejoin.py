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
# PURPOSE: Host-side test implements rejoin on leave.
# Host is ZR and gets leave+rejoin command from parent.
#

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
            ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY: self.nwk_discovery_complete,
            ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN: self.nwk_join_complete,
        })

        self.update_indication_switch({
            ncp_hl_call_code_e.NCP_HL_NWK_LEAVE_IND : self.nwk_leave_ind,
            ncp_hl_call_code_e.NCP_HL_NWK_JOINED_IND :self.nwk_rejoin_complete,
            ncp_hl_call_code_e.NCP_HL_NWK_JOIN_FAILED_IND :self.nwk_rejoin_failed
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

        self.apsde_data_req_max = 100
        self.ignore_apsde_data = True # As the standalone ZC sends packets infinitely.
    
    def begin(self):
       self.host.ncp_req_nwk_discovery(0, self.channel_mask, 5)

    def nwk_leave_ind(self, ind, ind_len):
        self.ind_log(ind, ncp_hl_nwk_leave_ind_t)

    def nwk_rejoin_complete(self, ind, ind_len):
        logger.info("nwk_rejoin_complete.")
        body = ncp_hl_nwk_nlme_join_rsp_t.from_buffer(ind.body)
        self.nwk_joined(body)

    def nwk_joined(self, body):
        logger.debug("joined ok short_addr {:#x} extpanid {} page {} channel {} enh_beacon {} mac_if {}".format(
            body.short_addr, body.extpanid,
            body.page, body.channel, body.enh_beacon, body.mac_iface_idx))

    def nwk_rejoin_failed(self, ind, ind_len):
        status = ncp_hl_status_t.from_buffer(ind.body, 0)
        self.ind_log(ind)

    def on_data_ind(self, dataind):
        # self.send_packet_back(dataind)
        pass


def main():
    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)
    fh = logging.FileHandler("zbs_leave_and_rejoin.log")
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
