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
# PURPOSE: Host-side test implements ZR part of Echo test with diagnostics
# ZC - zbs_diagnostics_zc
#

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
import time
from threading import Timer

logger = logging.getLogger(__name__)
log_file_name = "zbs_diag_echo_zr.log"

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19

class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_SECUR_SET_LOCAL_IC: self.start_nwk_disc,
            ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ: self.l_data_conf,
            ncp_hl_call_code_e.NCP_HL_GET_LOCK_STATUS: self.l_get_lock_status_rsp,
        })

        self.update_indication_switch({
        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL_MASK,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_ROLE,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_SET_LOCAL_IC,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_GET_STATS,              NCP_HL_STATUS.RET_OK],
                                 ]

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)
        self.l_dataind = None
        self.apsde_data_req_max = 34567 # any large number
        self.ncp_load_stop_time = 0
        self.ncp_load_counts = 0


    def begin(self):   
        self.host.ncp_req_secur_set_ic(bytes([0x83, 0xFE, 0xD3, 0x40, 0x7A, 0x93, 0x97, 0x23,
                                              0xA5, 0xC6, 0x39, 0xB2, 0x69, 0x16, 0xD5, 0x05,
                                              0xC3, 0xB5]))

    def start_nwk_disc(self, rsp, rsp_len):
        self.host.ncp_req_nwk_discovery(0, self.channel_mask, 5)

    def l_get_lock_status_rsp(self, rsp, rsp_len):
        if self.ncp_load_stop_time == 0:
            pass
        elif time.time() >= self.ncp_load_stop_time:
            logging.disable(logging.NOTSET) # restore normal logging
            logger.info("NCP_load from Host calls {}".format(self.ncp_load_counts))
            self.ncp_load_stop_time = 0
            self.host.ncp_req_set_trace(0xff)
        else:
            # load the NCP from the Host side
            self.ncp_load_counts += 1
            self.host.ncp_get_lock_status()
        
    def send_data_back_delayed(self):
        self.t.cancel()
        if self.l_dataind is not None:
            self.send_packet_back(self.l_dataind, False)
            self.l_dataind = None

    def on_data_ind(self, dataind):
        if dataind.data_len == 7 and dataind.data[0] == 0xAA and dataind.data[1] == 0xAA and dataind.data[3] == 0xBB and dataind.data[4] == 0xBB:
            # requires NCP load from Host
            self.ncp_load_stop_time = time.time() + dataind.data[2] + 1
            logging.disable(logging.WARNING)
            self.host.ncp_req_set_trace(1)
            self.host.ncp_get_lock_status()
        else:
            if dataind.data_len > 2 and self.l_dataind is None:
                self.l_dataind = ncp_hl_data_ind_t.from_buffer_copy(dataind)
                # schedule sending the data back in one second
                self.t = Timer(1.0, self.send_data_back_delayed)
                self.t.start()

    def l_data_conf(self, rsp, rsp_len):
        if rsp_len <= sizeof(rsp.hdr):
            self.rsp_log(rsp)
        else:
            self.rsp_log(rsp, ncp_hl_apsde_data_conf_t)
        # Change channel on ZC should result status NO_ACK. Can get statistics.
        sts = status_id(rsp.hdr.status_category, rsp.hdr.status_code)
        if sts == NCP_HL_STATUS.MAC_NO_ACK or sts == NCP_HL_STATUS.ZB_APS_STATUS_NO_ACK:
            self.host.ncp_req_zdo_get_stats(do_cleanup = False)


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
