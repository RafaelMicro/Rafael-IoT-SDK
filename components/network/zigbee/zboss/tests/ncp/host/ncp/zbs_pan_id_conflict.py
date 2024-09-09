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
# PURPOSE: Host-side test implements PAN ID conflict indication and resolution.
#

# ZC gets PAN ID conflict indication and resolves it.

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
import subprocess
import time

logger = logging.getLogger(__name__)

#
# Test itself
#

CHANNEL_MASK = 0x80000   # channel 19
PAN_ID = 0x6D4B  # Arbitrary selected
INSTALL_CODE = bytes([0x83, 0xFE, 0xD3, 0x40, 0x7A, 0x93, 0x97, 0x23,
                      0xA5, 0xC6, 0x39, 0xB2, 0x69, 0x16, 0xD5, 0x05,
                      0xC3, 0xB5])
IC_IEEE_ADDR = bytes([0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZC)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_NCP_RESET: self.ncp_reset_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_JOINED: self.get_joined_rsp,
            ncp_hl_call_code_e.NCP_HL_SECUR_ADD_IC: self.secur_add_ic_rsp,
            ncp_hl_call_code_e.NCP_HL_NWK_START_WITHOUT_FORMATION: self.nwk_start_without_formation_rsp,
            ncp_hl_call_code_e.NCP_HL_NWK_PAN_ID_CONFLICT_RESOLVE: self.nwk_pan_id_conflict_resolve_rsp,
        })

        self.update_indication_switch({
            ncp_hl_call_code_e.NCP_HL_NWK_PAN_ID_CONFLICT_IND: self.nwk_pan_id_conflict_ind
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
                                 ncp_hl_call_code_e.NCP_HL_GET_JOINED,                                 
                                 ncp_hl_call_code_e.NCP_HL_NWK_FORMATION,
                                 ncp_hl_call_code_e.NCP_HL_NWK_PERMIT_JOINING,
                                 ncp_hl_call_code_e.NCP_HL_SECUR_ADD_IC,
                                 ncp_hl_call_code_e.NCP_HL_NWK_PAN_ID_CONFLICT_IND,
                                 ncp_hl_call_code_e.NCP_HL_NWK_PAN_ID_CONFLICT_RESOLVE
                                 ]

    def ncp_reset_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if self.erase_nvram_on_start:
            if self.reset_state == ncp_hl_rst_state_e.TURNED_ON:
                self.reset_state = ncp_hl_rst_state_e.NVRAM_ERASE
                self.host.ncp_req_reset(ncp_hl_reset_opt_e.NVRAM_ERASE)
            elif self.reset_state == ncp_hl_rst_state_e.NVRAM_ERASE:
                self.reset_state = ncp_hl_rst_state_e.NVRAM_HAS_ERASED
                self.host.ncp_req_get_module_version()
            elif self.reset_state == ncp_hl_rst_state_e.NVRAM_HAS_ERASED:
                time.sleep(15)
                self.begin()
                self.reset_state = "DO NOTHING"

    def begin(self):
        self.host.ncp_req_get_joined()

    def get_joined_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            val = ncp_hl_uint8_t.from_buffer_copy(rsp.body, 0).uint8
            self.joined = val != 0
            strval = "False" if val == 0 else "True"
            logger.info("joined: %s", strval)
        if not self.joined:
            self.host.ncp_req_set_nwk_key(self.nwk_key, 0)
        else:
            self.host.ncp_req_nwk_start_without_formation()

    def nwk_start_without_formation_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

    def secur_add_ic_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if self.reset_state != "DO NOTHING":
            self.host.ncp_req_reset(ncp_hl_reset_opt_e.NO_OPTIONS)

    def nwk_pan_id_conflict_ind(self, ind, ind_len):
        self.ind_log(ind, ncp_hl_nwk_pan_id_conflict_ind_t)
        req = ncp_hl_nwk_pan_id_conflict_ind_t.from_buffer(ind.body, 0)
        self.host.ncp_req_nwk_pan_id_resolve(req.panid_count, req.panids)

    def nwk_pan_id_conflict_resolve_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)


def main():
    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)
    fh = logging.FileHandler("zbs_pan_id_conflict.log")
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
