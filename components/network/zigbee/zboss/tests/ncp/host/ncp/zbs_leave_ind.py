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
# PURPOSE: Host-side test implements leave indication.
#

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
PAN_ID = 0x6D43


class Test(BaseTest):
    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZC)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_ZDO_MGMT_LEAVE_REQ : self.zdo_mgmt_leave_rsp
        })

        self.update_indication_switch({
            ncp_hl_call_code_e.NCP_HL_NWK_LEAVE_IND : self.nwk_leave_ind
        })

        self.child_nwk_addr = ncp_hl_nwk_addr_t()
        self.child_ieee_addr = ncp_hl_ieee_addr_t()

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
                                 ncp_hl_call_code_e.NCP_HL_ZDO_MGMT_LEAVE_REQ, 
                                 ncp_hl_call_code_e.NCP_HL_NWK_LEAVE_IND,
                                 ]
        
    def begin(self):
        self.host.ncp_req_set_nwk_key(self.nwk_key, 0)

    def on_nwk_get_short_by_ieee_rsp(self, short_addr):
        self.host.ncp_req_zdo_mgmt_leave(
                short_addr,
                self.child_ieee_addr,
                NCP_HL_LEAVE_FLAGS.REMOVE_CHILDREN | NCP_HL_LEAVE_FLAGS.REJOIN)

    def zdo_mgmt_leave_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

    def on_dev_annce_ind(self, annce):
        self.host.ncp_req_nwk_get_ieee_by_short(annce.short_addr)

    def nwk_leave_ind(self, ind, ind_len):
        self.ind_log(ind, ncp_hl_nwk_leave_ind_t)


def main():
    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)

    fh = logging.FileHandler("zbs_leave_ind.log")
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
