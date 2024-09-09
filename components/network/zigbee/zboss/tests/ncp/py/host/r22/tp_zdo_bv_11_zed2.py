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

from time import sleep
from host.base_test import *
from host.ncp_hl import *

logger = logging.getLogger(__name__)

#
# Test itself
#

MY_IEEE_ADDR = bytes([0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])

LONG_POLL_INTERVAL = 24 # 24 = 6seconds * 4 quarterseconds
FAST_POLL_INTERVAL = 4  # 4 = 1second * 4 quarterseconds
LONG_POLL_PKT_N    = 5  # number of packets for receiving in long polling to change state
FAST_POLL_PKT_N    = 5  # number of packets for receiving in fast polling to change state
STOP_POLL_TIME     = 10 # on that time all polling will be stopped


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, MY_IEEE_ADDR, ncp_hl_role_e.NCP_HL_ZED)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE : self.set_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_RX_ON_WHEN_IDLE : self.get_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY : self.nwk_discovery_complete,
            ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN :self.nwk_join_complete,
            ncp_hl_call_code_e.NCP_HL_PIM_SET_LONG_POLL_INTERVAL : self.pim_set_long_interval_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_SET_FAST_POLL_INTERVAL : self.pim_set_fast_interval_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_START_FAST_POLL : self.pim_start_fast_poll_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_STOP_FAST_POLL : self.pim_stop_fast_poll_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_START_POLL : self.pim_start_poll_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_STOP_POLL : self.pim_stop_poll_rsp,
            ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ :self.data_conf,
        })

        self.update_indication_switch({
            ncp_hl_call_code_e.NCP_HL_ZDO_DEV_ANNCE_IND : self.dev_annce_ind,
            ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND : self.data_ind
        })

        # packets counter for determining step in polling sequence
        self.send_pack = 0

    def begin(self):
        self.host.ncp_req_set_rx_on_when_idle(ncp_hl_on_off_e.OFF)

    def set_rx_on_when_idle_rsp(self, rsp, rsp_len):
        logger.info("rx on when idle set. status %d", rsp.hdr.status_code)
        self.host.ncp_req_get_rx_on_when_idle()

    def get_rx_on_when_idle_rsp(self, rsp, rsp_len):
        logger.info("rx on when idle obtained. status %d", rsp.hdr.status_code)
        if rsp.hdr.status_code == 0:
            val = ncp_hl_uint8_t.from_buffer_copy(rsp.body, 0).uint8
            strval = "OFF" if val == ncp_hl_on_off_e.OFF else "ON"
            logger.info("rx_on_when_idle: %s", strval)
        self.host.ncp_req_nwk_discovery(0, self.channel_mask, 5)

    def pim_set_long_interval_rsp(self, rsp, rsp_len):
        logger.info("pim set long interval rsp set. status %d", rsp.hdr.status_code)
        self.host.ncp_req_pim_set_fast_poll_interval(FAST_POLL_INTERVAL)

    def pim_set_fast_interval_rsp(self, rsp, rsp_len):
        logger.info("pim set fast interval rsp set. status %d", rsp.hdr.status_code)

    def pim_start_fast_poll_rsp(self, rsp, rsp_len):
        logger.info("pim start fast poll rsp set. status %d", rsp.hdr.status_code)

    def pim_stop_fast_poll_rsp(self, rsp, rsp_len):
        logger.info("pim stop fast poll rsp set. status %d", rsp.hdr.status_code)

    def pim_start_poll_rsp(self, rsp, rsp_len):
        logger.info("pim start poll rsp set. status %d", rsp.hdr.status_code)

    def pim_stop_poll_rsp(self, rsp, rsp_len):
        logger.info("pim stop poll rsp set. status %d", rsp.hdr.status_code)
        sleep(STOP_POLL_TIME)
        self.send_pack = 0
        self.host.ncp_req_pim_start_poll()

    # please note, configuration parameters for ncp_req_nwk_join_req shall be set according to device role and
    # capabilities
    def nwk_discovery_complete(self, rsp, rsp_len):
        logger.info("nwk_discovery_complete. status %d", rsp.hdr.status_code)
        dh = ncp_hl_nwk_discovery_rsp_hdr_t.from_buffer(rsp.body)
        logger.debug("nwk discovery: network_count %d", dh.network_count)
        for i in range (0, dh.network_count):
            dsc = ncp_hl_nwk_discovery_dsc_t.from_buffer(rsp.body, sizeof(dh) + i * sizeof(ncp_hl_nwk_discovery_dsc_t))
            logger.debug("nwk #{}: extpanid {} panid {:#x} nwk_update_id {} page {} channel {} stack_profile {} permit_"
                         "joining {} router_capacity {} end_device_capacity {}".format(
                i, dsc.extpanid, dsc.panid,
                dsc.nwk_update_id, dsc.page, dsc.channel,
                self.host.nwk_dsc_stack_profile(dsc.flags),
                self.host.nwk_dsc_permit_joining(dsc.flags),
                self.host.nwk_dsc_router_capacity(dsc.flags),
                self.host.nwk_dsc_end_device_capacity(dsc.flags)))
        # In our test we have only 1 network, so join to the first one
        dsc = ncp_hl_nwk_discovery_dsc_t.from_buffer(rsp.body, sizeof(dh))
        # Join thru association to the pan and channel just found
        self.host.ncp_req_nwk_join_req(dsc.extpanid, 0, dsc.page, (1 << dsc.channel), 5,
                                       ncl_hl_mac_capability_e.NCP_HL_CAP_ALLOCATE_ADDRESS,
                                       0)

    def nwk_joined(self, body):   
        logger.debug("joined ok short_addr {:#x} extpanid {} page {} channel {} enh_beacon {} mac_if {}".format(
            body.short_addr, body.extpanid,
            body.page, body.channel, body.enh_beacon, body.mac_iface_idx))              
        self.host.ncp_req_pim_set_long_poll_interval(LONG_POLL_INTERVAL)

    def nwk_join_complete(self, rsp, rsp_len):
        logger.info("nwk_join_complete. status %d", rsp.hdr.status_code)
        if rsp.hdr.status_code == 0:
            body = ncp_hl_nwk_nlme_join_rsp_t.from_buffer(rsp.body)
            self.nwk_joined(body)
        self.get_ep = 1

    def nwk_rejoin_complete(self, ind, ind_len):
        logger.info("nwk_rejoin_complete.")
        body = ncp_hl_nwk_nlme_join_rsp_t.from_buffer(ind.body)
        self.nwk_joined(body) 
        
    def nwk_rejoin_failed(self, ind, ind_len):
        status = ncp_hl_status_t.from_buffer(ind.body, 0)
        logger.info("nwk_rejoin_failed. status %d", status.status_code)

    def on_data_conf(self, conf):
        self.send_pack += 1
        if self.send_pack == LONG_POLL_PKT_N:
            self.host.ncp_req_pim_start_fast_poll()
        elif self.send_pack == LONG_POLL_PKT_N + FAST_POLL_PKT_N:
            self.host.ncp_req_pim_stop_fast_poll()
        elif self.send_pack == LONG_POLL_PKT_N + FAST_POLL_PKT_N + LONG_POLL_PKT_N:
            self.host.ncp_req_pim_stop_poll()

    def nwk_leave_ind(self, ind, ind_len):
        leave_ind = ncp_hl_nwk_leave_ind_t.from_buffer(ind.body, 0)
        logger.info("nwk_leave_ind: ieee {} rejoin {}".format(list(map(hex, list(leave_ind.ieee_addr))), leave_ind.rejoin))        
