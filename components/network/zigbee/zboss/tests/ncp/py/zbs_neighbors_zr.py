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
# PURPOSE: Host-side test implements ZR part of Echo test.
#

from enum import Enum
from time import sleep
from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner

from time import sleep

logger = logging.getLogger(__name__)
log_file_name = "zbs_neighbors_zr.log"

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19

class TestStep(Enum):
    IDLE                      = 0
    GET_AND_CLEANUP_STATS_REQ = 1
    GET_AND_CLEANUP_STATS_RSP = 2
    DEV_ANNCE_IND             = 3
    GET_STATS_REQ             = 4
    GET_STATS_RSP             = 5
    FINISHED                  = 6

class Test(BaseTest):
    GET_STATS_REQ_DELAY_SEC = 20.0

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_SECUR_SET_LOCAL_IC: self.start_nwk_disc,
            ncp_hl_call_code_e.NCP_HL_ZDO_MGMT_LEAVE_REQ: self.zdo_mgmt_leave_rsp,
        })

        self.update_indication_switch({
            ncp_hl_call_code_e.NCP_HL_NWK_LEAVE_IND: self.nwk_leave_ind,
        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,           NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,               NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR,     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK, NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL_MASK, NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_ROLE,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_SET_LOCAL_IC,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,           NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,           NCP_HL_STATUS.RET_OK],

                                 # for GET_AND_CLEANUP_STATS_REQ
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_GET_STATS,           NCP_HL_STATUS.RET_OK],

                                 [ncp_hl_call_code_e.NCP_HL_ZDO_DEV_ANNCE_IND,       NCP_HL_STATUS.RET_OK],

                                 [ncp_hl_call_code_e.NCP_HL_ZDO_MGMT_LEAVE_REQ,      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_LEAVE_IND,           NCP_HL_STATUS.RET_OK],

                                 [ncp_hl_call_code_e.NCP_HL_ZDO_GET_STATS,           NCP_HL_STATUS.RET_OK],
                                ]

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

        self.zdo_get_stats_step = TestStep.IDLE
        self.child_short_addr = None
        self.child_ieee_addr = None
        self.test_finished = False

        self.dev_annce_counter = 0

    def begin(self):
        self.host.ncp_req_secur_set_ic(bytes([0x83, 0xFE, 0xD3, 0x40, 0x7A, 0x93, 0x97, 0x23,
                                              0xA5, 0xC6, 0x39, 0xB2, 0x69, 0x16, 0xD5, 0x05,
                                              0xC3, 0xB5]))

    def start_nwk_disc(self, rsp, rsp_len):
        logger.debug("start_nwk_disc()")
        self.host.ncp_req_nwk_discovery(0, self.channel_mask, 5)

    def on_nwk_join_complete(self, body):
        logger.debug("on_nwk_join_complete()")
        self.host.ncp_req_zdo_permit_joining(body.short_addr, 0xfe, 0)
        self.zdo_get_stats_step = TestStep.GET_AND_CLEANUP_STATS_REQ
        self.get_stats_state_machine()

    def on_dev_annce_ind(self, annce):
        self.dev_annce_counter += 1
        if self.dev_annce_counter == 1:
            self.child_short_addr = annce.short_addr
            self.child_ieee_addr = annce.long_addr
            self.zdo_get_stats_step = TestStep.DEV_ANNCE_IND
            self.get_stats_state_machine()
        elif self.dev_annce_counter == 2:
            self.zdo_get_stats_step = TestStep.GET_STATS_REQ
            self.get_stats_state_machine()

    def zdo_mgmt_leave_rsp(self, rsp, rsp_len):
        logger.debug("zdo_mgmt_leave_rsp(), rsp_len {}".format(rsp_len))

    def nwk_leave_ind(self, ind, ind_len):
        leave_ind = ncp_hl_nwk_leave_ind_t.from_buffer(ind.body, 0)
        logger.debug("nwk_leave_ind(), ind_len {}, ieee_addr {}, rejoin {}"
                     .format(ind_len, leave_ind.ieee_addr, leave_ind.rejoin))

    def on_zdo_get_stats_complete(self, stats):
        logger.debug("on_zdo_get_stats_complete()")
        if self.zdo_get_stats_step == TestStep.GET_STATS_RSP:
            self.stats = stats
            self.get_stats_state_machine()

    def get_stats_state_machine(self):
        if self.zdo_get_stats_step == TestStep.GET_AND_CLEANUP_STATS_REQ:
            logger.debug("TestStep.GET_AND_CLEANUP_STATS_REQ")
            self.zdo_get_stats_step = TestStep.GET_AND_CLEANUP_STATS_RSP

            self.host.ncp_req_zdo_get_stats(do_cleanup = True)

        elif self.zdo_get_stats_step == TestStep.GET_AND_CLEANUP_STATS_RSP:
            logger.debug("TestStep.GET_AND_CLEANUP_STATS_RSP, waiting for DevAnnce")
            self.zdo_get_stats_step = TestStep.IDLE

        elif self.zdo_get_stats_step == TestStep.DEV_ANNCE_IND:
            logger.debug("TestStep.DEV_ANNCE_IND, waiting for ZE leaving and rejoining")
            self.zdo_get_stats_step = TestStep.IDLE

            logger.debug("sleep for 5 seconds to the end of TCLK...")
            sleep(5)

            self.host.ncp_req_zdo_mgmt_leave(
                self.child_short_addr,
                self.child_ieee_addr,
                NCP_HL_LEAVE_FLAGS.REJOIN)

        elif self.zdo_get_stats_step == TestStep.GET_STATS_REQ:
            logger.debug("TestStep.GET_STATS_REQ")
            self.zdo_get_stats_step = TestStep.GET_STATS_RSP

            self.host.ncp_req_zdo_get_stats(
                do_cleanup = False,
                delay = self.GET_STATS_REQ_DELAY_SEC)

        elif self.zdo_get_stats_step == TestStep.GET_STATS_RSP:
            logger.debug("TestStep.GET_STATS_RSP")
            self.zdo_get_stats_step = TestStep.FINISHED

            if self.stats.nwk_neighbor_added == 0 \
               or self.stats.nwk_neighbor_removed == 0 \
               or self.stats.nwk_neighbor_stale == 0 \
               or self.stats.childs_removed == 0:
                logger.error("Test is FAILED! One of counters is zero!")
            else:
                logger.debug("Test is PASSED!")

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

        # Ignore failure statuses. Test procedure will handle errors by itself.
        test.host.ignore_unexpected_status = True

        test.run()
    except KeyboardInterrupt:
        return
    finally:
        logger.removeHandler(fh)


if __name__ == "__main__":
    main()
