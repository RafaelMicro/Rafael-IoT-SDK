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
# PURPOSE: Sample with a usage of nwk key in ZC role.
#

# This sample enables custom NWK key for debug purposes.

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner

logger = logging.getLogger(__name__)

#
# Test itself
#

CHANNEL_MASK = 0x80000   # channel 19
PAN_ID = 0x6D44  # Arbitrary selected


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZC)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_SET_NWK_KEY: self.set_nwk_key_rsp,
            ncp_hl_call_code_e.NCP_HL_SECUR_ADD_IC: self.secur_add_ic_rsp
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
                                 ncp_hl_call_code_e.NCP_HL_ZDO_DEV_ANNCE_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ]

    def begin(self):
        self.host.ncp_req_set_nwk_key(self.nwk_key, 0)

    def secur_add_ic_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

    def on_data_ind(self, dataind):
        self.send_packet_back(dataind)


def main():
    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)
    fh = logging.FileHandler("zbs_nwk_key_zc.log")
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
