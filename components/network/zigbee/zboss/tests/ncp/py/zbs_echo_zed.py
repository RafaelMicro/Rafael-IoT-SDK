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
# PURPOSE: Host-side test implements ZED role and exchanges packets with a parent device.
#
# zbs_echo test in ZED role, Host side. Use it with zbs_echo_zc.hex monolithic FW for CC1352.

# End device is configured with disabled RX_ON_WHEN_IDLE and next poll sequence:
# 1. ED enables auto poll and turbo poll
# 2. ED sets long poll interval to 1 sec and fast poll interval to 0.5 sec.
# 3. ED starts to poll with long interval and turbo poll enabled
# 4. After receiving 3 packets, ED keeps long poll, disabled turbo poll
# 5. After receiving 3 packets, ED enables fast poll.
# 6. After receiving 3 packets, ED disables fast polling, so long poll is automatically enabled.
# 7. After receiving 3 packets, ED stops polling for 10 sec.
# 8. ED polls once manually
# 9. ED enables automatic poll

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
from time import sleep

logger = logging.getLogger(__name__)
log_file_name = "zbs_echo_zed.log"

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19

LONG_POLL_INTERVAL = 4 # 4 = 1second * 4 quarterseconds
FAST_POLL_INTERVAL = 2  # 2 = 0.5second * 2 quarterseconds
LONG_TURBO_POLL_PKT_N   = 3  # number of packets for receiving in long + turbo polling to change state
LONG_POLL_PKT_N    = 6  # number of packets for receiving in long polling to change state
FAST_POLL_PKT_N    = 9  # number of packets for receiving in fast polling to change state
STOP_POLL_PKT_N    = 12 # number of packets before stopping auto poll
STOP_POLL_TIME     = 10 # on that time all polling will be stopped


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZED)
        self.just_started = True
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE: self.set_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_RX_ON_WHEN_IDLE: self.get_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_SET_LONG_POLL_INTERVAL: self.pim_set_long_interval_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_SET_FAST_POLL_INTERVAL: self.pim_set_fast_interval_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_ENABLE_TURBO_POLL: self.pim_enable_turbo_poll_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_DISABLE_TURBO_POLL: self.pim_disable_turbo_poll_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_START_FAST_POLL: self.pim_start_fast_poll_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_STOP_FAST_POLL: self.pim_stop_fast_poll_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_START_POLL: self.pim_start_poll_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_STOP_POLL: self.pim_stop_poll_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_SINGLE_POLL: self.pim_single_poll_rsp,
        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,                      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL_MASK,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_PAN_ID,                     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_ROLE,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_START_POLL,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_RX_ON_WHEN_IDLE,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_SET_LONG_POLL_INTERVAL,     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_SET_FAST_POLL_INTERVAL,     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_ENABLE_TURBO_POLL,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_DISABLE_TURBO_POLL,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_START_FAST_POLL,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_STOP_FAST_POLL,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_STOP_POLL,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_SINGLE_POLL,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_START_POLL,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,                 NCP_HL_STATUS.RET_OK],
                                 ]

        self.update_indication_switch({
        })
        self.send_pack = 0
        self.apsde_data_req_max = 15

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

    def begin(self):
        self.host.ncp_req_set_rx_on_when_idle(ncp_hl_on_off_e.OFF)

    def set_rx_on_when_idle_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_get_rx_on_when_idle()

    def get_rx_on_when_idle_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            val = ncp_hl_uint8_t.from_buffer_copy(rsp.body, 0).uint8
            strval = "OFF" if val == ncp_hl_on_off_e.OFF else "ON"
            logger.info("rx_on_when_idle: %s", strval)
        self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)

    def on_nwk_join_complete(self, body):
        self.host.ncp_req_pim_set_long_poll_interval(LONG_POLL_INTERVAL)

    def pim_set_long_interval_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_pim_set_fast_poll_interval(FAST_POLL_INTERVAL)

    def pim_set_fast_interval_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        # enale continuous turbo poll for short time, then keep it enabled
        self.host.ncp_req_pim_enable_turbo_poll(1)

    def pim_enable_turbo_poll_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

    def pim_disable_turbo_poll_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

    def pim_start_fast_poll_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

    def pim_stop_fast_poll_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

    def pim_start_poll_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if self.just_started :
            self.just_started = None
            self.begin()

    def pim_stop_poll_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        sleep(STOP_POLL_TIME)
        logger.info("Single manual poll")
        self.host.ncp_req_pim_single_poll()

    def pim_single_poll_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        logger.info("Start automatic poll")
        self.host.ncp_req_pim_start_poll()

    def on_data_ind(self, dataind):
        if len(self.host.required_packets) > 0:
            self.send_packet_back(dataind, False)

    def on_data_conf(self, conf):
        self.send_pack += 1
        if self.send_pack == LONG_TURBO_POLL_PKT_N:
            logger.info("Disabling turbo poll")
            self.host.ncp_req_pim_disable_turbo_poll()
        elif self.send_pack == LONG_POLL_PKT_N:
            logger.info("Starting fast poll")
            self.host.ncp_req_pim_start_fast_poll()
        elif self.send_pack == FAST_POLL_PKT_N:
            logger.info("Back to long poll")
            self.host.ncp_req_pim_stop_fast_poll()
        elif self.send_pack == STOP_POLL_PKT_N:
            logger.info("Stop automatic poll")
            self.host.ncp_req_pim_stop_poll()


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
