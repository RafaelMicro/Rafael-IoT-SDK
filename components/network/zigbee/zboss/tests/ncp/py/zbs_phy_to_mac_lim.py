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
# PURPOSE: Host-side test implements ZR part for test the diagnostic counter
# phy_to_mac_limit_reached.
# Standalone side ZC is zbs_test_desc_zc
#
# How to run the test:
# Need three devices: NCP, ZC and the "flood_maker"
# 1. Open the "flood_maker" by SmartRF Studio in IEEE.15.4 mode
#    1.1. In RF Parameters select 19 Channel
#    1.2. In Packet TX tab:
#       1.2.1. In the "Packet Count" field type 2000
#       1.2.2. Uncheck "Add Seq. Number"
#       1.2.3. Select the "Hex" radio button
#       1.2.4. In the text field type 00807a43500000ffcf00000022840000efcdab505050ffffff00
#              This is a beacon packet, the SmartRF Studio will calculate the checksum automatically.
#       1.2.5. Check the box "Advanced"
#       1.2.6. Set the "Packet Interval" field to 0
# 2. Start the ncp_trace tool.
# 3. Start the Zigbee sniffer.
# 4. Power on the ZC.
# 5. Run python3 zbs_phy_to_mac_lim.py
# 6. After the message appears "Packet NCP_HL_NWK_NLME_JOIN received" in python console
#    press the Start button in SmartRF Studio to flood the air with beacons
# 7. Wait while "ALL REQUIRED NCP CALLS CAPTURED" message appears in python console.
# 8. Analyze the zbs_phy_to_mac_lim.log, sniffer .pcap and ncp_trace .pcap
#    Pay attention to mac_rx_bcast and phy_to_mac_que_lim_reached counters in the .log file,
#    as well as the number and rate of beacons arriving in .pcap files
#

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
from threading import Timer

logger = logging.getLogger(__name__)
log_file_name = "zbs_phy_to_mac_lim.log"

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
        })

        self.update_indication_switch({
        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_GET_STATS,              NCP_HL_STATUS.RET_OK],
                                 ]

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)
        self.auto_poll = None
        self.nwk_key_disabled = True
        self.t = Timer(20.0, self.get_stats_by_timer)

    def begin(self):   
        self.host.ncp_req_nwk_discovery(0, self.channel_mask, 5)
        self.t.start()

    def get_stats_by_timer(self):
        self.t.cancel()
        logger.info("call ncp_req_zdo_get_stats()")
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
