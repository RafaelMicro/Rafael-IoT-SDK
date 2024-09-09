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
# PURPOSE: Host-side test implements ZR part of Key Establishment.
#
# SE join and key establishment - ZR role. Keys & IC for IHD in samples/se
# As a ZC can use standalone SE test esi_device from samples/se/energy_service_interface
# As another ZR for passive side of partner lk establishment use standalone SE test samples/se/metering/metering_device
#

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
from time import sleep

logger = logging.getLogger(__name__)

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19
INSTALL_CODE = bytes([0x96, 0x6b, 0x9f, 0x3e, 0xf9, 0x8a, 0xe6, 0x05, 0x97, 0x08])

# CS1 - 1, CS2 - 2.
#
# Note about testing using esi_device ZC: be sure channel and CS is
# same at both tests. To change CS for esi_device update
# se/common/se_common.h and recompile.
# Note that installcodes are different for different CS, too, so with wrong CS we can't even authenticate.

# Test partner LK establishment with samples/se/metering/metering_device.
# To initiate key establishment from our side, set test_partner_lk_from_our_side to 1.
# To initiate it frpom the other side set test_partner_lk_from_our_side to 0.
test_partner_lk_from_our_side = 1


#
# Keys from SE specification. Can use it for testing freely.
# Got from samples/se/common/se_cert_spec.h
#
# ZC is ESI from se_cert_spec.h.
# ZR is IHD from se_cert_spec.h.
#

class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION: self.get_module_version_rsp_se,
            ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN: self.nwk_join_complete,
            ncp_hl_call_code_e.NCP_HL_SECUR_START_KE: self.secur_sbke_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ: self.zdo_node_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC: self.af_simple_desc_set,
            ncp_hl_call_code_e.NCP_HL_ZDO_MATCH_DESC_REQ: self.match_desc_resp,
        })

        self.update_indication_switch({
        })

        self.required_packets = [ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_NCP_RESET,
                                 ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,
                                 ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,
                                 ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,
                                 ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,
                                 ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,
                                 ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,
                                 ncp_hl_call_code_e.NCP_HL_SECUR_START_KE,
                                 ]

        self.use_cs = 2

    def get_module_version_rsp_se(self, rsp, rsp_len):
        ver = rsp.body.uint32
        logger.info("NCP ver 0x%x", ver)
        # Use local ieee which hard-coded in the certificate.
        # Certificates from CE spec uses different local addresses.
        if self.use_cs == 1:
            self.zc_ieee = esi_dev_addr_cs1
            self.zr_ieee = ihd_dev_addr_cs1
        else:
            self.zc_ieee = esi_dev_addr_cs2
            self.zr_ieee = ihd_dev_addr_cs2
        self.host.ncp_req_get_local_ieee_addr()

    def begin(self):
        self.set_simple_desc_ep1_se()
        self.set_ep = 1

    def nwk_join_complete(self, rsp, rsp_len):
        self.rsp_log(rsp)
        body = ncp_hl_nwk_nlme_join_rsp_t.from_buffer(rsp.body)
        if rsp.hdr.status_code == 0:
            logger.debug("joined ok short_addr {:#x} extpanid {} page {} channel {} enh_beacon {} mac_if {}".format(
                body.short_addr, body.extpanid,
                body.page, body.channel, body.enh_beacon, body.mac_iface_idx))
        self.host.ncp_req_secur_start_ke(self.use_cs)

    def secur_sbke_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        # resp to partner link key establishment initiated by us

        hdr = ncp_hl_match_desc_hdr_t()
        hdr.addr_of_interest = 0  # Trust Center
        hdr.profile_id = 0x0109  # Smart Energy
        hdr.in_clu_count = 0
        hdr.out_clu_count = 1

        match_desc_req = ncp_hl_match_desc_t()
        match_desc_req.hdr = hdr
        match_desc_req.clusters[0] = 0x000a  # Time cluster

        self.host.ncp_req_zdo_match_desc(match_desc_req)

    def on_nwk_get_short_by_ieee_rsp(self, short_addr):
        self.host.ncp_req_zdo_node_desc(short_addr)

    def zdo_node_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            self.rsp_log(rsp, ncp_hl_zdo_node_desc_t)

    def set_simple_desc_ep2(self):
        cmd = ncp_hl_set_simple_desc_t()
        cmd.hdr.endpoint = 20
        cmd.hdr.profile_id = 0x0109  # SE
        cmd.hdr.device_id = 0x1234
        cmd.hdr.device_version = 9
        # Let it be IHD. Actually we need only KEC now, but let's setup
        # the same set of descriptors as our IHD does - see
        # ZB_SE_DECLARE_IN_HOME_DISPLAY_CLUSTER_LIST()
        cmd.hdr.in_clu_count = 2
        cmd.hdr.out_clu_count = 11
        cmd.clusters[0] = 0x0000  # ZB_ZCL_CLUSTER_ID_BASIC srv
        cmd.clusters[1] = 0x0800  # ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT src
        cmd.clusters[2] = 0x0800  # ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT cli
        cmd.clusters[3] = 0x000a  # ZB_ZCL_CLUSTER_ID_TIME cli
        cmd.clusters[4] = 0x0707  # ZB_ZCL_CLUSTER_ID_CALENDAR cli
        cmd.clusters[5] = 0x0702  # ZB_ZCL_CLUSTER_ID_METERING cli
        cmd.clusters[6] = 0x0700  # ZB_ZCL_CLUSTER_ID_PRICE cli
        cmd.clusters[7] = 0x0701  # ZB_ZCL_CLUSTER_ID_DRLC cli
        cmd.clusters[8] = 0x0025  # ZB_ZCL_CLUSTER_ID_KEEP_ALIVE cli
        cmd.clusters[9] = 0x0709  # ZB_ZCL_CLUSTER_ID_EVENTS cli
        cmd.clusters[10] = 0x0703  # ZB_ZCL_CLUSTER_ID_MESSAGING cli
        cmd.clusters[11] = 0x0706  # ZB_ZCL_CLUSTER_ID_ENERGY_MANAGEMENT cli
        cmd.clusters[12] = 0x070A  # ZB_ZCL_CLUSTER_ID_MDU_PAIRING cli
        cmd.clusters[13] = 0x0705  # ZB_ZCL_CLUSTER_ID_PREPAYMENT cli
        self.set_ep = 2
        self.host.ncp_req_set_simple_desc(cmd)

    def af_simple_desc_set(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if self.set_ep == 1:
            self.set_simple_desc_ep2()
        else:
            self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)

    def match_desc_resp(self, ind, ind_len):
        resp = self.ind_rx(ind, ncp_hl_match_desc_rsp_t)
        if resp.match_len == 0:
            logger.info("No time cluster found")
        else:
            self._ep = resp.match_list[0]
            logger.info("Time cluster found, ep {:#x}".format(self._ep))
            self._read_time(
                0,
                self._ep,
                20  # ID of the endpoint on which Time Client cluster resides should be used here
            )

    def on_data_ind(self, dataind):
        # profileid - SE
        # clusterid - Time
        # sequence number: (dataind.data[1]) 0x19
        # command: (dataind.data[2]) Read Attribute Response
        if dataind.profileid == 0x0109 \
                and dataind.clusterid == 0x000a\
                and dataind.data[1] == 0x19 \
                and dataind.data[2] == 0x01:
            logger.info("Read attribute response received")

    def _read_time(self, addr, dest_ep, src_ep):
        params = ncp_hl_data_req_param_t()
        params.dst_addr.short_addr = addr
        params.addr_mode = ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_16_ENDP_PRESENT
        params.profileid = 0x0109  # Smart Energy
        params.clusterid = 0x000a  # Time cluster
        params.dst_endpoint = dest_ep
        params.src_endpoint = src_ep
        params.radius = 30
        params.tx_options = ncp_hl_tx_options_e.NCP_HL_TX_OPT_SECURITY_ENABLED | ncp_hl_tx_options_e.NCP_HL_TX_OPT_ACK_TX
        params.use_alias = 0
        params.alias_src_addr = 0
        params.alias_seq_num = 0

        data = (c_ubyte * 128)()

        packet = [
            0x00,  # frame control: 0x00
            0x19,  # sequence number: 0x19
            0x00,  # command: read attribute 0x00
            0x00, 0x00  # attribute: time 0x0000
        ]

        data[:len(packet)] = packet
        self.host.ncp_req_apsde_data_request(params, data, len(packet))


def main():
    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)
    fh = logging.FileHandler("zbs_se_key_est_zr_cs2.log")
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
