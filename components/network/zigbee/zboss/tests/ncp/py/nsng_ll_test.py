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
# PURPOSE: Trivial Host-side test for nsng shared library.
#
# start nsng, load .so, wait for NCP process connect, send one request, read resp

import sys
import threading
from enum import IntEnum
from ctypes import *
import logging
from ncp_hl.ncp_host_hl import *

logger = logging.getLogger(__name__)

#
# Test itself
#

CHANNEL_MASK = 0x800            # channel 11
MY_IEEE_ADDR = bytes([0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb])

class zdo_startup_zc_test:

    def __init__(self):
        self.rsp_switch = {
            ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION : self.get_module_version_rsp,
            ncp_hl_call_code_e.NCP_HL_SET_LOCAL_IEEE_ADDR : self.set_long_addr_done,
            ncp_hl_call_code_e.NCP_HL_NCP_RESET : self.ncp_reset_resp,
            ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK : self.channel_mask_set,
            ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE : self.zigbee_role_set,
            ncp_hl_call_code_e.NCP_HL_NWK_FORMATION : self.formation_complete,
            ncp_hl_call_code_e.NCP_HL_NWK_PERMIT_JOINING : self.nwk_permit_joining_complete,
            ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ :self.data_conf
        }

        self.ind_switch = {
            ncp_hl_call_code_e.NCP_HL_ZDO_DEV_ANNCE_IND : self.dev_annce_ind,
            ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND : self.data_ind,
        }

    def run(self):
        self.host = ncp_host("../ncp_ll_nsng.so", self.rsp_switch, self.ind_switch, required_packets=[], ignore_apsde_data=True)
        self.host.ncp_req_get_module_version()
        # main loop
        while True:
            self.host.wait_for_ncp()
            self.host.run_ll_quant(None, 0)
        pass
            
    def get_module_version_rsp(self, rsp, rsp_len):
        body_ver = ncp_hl_module_ver_t.from_buffer(rsp.body)
        logger.info("NCP ver 0x%x, protocol ver 0x%02x, stack ver 0x%02x, other ver 0x%04x",
                   body_ver.u32, body_ver.ver_proto, body_ver.ver_stack, body_ver.ver_other)
        self.host.ncp_req_set_local_ieee_addr(MY_IEEE_ADDR)

    def set_long_addr_done(self, rsp, rsp_len):
        logger.info("set_long_addr_done status %d", rsp.hdr.status_code)
        self.host.ncp_req_set_zigbee_channel_mask(0, CHANNEL_MASK)
        
    def ncp_reset_resp(self, rsp, rsp_len):
        logger.info("ncp_reset_resp status %d", rsp.hdr.status_code)

    def channel_mask_set(self, rsp, rsp_len):
        logger.info("channel mask set. status %d", rsp.hdr.status_code)
        self.host.ncp_req_set_zigbee_role(ncp_hl_role_e.NCP_HL_ZC)

    def zigbee_role_set(self, rsp, rsp_len):
        logger.info("zb role set. status %d", rsp.hdr.status_code)
        self.host.ncp_req_nwk_formation(0, CHANNEL_MASK, 5, 0, 0)

    def formation_complete(self, rsp, rsp_len):
        logger.info("Formation complete. status %d", rsp.hdr.status_code)
        self.host.ncp_req_nwk_permit_joining(0xfe)

    def nwk_permit_joining_complete(self, rsp, rsp_len):
        logger.info("nwk_permit_joining_complete status %d", rsp.hdr.status_code)

    def dev_annce_ind(self, ind, ind_len):
        annce = ncp_hl_dev_annce_t.from_buffer(ind.body, 0)
        logger.info("device annce: dev {:#x} ieee {} cap {:#x}".format(annce.short_addr, list(map(lambda x: hex(x), list(annce.long_addr))), annce.capability))

    def data_ind(self, ind, ind_len):
        dataind = ncp_hl_data_ind_t.from_buffer(ind.body, 0)
        logger.info("apsde.data.ind len {} fc {:#x} src {:#x}/{:#x} dst {:#x}/{:#x} group {:#x} src ep {} dst ep {} cluster {:#x} profile {:#x} apscnt {} lqi {} rssi {} keyfl {:#x}".format(
            dataind.data_len, dataind.fc, dataind.src_addr, dataind.mac_src_addr, dataind.dst_addr, dataind.mac_dst_addr, dataind.group_addr, dataind.src_endpoint, dataind.dst_endpoint,
            dataind.clusterid, dataind.profileid, dataind.aps_counter, dataind.lqi, dataind.rssi, dataind.key_flags))
        data4dump = list(dataind.data)[:(dataind.data_len-1)]
        logger.info("data {}".format(list(map(lambda x: hex(x), data4dump))))
        # Send packet back
        params = ncp_hl_data_req_param_t()
        params.dst_addr.short_addr = dataind.src_addr
        params.addr_mode = ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_16_ENDP_PRESENT
        params.profileid = dataind.profileid
        params.clusterid = dataind.clusterid
        params.dst_endpoint = dataind.src_endpoint
        params.src_endpoint = dataind.dst_endpoint
        params.radius = 30
        params.tx_options = ncp_hl_tx_options_e.NCP_HL_TX_OPT_SECURITY_ENABLED | ncp_hl_tx_options_e.NCP_HL_TX_OPT_ACK_TX
        params.use_alias = 0
        params.alias_src_addr = 0
        params.alias_seq_num = 0
        self.host.ncp_req_apsde_data_request(params, dataind.data, dataind.data_len)

    def data_conf(self, rsp, rsp_len):
        if rsp_len <= sizeof(rsp.hdr):
            logger.info("apsde-data.conf status {} {}".format(rsp.hdr.status_category, rsp.hdr.status_code))
        else:
            conf = ncp_hl_apsde_data_conf_t.from_buffer(rsp.body.arr, 0)
            # TODO: long/short addr
            logger.info("apsde-data.conf status {} addr_mode {} dst_addr {} src_endpoint {} dst_endpoint {} tx_time {} ".format(
                        conf.status, conf.addr_mode,
                        conf.dst_addr.short_addr if conf.addr_mode != ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT else list(map(lambda x: hex(x), list(conf.dst_addr.long_addr))),
                        conf.src_endpoint, conf.dst_endpoint, conf.tx_time))
        
def main():

    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)

    test = zdo_startup_zc_test()
    test.run()
    pass    


if __name__ == "__main__":
    main()
