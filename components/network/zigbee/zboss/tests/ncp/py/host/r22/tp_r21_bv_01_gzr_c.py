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
# tp_r21_bv_01 test in ZR role, Host side.

from host.base_test import *
from host.ncp_hl import *

logger = logging.getLogger(__name__)

#
# Test itself
#

MY_IEEE_ADDR = bytes([0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00])

class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, MY_IEEE_ADDR, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY : self.nwk_discovery_complete,
            ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN :self.nwk_join_complete,
            ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ : self.zdo_node_desc_rsp,
        })


    def begin(self):
        self.host.ncp_req_nwk_discovery(0, self.channel_mask, 5)


    def nwk_discovery_complete(self, rsp, rsp_len):
        logger.info("nwk_discovery_complete. status %d", rsp.hdr.status_code)
        dh = ncp_hl_nwk_discovery_rsp_hdr_t.from_buffer(rsp.body)
        logger.debug("nwk disccovery: network_count %d", dh.network_count)
        for i in range (0, dh.network_count):
            dsc = ncp_hl_nwk_discovery_dsc_t.from_buffer(rsp.body, sizeof(dh) + i * sizeof(ncp_hl_nwk_discovery_dsc_t))
            logger.debug("nwk #{}: extpanid {} panid {:#x} nwk_update_id {} page {} channel {} stack_profile {} permit_joining {} router_capacity {} end_device_capacity {}".format(
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
                                       ncl_hl_mac_capability_e.NCP_HL_CAP_ROUTER|ncl_hl_mac_capability_e.NCP_HL_CAP_ALLOCATE_ADDRESS,
                                       0)

    def nwk_join_complete(self, rsp, rsp_len):
        logger.info("nwk_join_complete. status %d", rsp.hdr.status_code)
        body = ncp_hl_nwk_nlme_join_rsp_t.from_buffer(rsp.body)
        if rsp.hdr.status_code == 0:
            logger.debug("joined ok short_addr {:#x} extpanid {} page {} channel {} enh_beacon {} mac_if {}".format(
                body.short_addr, body.extpanid,
                body.page, body.channel, body.enh_beacon, body.mac_iface_idx))
            self.short_addr = body.short_addr
            self.host.ncp_req_zdo_node_desc(0x0)


    def zdo_node_desc_rsp(self, rsp, rsp_len):
        logger.info("zdo_node_desc_rsp status %d", rsp.hdr.status_code)
        if rsp.hdr.status_code == 0:
            node_desc = ncp_hl_zdo_node_desc_t.from_buffer_copy(rsp.body, 0)
            logger.info("node desc: node_desc_flags {:#x} mac_capability_flags {:#x} manufacturer_code {:#x} max_buf_size {} max_incoming_transfer_size {} server_mask {:#x} max_outgoing_transfer_size {} desc_capability_field {:#x}".format(
                    node_desc.node_desc_flags, node_desc.mac_capability_flags, node_desc.manufacturer_code, node_desc.max_buf_size, node_desc.max_incoming_transfer_size, node_desc.server_mask, node_desc.max_outgoing_transfer_size, node_desc.desc_capability_field))
