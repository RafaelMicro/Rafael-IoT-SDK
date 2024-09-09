#!/usr/bin/env python3.7

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
# PURPOSE: Host-side test implements ZED to issue Get Configuration Parameter commands
#
# No partner required.
#

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
from time import sleep

logger = logging.getLogger(__name__)
log_file_name = "zbs_get_cfg_prms_zed.log"

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19

# classes for parsing response
class rsp_s(Structure):
    _pack_ = 1
    _fields_ = [("u16_1", c_ushort)]

class rsp_s_s(Structure):
    _pack_ = 1
    _fields_ = [("u16_1", c_ushort),
                ("u16_2", c_ushort)]

class rsp_c_c(Structure):
    _pack_ = 1
    _fields_ = [("i8_1", c_byte),
                ("i8_2", c_byte)]

class rsp_b_b_s(Structure):
    _pack_ = 1
    _fields_ = [("u8_1", c_ubyte),
                ("u8_2", c_ubyte),
                ("u16_1", c_ushort)]

class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZED)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION: self.get_module_version_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER: self.get_cfg_prms_rsp,
        })

        self.update_indication_switch({
        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,           NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,               NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],# 24 pcs (cp_id_last)
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_NOT_FOUND], #ZLL isn't supported
                                 [ncp_hl_call_code_e.NCP_HL_GET_CONFIG_PARAMETER,    NCP_HL_STATUS.RET_OK],
                                 ]

        self.cp_id = 1  # configuration parameter id, consistently increases to cp_id_last
        self.cp_id_last = 24 # configuration parameter last id, if # parameters have changed, change it accordingly

        self.nwk_key_disabled = True

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

    def get_module_version_rsp(self, rsp, rsp_len):
        self.host.ncp_req_config_parameter(self.cp_id)

    def get_cfg_prms_rsp(self, rsp, rsp_len):
        sts_ret = as_enum(status_id(rsp.hdr.status_category, rsp.hdr.status_code), NCP_HL_STATUS)
        cfg_prm = as_enum(self.cp_id, NCP_HL_CONFIG_PARAMETER)

        rsp_value = "none"
        if rsp.hdr.status_code == NCP_HL_STATUS.RET_OK:
            prm = rsp.body.arr[1:]
            if self.cp_id < 19 or self.cp_id == 23:
                rsp_value = prm[0]
            elif self.cp_id == 19:
                answ = rsp_s.from_buffer_copy(bytes(prm))
                rsp_value = answ.u16_1
            elif self.cp_id == 20 or self.cp_id == 21:
                answ = rsp_s_s.from_buffer_copy(bytes(prm))
                rsp_value = "{}/{} msec".format(answ.u16_1, answ.u16_2)
            elif self.cp_id == 22:
                answ = rsp_c_c.from_buffer_copy(bytes(prm))
                rsp_value = "{}/{} dBm".format(answ.i8_1, answ.i8_2)
            elif self.cp_id == 24:
                answ = rsp_b_b_s.from_buffer_copy(bytes(prm))
                rsp_value = "mtorr {}, radius {}, disc_time {}".format(answ.u8_1, answ.u8_2, answ.u16_1)

        logger.info("cfg prm #{} ({}), return sts {}, body_len {}, value: {}".format(self.cp_id, cfg_prm, sts_ret, rsp_len-7, rsp_value))
        if self.cp_id < self.cp_id_last:
            self.cp_id = self.cp_id + 1
            sleep(0.2)
            self.host.ncp_req_config_parameter(self.cp_id)


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
