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
# PURPOSE: Host-side test implements ZED role and exchanges packets with a parent
# device and different TX power.
#
# zbs_txpwr_echo_zed test in ZED role, Host side. Use it with zbs_echo_zc.hex monolithic FW for CC1352.

# End device is changed TX power every three pairs of echo packets with following sequence:
# -20, -10, 0, +5, +14, +20 and +2 dBm
# Change TX power will be seen, for example, over SmartRF Studio in Packet RX mode.
#

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
from time import sleep

logger = logging.getLogger(__name__)
log_file_name = "zbs_txpwr_echo_zed.log"

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19

LONG_POLL_INTERVAL = 4  # 4 = 1second * 4 quarterseconds
TX_PWR_CHG_N       = 3

class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZED)
        self.just_started = True
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE: self.set_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_SET_LONG_POLL_INTERVAL: self.pim_set_long_interval_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_START_POLL: self.pim_start_poll_rsp,
            ncp_hl_call_code_e.NCP_HL_SET_TX_POWER: self.set_tx_power_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_TX_POWER: self.get_tx_power_rsp,
        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,               NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,                   NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL_MASK,     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_PAN_ID,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_ROLE,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_START_POLL,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,               NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,               NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_SET_LONG_POLL_INTERVAL,  NCP_HL_STATUS.RET_OK],
                                 # -20 dBm
                                 [ncp_hl_call_code_e.NCP_HL_SET_TX_POWER,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_TX_POWER,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 # -10 dBm
                                 [ncp_hl_call_code_e.NCP_HL_SET_TX_POWER,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_TX_POWER,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 # 0 dBm
                                 [ncp_hl_call_code_e.NCP_HL_SET_TX_POWER,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_TX_POWER,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 # +5 dBm
                                 [ncp_hl_call_code_e.NCP_HL_SET_TX_POWER,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_TX_POWER,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 # +14 dBm
                                 [ncp_hl_call_code_e.NCP_HL_SET_TX_POWER,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_TX_POWER,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 # +20 dBm
                                 [ncp_hl_call_code_e.NCP_HL_SET_TX_POWER,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_TX_POWER,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 # +2 dBm
                                 [ncp_hl_call_code_e.NCP_HL_SET_TX_POWER,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_TX_POWER,                NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ,              NCP_HL_STATUS.RET_OK],
                                 ]

        self.update_indication_switch({
        })
        self.send_pack = 0
        self.apsde_data_req_max = 1500000
        self.desired_tx_power = -30 # invalid TX power

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

    def begin(self):
        self.host.ncp_req_set_rx_on_when_idle(ncp_hl_on_off_e.OFF)

    def set_rx_on_when_idle_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)
        # self.host.ncp_req_get_rx_on_when_idle()

    # def get_rx_on_when_idle_rsp(self, rsp, rsp_len):
    #     self.rsp_log(rsp)
    #     if rsp.hdr.status_code == 0:
    #         val = ncp_hl_uint8_t.from_buffer_copy(rsp.body, 0).uint8
    #         strval = "OFF" if val == ncp_hl_on_off_e.OFF else "ON"
    #         logger.info("rx_on_when_idle: %s", strval)
    #     self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)

    def on_nwk_join_complete(self, body):
        # self.set_tx_power()
        # self.host.ncp_req_pim_set_fast_poll_interval(FAST_POLL_INTERVAL)
        self.host.ncp_req_pim_set_long_poll_interval(LONG_POLL_INTERVAL)

    def pim_set_long_interval_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.set_tx_power()
        # self.host.ncp_req_pim_set_fast_poll_interval(FAST_POLL_INTERVAL)

    # def pim_enable_turbo_poll_rsp(self, rsp, rsp_len):
    #     self.rsp_log(rsp)
    #     self.set_tx_power()

    # def pim_set_fast_interval_rsp(self, rsp, rsp_len):
    #     self.rsp_log(rsp)
     #    self.host.ncp_req_pim_enable_turbo_poll(20000)
        # self.host.ncp_req_pim_start_fast_poll()

    # def start_fast_poll_rsp(self, rsp, rsp_len):
    #     self.rsp_log(rsp)
        # self.set_tx_power()
        
    def set_tx_power(self):
        if self.desired_tx_power < -20:
            self.desired_tx_power = -20
        elif self.desired_tx_power == -20:
            self.desired_tx_power = -10
        elif self.desired_tx_power == -10:
            self.desired_tx_power = 0
        elif self.desired_tx_power == 0:
            self.desired_tx_power = 5
        elif self.desired_tx_power == 5:
            self.desired_tx_power = 14
        elif self.desired_tx_power == 14:
            self.desired_tx_power = 20
        else:
            self.desired_tx_power = 2
        logger.info("set Tx power to %d dBm", self.desired_tx_power)
        self.host.ncp_req_set_tx_power(self.desired_tx_power)

    def set_tx_power_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            body = ncp_hl_int8_t.from_buffer_copy(rsp.body)
            logger.info("set TX power returns {} dBm".format(body.int8))
        # self.host.ncp_req_pim_enable_turbo_poll(2)
        self.host.ncp_req_get_tx_power()

    def get_tx_power_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            body = ncp_hl_int8_t.from_buffer_copy(rsp.body)
            logger.info("get TX power returns {} dBm".format(body.int8))

    def pim_start_poll_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if self.just_started :
            self.just_started = None
            self.begin()

    def on_data_ind(self, dataind):
        if len(self.host.required_packets) > 0:
            self.send_packet_back(dataind, False)

    def on_data_conf(self, conf):
        self.send_pack += 1
        if self.send_pack % TX_PWR_CHG_N == 0:
            self.set_tx_power()


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
