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
# Made OTA upgrade of NCP.

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
from time import sleep

logger = logging.getLogger(__name__)
log_file_name = "zbs_ota_upgrade.log"

class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION : self.get_module_version_rsp,
            ncp_hl_call_code_e.NCP_HL_NCP_RESET : self.ncp_reset_resp,
            ncp_hl_call_code_e.NCP_HL_NCP_RESET : self.ncp_reset_resp,
            ncp_hl_call_code_e.NCP_HL_OTA_SEND_PORTION_FW: self.ota_send_portion_fw_rsp
        })

        self.update_indication_switch({
            ncp_hl_call_code_e.NCP_HL_OTA_START_UPGRADE_IND : self.ota_start_upgrade_ind
        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_OTA_START_UPGRADE_IND,  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_OTA_SEND_PORTION_FW,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,     NCP_HL_STATUS.RET_OK],
                                ]

        self.nwk_key_disabled = True

        self.reset_state = ncp_hl_rst_state_e.TURNED_ON
        self.ota_upgrade_step = ncp_hl_ota_upgrade_e.IDLE
        self.state = 0

        self.image_file = open('zbs_ncp.zigbee', 'rb')
        self.image_progress = 0

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

    def ncp_reset_resp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_get_module_version()

    def get_module_version_rsp(self, rsp, rsp_len):
        ver = self.rsp_rx(rsp, ncp_hl_module_ver_t)
        if (ver.ver_fw == 0x03020110 and
            self.ota_upgrade_step == ncp_hl_ota_upgrade_e.IDLE):
            self.ota_upgrade_step = ncp_hl_ota_upgrade_e.RUN_BOOTLOADER
            sleep(1)
            self.host.ncp_req_ota_run_bootloader()

    def ota_start_upgrade_ind(self, ind, ind_len):
        self.ind_log(ind, None)
        self.ota_upgrade_step = ncp_hl_ota_upgrade_e.BOOTLOADER_IS_RUN
        self.image_file.seek(0)
        self.image_progress = 0
        self.send_fw()

    def ota_send_portion_fw_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            self.send_fw()

    def send_fw(self):
        size = 1500
        try:
            payload = self.image_file.read(size)
            self.image_progress += len(payload)
            if payload:
                # logger.info('Sending: {}'.format(payload))
                self.host.ncp_req_ota_send_portion_fw(payload, len(payload))
                logger.info('Sent: {} bytes'.format(self.image_progress))
            else:
                logger.info('Image file ended.')
                #self.image_file.close()
        except Exception as e:
            logger.info(e)


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
        CHANNEL_MASK = 0x80000
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
