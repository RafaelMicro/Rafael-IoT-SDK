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
# PURPOSE: Host-side tool for NVRAM erasing.
#

import sys
import threading
import platform
from enum import IntEnum
from ctypes import *
import logging
from ncp_hl import *

logger = logging.getLogger(__name__)

#
# Test itself
#

CHANNEL_MASK = 0x80000            # channel 19

#reset options
erase_nvram_on_start = not ncp_on_nsng()

class zbs_nvram_erase:

    def __init__(self):
        self.rsp_switch = {
            ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION : self.get_module_version_rsp,
            ncp_hl_call_code_e.NCP_HL_NCP_RESET : self.ncp_reset_resp
        }

        self.ind_switch = {

        }

        self.reset_state = ncp_hl_rst_state_e.TURNED_ON

        # Is it PC with a simulator build or RPi with NCP connected over SPI?
    def get_ll_so_name(self):
        a = platform.machine()
        if a == "x86_64":
            return "../ncp_ll_nsng.so"
        else:
            return "./zbs_ncp_rpi.so"

    def run(self):
        self.host = ncp_host(self.get_ll_so_name(), self.rsp_switch, self.ind_switch)
        # main loop
        while True:
            self.host.wait_for_ncp()
            self.host.run_ll_quant(None, 0)
        pass

    def get_module_version_rsp(self, rsp, rsp_len):
        rsp_log(rsp, ncp_hl_module_ver_t)

    def ncp_reset_resp(self, rsp, rsp_len):
        rsp_log(rsp)
        if erase_nvram_on_start == True:
            if self.reset_state == ncp_hl_rst_state_e.TURNED_ON:
                self.reset_state = ncp_hl_rst_state_e.NVRAM_ERASE
                self.host.ncp_req_reset(ncp_hl_reset_opt_e.NVRAM_ERASE)
            elif self.reset_state == ncp_hl_rst_state_e.NVRAM_ERASE:
                self.reset_state = ncp_hl_rst_state_e.NVRAM_HAS_ERASED
                logger.info("NVRAM has been erased: status %d", rsp.hdr.status_code)
                self.host.ncp_req_get_module_version()
        else:
            self.host.ncp_req_get_module_version()


def main():

    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)

    test = zbs_nvram_erase()
    test.run()
    pass


if __name__ == "__main__":
    main()
