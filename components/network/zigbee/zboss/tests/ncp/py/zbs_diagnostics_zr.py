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
log_file_name = "zbs_diagnostics_zr.log"

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
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_GET_STATS,              NCP_HL_STATUS.RET_OK],
                                 ]

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)
        self.l_dataind = None
        self.apsde_data_req_max = 34567 # any large number
        self.ncp_load_stop_time = 0
        self.send_bcast_counts = 0
        self.tmr_no_data = Timer(6, self.data_flow_paused)
        self.no_memory_error = False
        self.was_flood_stage = False
        self.short_addr = ncp_hl_nwk_addr_t()
        self.big_pkts_req = 0


    def begin(self):   
        self.host.ncp_req_secur_set_ic(bytes([0x83, 0xFE, 0xD3, 0x40, 0x7A, 0x93, 0x97, 0x23,
                                              0xA5, 0xC6, 0x39, 0xB2, 0x69, 0x16, 0xD5, 0x05,
                                              0xC3, 0xB5]))

    def start_nwk_disc(self, rsp, rsp_len):
        self.host.ncp_req_nwk_discovery(0, self.channel_mask, 5)

    def flood_from_air(self):
        self.tmr_flood.cancel()
        x = 0
        while x < 50:
            if self.host.run_ll_quant(None, 0) != -4:
                self.send_data(True, False, False) # broadcast = True, is_big = False, is_route = False
                x += 1

    def send_data_back_delayed(self):
        self.tmr_data_back.cancel()
        self.tmr_no_data.cancel()
        self.tmr_no_data = Timer(6, self.data_flow_paused)
        self.tmr_no_data.start()
        if self.l_dataind is not None:
            self.send_packet_back(self.l_dataind, False)
            self.l_dataind = None

    def data_flow_paused(self):
        self.tmr_no_data.cancel()
        if self.no_memory_error == False:
            # send pkt to undefined address with NCP_HL_TX_OPT_FORCE_MESH_ROUTE_DISC option
            self.send_data(False, False, True) # broadcast = False, is_big = False, is_route = True
            self.send_big_pkt()

    def send_big_pkt(self):
        if self.no_memory_error == True or self.big_pkts_req > 5:
            pass
        else:
            self.tmr_big_data_loop = Timer(0.3, self.send_big_pkt)
            self.tmr_big_data_loop.start()
            while True:
                if self.host.run_ll_quant(None, 0) != -4:
                    self.send_data(False, True, False) # broadcast = False, is_big = True, is_route = False
                    self.big_pkts_req += 1
                    break

    def on_data_ind(self, dataind):
        self.tmr_no_data.cancel()
        # special data packet inform the test about beacons flood from ZC begin
        if dataind.data_len == 7 and dataind.data[0] == 0xAA and dataind.data[1] == 0xAA and dataind.data[3] == 0xBB and dataind.data[4] == 0xBB:
            self.tmr_flood = Timer(2, self.flood_from_air)
            self.tmr_flood.start()
            self.was_flood_stage = True
        else:
            if dataind.data_len > 2 and self.l_dataind is None:
                self.l_dataind = ncp_hl_data_ind_t.from_buffer_copy(dataind)
                # schedule sending the data back in one second
                self.tmr_data_back = Timer(1.0, self.send_data_back_delayed)
                self.tmr_data_back.start()

    def send_data(self, is_bcast, is_big, is_route):
        if is_bcast:
            is_big = False
            is_route = False
        params = ncp_hl_data_req_param_t()
        params.addr_mode = ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_16_ENDP_PRESENT
        if is_route:
            params.dst_addr.short_addr = self.short_addr.u16 + 1 # Non-existent address in network (our + 1)
        else:
            params.dst_addr.short_addr = 0xffff if is_bcast else 0
        params.dst_endpoint = 1
        params.profileid = 2
        params.clusterid = 0
        params.src_endpoint = 1
        params.tx_options = 0 if is_bcast else (ncp_hl_tx_options_e.NCP_HL_TX_OPT_SECURITY_ENABLED | ncp_hl_tx_options_e.NCP_HL_TX_OPT_ACK_TX | ncp_hl_tx_options_e.NCP_HL_TX_OPT_FRAG_PERMITTED)
        if is_route:
            params.tx_options |= ncp_hl_tx_options_e.NCP_HL_TX_OPT_FORCE_MESH_ROUTE_DISC
        params.use_alias = 0
        params.alias_src_addr = 0
        params.alias_seq_num = 0
        params.radius = 5
        data_size = 1500 if is_big else 10
        data = [1] * data_size
        self.host.ncp_req_apsde_data_request(params, ncp_hl_apsde_data_t(*data),  data_size)

    def l_data_conf(self, rsp, rsp_len):
        if rsp_len <= sizeof(rsp.hdr):
            self.rsp_log(rsp)
        else:
            self.rsp_log(rsp, ncp_hl_apsde_data_conf_t)
        # Change channel on ZC should result status NO_ACK. Can get statistics.
        sts = status_id(rsp.hdr.status_category, rsp.hdr.status_code)
        if sts == NCP_HL_STATUS.RET_NO_MEMORY:
            self.no_memory_error = True
        elif (sts == NCP_HL_STATUS.MAC_NO_ACK or sts == NCP_HL_STATUS.ZB_APS_STATUS_NO_ACK) and self.was_flood_stage:
            self.host.ncp_req_zdo_get_stats(do_cleanup = True)
            self.host.ncp_req_zdo_get_stats(do_cleanup = False)

    def on_nwk_join_complete(self, body):
        self.short_addr = body.short_addr
        self.host.ncp_req_zdo_permit_joining(body.short_addr, 0xfe, 0)

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
