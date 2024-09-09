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
# PURPOSE: Host-side test implements ZR part of Echo test.
#

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner

logger = logging.getLogger(__name__)

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19S


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_NCP_RESET: self.ncp_reset_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_JOINED: self.get_joined_rsp,
        })

        self.update_indication_switch({
           })

        self.required_packets = [ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,
                                 ncp_hl_call_code_e.NCP_HL_GET_JOINED,
                                 ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,
                                 ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,
                                 ncp_hl_call_code_e.NCP_HL_GET_AUTHENTICATED,
                                 ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_GET_JOINED,
                                 ncp_hl_call_code_e.NCP_HL_GET_AUTHENTICATED,
                                 ]

    def begin(self):
        self.host.ncp_req_get_joined()

    def ncp_reset_rsp(self, rsp, rsp_len):
        if self.erase_nvram_on_start:
            if self.reset_state == ncp_hl_rst_state_e.TURNED_ON:
                self.reset_state = ncp_hl_rst_state_e.NVRAM_ERASE
                self.host.ncp_req_reset(ncp_hl_reset_opt_e.NVRAM_ERASE)
            elif self.reset_state == ncp_hl_rst_state_e.NVRAM_ERASE:
                self.reset_state = ncp_hl_rst_state_e.NVRAM_HAS_ERASED
                self.host.ncp_req_get_module_version()
            elif self.reset_state == ncp_hl_rst_state_e.NVRAM_HAS_ERASED:
                self.begin()

    def on_nwk_join_complete(self, body):
        self.host.ncp_req_get_authenticated()
        self.host.ncp_req_reset(ncp_hl_reset_opt_e.NO_OPTIONS)

    def get_joined_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            val = ncp_hl_uint8_t.from_buffer_copy(rsp.body, 0).uint8
            self.joined = val != 0
            strval = "False" if val == 0 else "True"
            logger.info("joined: %s", strval)

        if not self.joined:
            self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)
        else:
            self.host.ncp_req_get_authenticated()

    def on_data_ind(self, dataind):
        self.send_packet_back(dataind)


def main():
    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)
    fh = logging.FileHandler("zbs_nvram_zr.log")
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
