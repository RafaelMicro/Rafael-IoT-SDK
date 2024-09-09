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
# PURPOSE: Host-side test implements ZED role of CBKE initiated by nonTC and whitelist feature. Remote device isn't allowed to do CBKE.
#
# nonTC initiates CBKE procedure after ZED had joined to ZC, got CBKE and PLKE with nonTC
# Cryptosuite depends on use_cs variable, corresponding CS must present in production configuration
# That test requires 3 certificates while SE spec provides just 2, so generate your own certificates.
#

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
from time import sleep

logger = logging.getLogger(__name__)
log_file_name = "zbs_se_cbke_whitelist_zed_add_del_cs2.log"

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19

# CS1 - 1, CS2 - 2.
#
# Note about testing using esi_device ZC: be sure channel and CS is
# same at both tests. To change CS for esi_device update
# se/common/se_common.h and recompile.
# Note that installcodes are different for different CS, too, so with wrong CS we can't even authenticate.

# Test partner LK establishment with samples/se/metering/metering_device.
# To initiate key establishment from our side, set test_partner_lk_from_our_side to 1.
# To initiate it frpom the other side set test_partner_lk_from_our_side to 0.
# Note: keep it 1.
test_partner_lk_from_our_side = 1


# IEEE address on nonTC device to add it to whitelist
ihd_dev_addr_cs2 = bytes ([0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb])


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZED)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION: self.get_module_version_rsp_se,
            ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE: self.set_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY: self.nwk_discovery_complete,
            ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN: self.nwk_join_complete,
            ncp_hl_call_code_e.NCP_HL_SECUR_START_KE: self.secur_sbke_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ: self.zdo_node_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC: self.af_simple_desc_set,
            ncp_hl_call_code_e.NCP_HL_SECUR_START_PARTNER_LK: self.partner_lk_rsp,
            ncp_hl_call_code_e.NCP_HL_SECUR_KE_WHITELIST_ADD: self.whitelist_add_rsp,
            ncp_hl_call_code_e.NCP_HL_SECUR_KE_WHITELIST_DEL: self.whitelist_del_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_NWK_ADDR_REQ: self.zdo_nwk_addr_rsp,
        })

        self.update_indication_switch({
            ncp_hl_call_code_e.NCP_HL_SECUR_CBKE_SRV_FINISHED_IND: self.cbke_srv_finished_ind,
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
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_KE_WHITELIST_ADD,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_KE_WHITELIST_DEL,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_START_KE,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_ZDO_NWK_ADDR_REQ,               NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_START_PARTNER_LK,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_CBKE_SRV_FINISHED_IND,    NCP_HL_STATUS.RET_OK],
                                 ]

        self.use_cs = 2
        self.first_cbke_rsp = True

        self.zdo_addr_req_type = ncl_hl_zdo_addr_req_type_e.NCP_HL_ZDO_SINGLE_DEV_REQ

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

    def get_module_version_rsp_se(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_module_ver_t)
        # Use local ieee which hard-coded in the certificate.
        # Certificates from CE spec uses different local addresses.
        if self.use_cs == 1:
            self.zc_ieee = esi_dev_addr_cs1
            self.zr_ieee = ihd_dev_addr_cs1
        else:
            self.zc_ieee = esi_dev_addr_cs2
            self.zr_ieee = ihd_dev_addr_cs2
        self.host.ncp_req_get_local_ieee_addr(0)

    def begin(self):
        self.host.ncp_req_set_rx_on_when_idle(ncp_hl_on_off_e.OFF)

    def set_rx_on_when_idle_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_secur_ke_whitelist_add(ihd_dev_addr_cs2)

    def nwk_discovery_complete(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_nwk_discovery_rsp_hdr_t)
        dh = ncp_hl_nwk_discovery_rsp_hdr_t.from_buffer_copy(rsp.body)
        for i in range(0, dh.network_count):
            dsc = ncp_hl_nwk_discovery_dsc_t.from_buffer_copy(rsp.body, sizeof(dh) + i * sizeof(ncp_hl_nwk_discovery_dsc_t))
            self.rsp_log(rsp, ncp_hl_nwk_discovery_dsc_t)
        # In our test we have only 1 network, so join to the first one
        dsc = ncp_hl_nwk_discovery_dsc_t.from_buffer_copy(rsp.body, sizeof(dh))
        # Join thru association to the pan and channel just found
        self.host.ncp_req_nwk_join_req(dsc.extpanid, 0, dsc.page, (1 << dsc.channel), 5,
                                       ncl_hl_mac_capability_e.NCP_HL_CAP_ALLOCATE_ADDRESS,
                                       0)

    def nwk_join_complete(self, rsp, rsp_len):
        self.rsp_log(rsp)
        body = ncp_hl_nwk_nlme_join_rsp_t.from_buffer_copy(rsp.body)
        if rsp.hdr.status_code == 0:
            logger.debug("joined ok short_addr {:#x} extpanid {} page {} channel {} enh_beacon {} mac_if {}".format(
                body.short_addr, body.extpanid,
                body.page, body.channel, body.enh_beacon, body.mac_iface_idx))
            self.host.ncp_req_secur_start_ke(self.use_cs, 0x0000)

    def secur_sbke_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        # do it only for the first cbke rsp (with ZC)
        if self.first_cbke_rsp == True:
            self.first_cbke_rsp = False
            zc_ieee = ncp_hl_ieee_addr_t(ihd_dev_addr_cs2)
            short_addr = ncp_hl_nwk_addr_t(0xFFFF)
            self.host.ncp_req_zdo_nwk_addr(short_addr, zc_ieee, ncl_hl_zdo_addr_req_type_e.NCP_HL_ZDO_SINGLE_DEV_REQ, 0)

    def whitelist_add_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_secur_ke_whitelist_del(ihd_dev_addr_cs2)

    def whitelist_del_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.set_simple_desc_ep1_se()
        self.set_ep = 1

    def zdo_nwk_addr_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            addr_rsp = ncp_hl_zdo_addr_rsp_t.from_buffer_copy(rsp.body, 0)
            logger.info("zdo_nwk_addr_rsp {:{opt}}".format(addr_rsp, opt=self.zdo_addr_opt()))
            self.partner_addr = addr_rsp.short_addr_remote_dev.u16
            if test_partner_lk_from_our_side == 1:
                self.host.ncp_req_establish_partner_lk(addr_rsp.short_addr_remote_dev.u16)

    # resp to partner link key establishment initiated by us
    def partner_lk_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if status_id(rsp.hdr.status_category, rsp.hdr.status_code) == NCP_HL_STATUS.RET_TIMEOUT:
            # timeout, make request again
            self.host.ncp_req_establish_partner_lk(self.partner_addr)

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
            # Let other device establish TLCK
            sleep(10)
            self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)

    def cbke_srv_finished_ind(self, ind, ind_len):
        self.ind_log(ind, ncp_hl_secur_cbke_srv_finished_ind_t)


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
