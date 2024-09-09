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
# PURPOSE: Sample with mgmt_permit_joining in ZR role.
#

# ZR sends Mgmt_permit_joining request to ZC.

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
import time

logger = logging.getLogger(__name__)

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY: self.nwk_discovery_complete,
            ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN: self.nwk_join_complete,
            ncp_hl_call_code_e.NCP_HL_ZDO_PERMIT_JOINING_REQ: self.zdo_permit_joining_rsp
        })

        self.update_indication_switch({
            ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND: self.data_ind,
        })

        self.required_packets = [ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,
                                 ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,
                                 ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,
                                 ncp_hl_call_code_e.NCP_HL_ZDO_PERMIT_JOINING_REQ
                                 ]

    def begin(self):   
       self.host.ncp_req_nwk_discovery(0, self.channel_mask, 5)

    def on_nwk_join_complete(self, body):
        time.sleep(1)
        self.host.ncp_req_zdo_permit_joining(ncp_hl_nwk_addr_t(0), 0)

    def zdo_permit_joining_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

    def data_ind(self, ind, ind_len):
        dataind = ncp_hl_data_ind_t.from_buffer(ind.body, 0)
        logger.info(
            "apsde.data.ind len {} fc {:#x} src {:#x}/{:#x} dst {:#x}/{:#x} group {:#x} src ep {} dst ep {} cluster {:#x} profile {:#x} apscnt {} lqi {} rssi {} keyfl {:#x}".format(
                dataind.data_len, dataind.fc, dataind.src_addr, dataind.mac_src_addr, dataind.dst_addr,
                dataind.mac_dst_addr, dataind.group_addr, dataind.src_endpoint, dataind.dst_endpoint,
                dataind.clusterid, dataind.profileid, dataind.aps_counter, dataind.lqi, dataind.rssi,
                dataind.key_flags))
        data4dump = list(dataind.data)[:(dataind.data_len)]
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


def main():
    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)
    fh = logging.FileHandler("zbs_permit_joining_zr.log")
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
