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
# PURPOSE: Host-side test implements sample of production configuration usage.
#
# Reads MAC, IC, cert and manufacture specific part from production configuration of NCP.
# Also set/get/del ICs to whitelist of devices.

#import sys
#import threading
#import platform
#from enum import IntEnum
#from ctypes import *
#import logging

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
from time import sleep

logger = logging.getLogger(__name__)
log_file_name = "zbs_prod_cfg_sample_zr_cs1.log"

dev1_installcode = bytes([0xBB, 0xCC, 0xAA, 0xDD, 0x11, 0x22, 0x33, 0x44,
                          0x08, 0x29])

dev2_installcode = bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
                          0x4F, 0xD9])

dev1_ieee = bytes([0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb])
dev2_ieee = bytes([0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa])

CHANNEL_MASK = 0x80000  # channel 19

#these issuers are from Specification CryptoSuites
issuer_cs1 = esi_certificate_cs1[30:38]
issuer_cs2 = esi_certificate_cs2[11:19]

use_cs = 1

#reset options
erase_nvram_on_start = not TestRunner().ncp_on_nsng()

class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION : self.get_module_version_rsp,
            ncp_hl_call_code_e.NCP_HL_NCP_RESET : self.ncp_reset_resp,
            ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR : self.get_local_ieee_addr_rsp,
            ncp_hl_call_code_e.NCP_HL_SECUR_GET_LOCAL_IC : self.secur_get_local_ic_rsp,
            ncp_hl_call_code_e.NCP_HL_SECUR_GET_CERT : self.secur_get_cert_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_SERIAL_NUMBER : self.get_serial_number_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_VENDOR_DATA : self.get_vendor_data_rsp,
        })

        self.update_indication_switch({

        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_GET_CERT,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_GET_LOCAL_IC,     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_SERIAL_NUMBER,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_VENDOR_DATA,        NCP_HL_STATUS.RET_OK],
                                 ]

        self.nwk_key_disabled = True

        self.reset_state = ncp_hl_rst_state_e.TURNED_ON
        self.state = 0

        self.local_addr = ncp_hl_local_addr_t()
        self.ieee_addr = ncp_hl_ieee_addr_t()

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

    def rsp_delayed_log(self, rsp, cls = None, frame = None):
        self.rsp_log(rsp, cls, frame)
        sleep(0.5)

        # Is it PC with a simulator build or RPi with NCP connected over SPI?
    def get_ll_so_name(self):
        a = platform.machine()
        if a == "x86_64":
            return "../ncp_ll_nsng.so"
        else:
            return "./zbs_ncp_rpi.so"

    def ncp_reset_resp(self, rsp, rsp_len):
        self.rsp_delayed_log(rsp)
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
        self.rsp_delayed_log(rsp, ncp_hl_module_ver_t)
        if use_cs == 1:
            self.issuer = issuer_cs1
        else:
            self.issuer = issuer_cs2
        self.host.ncp_req_get_local_ieee_addr(0)

    def get_local_ieee_addr_rsp(self, rsp, rsp_len):
        self.local_addr = self.rsp_rx(rsp, ncp_hl_local_addr_t)
        self.ieee_addr = ncp_hl_ieee_addr_t(self.local_addr.long_addr)
        self.host.ncp_req_secur_get_cert(use_cs, self.issuer, self.ieee_addr)


    def secur_get_cert_rsp(self, rsp, rsp_len):
        self.rsp_delayed_log(rsp)
        if rsp.hdr.status_code == 0:
            cs_suite_no = ncp_hl_uint8_t.from_buffer_copy(rsp.body, 0)
            if cs_suite_no.uint8 == 1:
                suite = ncp_hl_secur_add_cert_cs1_t.from_buffer_copy(rsp.body, 0)
            elif cs_suite_no.uint8 == 2:
                suite = ncp_hl_secur_add_cert_cs2_t.from_buffer_copy(rsp.body, 0)
            else:
                logger.warn("UNKNOWN certificate suite number %d", cs_suite_no.uint8)
            if suite:
                logger.info("CS {}".format(suite.cs_type))
                logger.info("ca_public_key {}".format(list(map(lambda x: hex(x), suite.ca_public_key))))
                logger.info("certificate {}".format(list(map(lambda x: hex(x), suite.certificate))))
                logger.info("private_key {}".format(list(map(lambda x: hex(x), suite.private_key))))
            self.host.ncp_req_secur_get_local_ic()

    def secur_get_local_ic_rsp(self, rsp, rsp_len):
        self.rsp_delayed_log(rsp, ncp_hl_ic_rsp_t)
        self.host.ncp_req_get_serial_number()

    def get_serial_number_rsp(self, rsp, rsp_len):
        self.rsp_delayed_log(rsp, ncp_hl_serial_number_t)
        self.host.ncp_req_get_vendor_data()

    def get_vendor_data_rsp(self, rsp, rsp_len):
        self.rsp_delayed_log(rsp)
        payload = ncp_hl_payload_t.from_buffer_copy(rsp.body, 0)
        logger.info(" vendor data: {}".format(ncp_hl_dump(payload.data[:payload.len])))

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
