# /***************************************************************************
# *               ZBOSS Zigbee 3.0                                           *
# *                                                                          *
# *  This is unpublished proprietary source code of DSR Corporation          *
# *  The copyright notice does not evidence any actual or intended           *
# *  publication of such source code.                                        *
# *                                                                          *
# *          Copyright (c) 2012-2019 DSR Corporation, Denver CO, USA.        *
# *                       http://www.dsr-zboss.com                           *
# *                       http://www.dsr-corporation.com                     *
# *                                                                          *
# *                            All rights reserved.                          *
# *                                                                          *
# *                                                                          *
# * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR   *
# * Corporation                                                              *
# *                                                                          *
# ****************************************************************************
# PURPOSE: Host-side test implements ZC part of Key Establishment.
#
# SE join and key establishment - ZC role.

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner

logger = logging.getLogger(__name__)

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19
PAN_ID = 0x7777  # Arbitrary selected
INSTALL_CODE = bytes([0x96, 0x6b, 0x9f, 0x3e, 0xf9, 0x8a, 0xe6, 0x05, 0x97, 0x08])


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZC, pan_id=PAN_ID)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION: self.get_module_version_rsp_se,
            ncp_hl_call_code_e.NCP_HL_NWK_PERMIT_JOINING: self.nwk_permit_joining_complete,
            ncp_hl_call_code_e.NCP_HL_SECUR_ADD_IC: self.secur_add_ic_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ: self.zdo_node_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC: self.af_simple_desc_set,

        })

        self.update_indication_switch({
            ncp_hl_call_code_e.NCP_HL_SECUR_CHILD_KE_FINISHED_IND: self.child_ke_finished_ind,
        })

        self.required_packets = [ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,
                                 ncp_hl_call_code_e.NCP_HL_SET_PAN_ID,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,
                                 ncp_hl_call_code_e.NCP_HL_SECUR_ADD_IC,
                                 ncp_hl_call_code_e.NCP_HL_NWK_FORMATION,
                                 ncp_hl_call_code_e.NCP_HL_NWK_PERMIT_JOINING,
                                 ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,
                                 ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,
                                 ncp_hl_call_code_e.NCP_HL_ZDO_DEV_ANNCE_IND,
                                 ncp_hl_call_code_e.NCP_HL_NWK_GET_IEEE_BY_SHORT,
                                 ncp_hl_call_code_e.NCP_HL_NWK_GET_NEIGHBOR_BY_IEEE,
                                 ncp_hl_call_code_e.NCP_HL_NWK_GET_SHORT_BY_IEEE,
                                 ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ,
                                 ncp_hl_call_code_e.NCP_HL_SECUR_CHILD_KE_FINISHED_IND,
                                 ]

        self.use_cs = 2

    def on_get_module_version_rsp_se(self):
        self.host.ncp_req_set_zigbee_channel_mask(0, self.channel_mask)

    def begin(self):
        self.host.ncp_req_secur_add_ic(self.zr_ieee, INSTALL_CODE)

    def secur_add_ic_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_nwk_formation(0, CHANNEL_MASK, 5, 0, 0)

    def nwk_permit_joining_complete(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.set_simple_desc_ep1_se()
        self.set_ep = 1

    def on_nwk_get_short_by_ieee_rsp(self, short_addr):
        self.host.ncp_req_zdo_node_desc(short_addr)

    def zdo_node_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            self.rsp_log(rsp, ncp_hl_zdo_node_desc_t)

    def af_simple_desc_set(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if self.set_ep == 1:
            self.set_simple_desc_ep2_se()
            self.set_ep = 2

    def child_ke_finished_ind(self, ind, ind_len):
        self.ind_log(ind, ncp_hl_secur_child_cbke_ind_t)

    def on_dev_annce_ind(self, annce):
        self.host.ncp_req_nwk_get_ieee_by_short(annce.short_addr)

    def on_data_ind(self, dataind):
        # profile - SE
        # cluster - Time
        # dest endpoint - 20
        # frame control - 0x00
        # command - Read Attribute
        # attribute - Time
        if dataind.profileid == 0x0109 \
                and dataind.clusterid == 0x000a \
                and dataind.dst_endpoint == 20 \
                and dataind.data[0] == 0x00 \
                and dataind.data[2] == 0x00 \
                and dataind.data[3] == 0x00 \
                and dataind.data[4] == 0x00:
            logger.info("Read attribute request for Time attribute received")

            sequence_number = dataind.data[1]

            params = ncp_hl_data_req_param_t()
            params.dst_addr.short_addr = dataind.src_addr
            params.addr_mode = ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_16_ENDP_PRESENT
            params.profileid = 0x0109  # Smart Energy
            params.clusterid = 0x000a  # Time cluster
            params.dst_endpoint = dataind.src_endpoint
            params.src_endpoint = dataind.dst_endpoint
            params.radius = 30
            params.tx_options = ncp_hl_tx_options_e.NCP_HL_TX_OPT_SECURITY_ENABLED | ncp_hl_tx_options_e.NCP_HL_TX_OPT_ACK_TX
            params.use_alias = 0
            params.alias_src_addr = 0
            params.alias_seq_num = 0

            data = (c_ubyte * 128)()

            packet = [
                0x18,  # frame control: 0x18
                sequence_number,  # sequence number: the same as in the request
                0x01,  # command: read attribute response 0x00
                0x00, 0x00,  # attribute: time 0x0000
                0x00,  # status: success 0x00
                0x31,  # data type: 16-bit enum 0x32
                0x00, 0x00  # attribute value: 0x0000
            ]

            data[:len(packet)] = packet

            self.host.ncp_req_apsde_data_request(params, data, len(packet))


def main():
    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)
    fh = logging.FileHandler("zbs_se_key_est_zc_cs2.log")
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
