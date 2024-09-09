#!/usr/bin/env python3.7

#/* ZBOSS Zigbee software protocol stack
# *
# * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
# * www.dsr-zboss.com
# * www.dsr-corporation.com
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
# PURPOSE: Host-side test implements debug write
#

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner

logger = logging.getLogger(__name__)
log_file_name = "zbs_debug_write.log"

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION: self.get_module_version_rsp,
            ncp_hl_call_code_e.NCP_HL_DEBUG_WRITE: self.debug_write_rsp,
        })

        self.update_indication_switch({

        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,           NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_DEBUG_WRITE,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_DEBUG_WRITE,         NCP_HL_STATUS.RET_OK],
                                 ]

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

        self.nwk_key_disabled = True
        self.is_debug_write_sent = False

    def ncp_reset_resp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if erase_nvram_on_start == True:
            if self.reset_state == ncp_hl_rst_state_e.TURNED_ON:
                self.reset_state = ncp_hl_rst_state_e.NVRAM_ERASE
                self.host.ncp_req_reset(ncp_hl_reset_opt_e.NVRAM_ERASE)
            elif self.reset_state == ncp_hl_rst_state_e.NVRAM_ERASE:
                self.reset_state = ncp_hl_rst_state_e.NVRAM_HAS_ERASED
                self.host.ncp_req_get_module_version()
        else:
            self.host.ncp_req_get_module_version()

    def get_module_version_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_module_ver_t)
        self.host.ncp_req_debug_write(ncl_hl_debug_write_e.NCP_HL_DEBUG_WR_TEXT, "There is a demonstration of debug write request in text mode".encode())

    def debug_write_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if not self.is_debug_write_sent:
            self.is_debug_write_sent = True
            self.host.ncp_req_debug_write(ncl_hl_debug_write_e.NCP_HL_DEBUG_WR_BIN, bytes([0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                                                           0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F]))


def main():
    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)
    fh = logging.FileHandler(log_file_name)
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
