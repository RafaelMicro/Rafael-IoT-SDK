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
# PURPOSE: Host-side test implements ZED role and exchanges packets with a parent device.
#
# zbs_echo test in ZED role, Host side. Use it with zbs_echo_zc.hex monolithic FW for CC1352.

# End device is configured with disabled RX_ON_WHEN_IDLE and next poll sequence:
# 1. ED sets long poll interval to 6 sec and fast poll interval to 1 sec.
# 2. ED starts to poll with long interval.
# 3. After receiving 5 packets, ED enables fast poll. Note, that default fast poll timeout is set to 10 sec.
# 4. After receiving 5 packets, ED disables fast polling, so long poll is automatically enabled.
# 5. After receiving 5 packets, ED stops polling for 10 sec.
# 6. ED goes to step 2.

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
from time import sleep

logger = logging.getLogger(__name__)

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19

LONG_POLL_INTERVAL = 24 # 24 = 6seconds * 4 quarterseconds
FAST_POLL_INTERVAL = 4  # 4 = 1second * 4 quarterseconds
LONG_POLL_PKT_N    = 5  # number of packets for receiving in long polling to change state
FAST_POLL_PKT_N    = 5  # number of packets for receiving in fast polling to change state
STOP_POLL_TIME     = 10 # on that time all polling will be stopped


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZED)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE: self.set_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_RX_ON_WHEN_IDLE: self.get_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_SET_LONG_POLL_INTERVAL: self.pim_set_long_interval_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_SET_FAST_POLL_INTERVAL: self.pim_set_fast_interval_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_START_FAST_POLL: self.pim_start_fast_poll_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_STOP_FAST_POLL: self.pim_stop_fast_poll_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_START_POLL: self.pim_start_poll_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_STOP_POLL: self.pim_stop_poll_rsp,
        })

        self.required_packets = [ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,
                                 ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE,
                                 ncp_hl_call_code_e.NCP_HL_GET_RX_ON_WHEN_IDLE,
                                 ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,
                                 ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,
                                 ncp_hl_call_code_e.NCP_HL_PIM_SET_LONG_POLL_INTERVAL,
                                 ncp_hl_call_code_e.NCP_HL_PIM_SET_FAST_POLL_INTERVAL,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ncp_hl_call_code_e.NCP_HL_PIM_START_FAST_POLL,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ncp_hl_call_code_e.NCP_HL_PIM_STOP_FAST_POLL, 
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,
                                 ncp_hl_call_code_e.NCP_HL_PIM_STOP_POLL,
                                 ncp_hl_call_code_e.NCP_HL_PIM_START_POLL,
                                 ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,
                                 ]

        self.update_indication_switch({
        })
        self.send_pack = 0
        self.apsde_data_req_max = 15

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

    def pim_start_fast_poll_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

    def pim_stop_fast_poll_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

    def pim_start_poll_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

    def pim_stop_poll_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        sleep(STOP_POLL_TIME)
        self.send_pack = 0
        self.host.ncp_req_pim_start_poll()

    def on_data_ind(self, dataind):
        self.send_packet_back(dataind)
    
    def on_data_conf(self, conf):
        self.send_pack += 1
        if self.send_pack == LONG_POLL_PKT_N:
            self.host.ncp_req_pim_start_fast_poll()
        elif self.send_pack == LONG_POLL_PKT_N + FAST_POLL_PKT_N:
            self.host.ncp_req_pim_stop_fast_poll()
        elif self.send_pack == LONG_POLL_PKT_N + FAST_POLL_PKT_N + LONG_POLL_PKT_N:
            self.host.ncp_req_pim_stop_poll()


def main():
    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)
    fh = logging.FileHandler("zbs_echo_zed.log")
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
