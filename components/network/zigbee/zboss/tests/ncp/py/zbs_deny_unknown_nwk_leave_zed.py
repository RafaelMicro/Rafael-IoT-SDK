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
# PURPOSE: Host-side test implements ZED checks that allow/deny leaving the network setting for ZED
# it is not supported.
#
# TC initiates CBKE procedure after ZED had joined and sets leave_allowed to 0.
# After that ZED waits 10sec to let ZC send leave request, which is expected
# to succeed. Then, ZED sets leave_allowed back to 1 and waits for second leave
# request which is expected to succeed.
# Cryptosuite depends on production configuration


from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
from time import sleep

logger = logging.getLogger(__name__)
log_file_name = "zbs_deny_unknown_nwk_leave_zed.log"

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19

class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZED)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION: self.get_module_version_rsp_se,
            ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN: self.nwk_join_complete,
            ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE: self.set_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ: self.zdo_node_desc_rsp,
            ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC: self.af_simple_desc_set,
            ncp_hl_call_code_e.NCP_HL_SET_LEAVE_WO_REJOIN_ALLOWED: self.set_leave_wo_rejoin_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_LEAVE_WO_REJOIN_ALLOWED: self.get_leave_wo_rejoin_rsp
        })

        self.update_indication_switch({
            ncp_hl_call_code_e.NCP_HL_SECUR_CBKE_SRV_FINISHED_IND: self.cbke_srv_finished_ind,
            ncp_hl_call_code_e.NCP_HL_NWK_REJOINED_IND: self.nwk_rejoin_complete,
            ncp_hl_call_code_e.NCP_HL_NWK_LEAVE_IND: self.handle_leave_ind
        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL_MASK,    NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_PAN_ID,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_ROLE,            NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_START_POLL,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_LEAVE_WO_REJOIN_ALLOWED,NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LEAVE_WO_REJOIN_ALLOWED,NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SECUR_CBKE_SRV_FINISHED_IND,NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_LEAVE_WO_REJOIN_ALLOWED,NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_AUTHENTICATED,          NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_LEAVE_WO_REJOIN_ALLOWED,NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_LEAVE_IND,              NCP_HL_STATUS.RET_OK],
                                 ]

        self.key_idx = 0
        self.leave_iteration = 0 # 0 leave from unknown device will not be accepted
                                 # 1 leave from ZC without rejoin flag will not be accepted
                                 # 2 leave from ZC without rejoin flag will be accepted

        self.leave_allowed = False


        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

    def get_module_version_rsp_se(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_module_ver_t)
        self.host.ncp_req_get_local_ieee_addr(0)

    def begin(self):
        # start with leave without rejoin allowed
        logger.info("Leave iteration 0: Nwk leave allowed, Nwk leave without rejoin allowed")
        self.host.ncp_req_set_leave_wo_rejoin(1)

    def set_rx_on_when_idle_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.set_simple_desc_ep1_se()
        self.set_ep = 1

    def nwk_join_complete(self, rsp, rsp_len):
        self.rsp_log(rsp)
        body = ncp_hl_nwk_nlme_join_rsp_t.from_buffer_copy(rsp.body)
        if rsp.hdr.status_code == 0:
            logger.debug("joined ok short_addr {:#x} extpanid {} page {} channel {} enh_beacon {} mac_if {}".format(
                body.short_addr, body.extpanid,
                body.page, body.channel, body.enh_beacon, body.mac_iface_idx))

    def cbke_srv_finished_ind(self, ind, ind_len):
        self.ind_log(ind, ncp_hl_secur_cbke_srv_finished_ind_t)
        # wait for leave requests
        # leave without rejoin is enabled and nwk leave is enabled but leave requests are from unknown device and will be ignored
        logger.info("Leave from unknown devices will be received in the following 15 seconds and are expected to be discarded")
        sleep(15)
        self.leave_iteration = self.leave_iteration + 1
        # disable leave without rejoin since after three leave requests from unknown device valid leaves without rejoin wil be sent
        self.host.ncp_req_set_leave_wo_rejoin(0)

    def get_leave_wo_rejoin_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_req_set_rx_on_when_idle(ncp_hl_on_off_e.OFF)

    def set_leave_wo_rejoin_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if self.leave_iteration == 0:
            self.host.ncp_req_get_leave_wo_rejoin()
        elif self.leave_iteration == 1:
            logger.info("Leave iteration 1: Nwk leave allowed, Nwk leave without rejoin disallowed")
            logger.info("Valid Nwk leave without rejoin flag will be received. (they will be discarded because"
                        "leave without rejoin is not allowed)")
            # wait for more leave requests that are expect to not be successfull
            sleep(10)
            self.leave_iteration = self.leave_iteration + 1
            self.host.ncp_req_get_authenticated()
        elif self.leave_iteration == 2:
            logger.info("Leave iteration 2: Nwk leave allowed, Nwk leave without rejoin allowed")

            # leave is now allowed because mono sample only sends leave without rejoin,
            # and only after this command leave without rejoin is allowed
            self.leave_allowed = True
            logger.info("Leave will be successfull shortly")


    def get_authenticated_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        auth_val = ncp_hl_uint8_t.from_buffer_copy(rsp.body, 0)
        if (auth_val):
            logger.info("Authenticated. Leave was failed as expected.")
        else:
            logger.error("Not authenticated. Leave was successful when it is not expect to be.")

        if self.leave_iteration == 2:
            self.host.ncp_req_set_leave_wo_rejoin(1)

    def nwk_rejoin_complete(self, ind, ind_len):
        self.ind_log(ind, ncp_hl_nwk_leave_ind_t)
        logger.error("Rejoin happened and it is not expected in this test.")

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

    def handle_leave_ind(self, ind, ind_len):
        self.ind_log(ind)
        if self.leave_allowed == False:
            # sample only sends valid leaves without rejoin. this flag is only True when leave wo rejoin is enable
            logger.error("Received leave when its not allowed!")
        else:
            logger.info("Leave req received when it is allowed!")

        # ensure rejoin does not happen
        logger.info("Wait 10 seconds to ensure rejoin does not occur")
        sleep(10)

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
