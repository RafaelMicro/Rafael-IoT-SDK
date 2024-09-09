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
# tp_pro_bv_01 test in ZR role, Host side. Use it with tp_pro_bv_01_zc.hex monolithic FW for CC1352.

from host.base_test import *
from host.ncp_hl import *

logger = logging.getLogger(__name__)

#
# Test itself
#

MY_IEEE_ADDR = bytes([0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00])
IEEE_ADDR_D  = bytes([0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00])

class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, MY_IEEE_ADDR, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY : self.nwk_discovery_complete,
            ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN :self.nwk_join_complete,
            ncp_hl_call_code_e.NCP_HL_APSME_BIND: self.apsme_bind_rsp,
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
            self.short_addr = ncp_hl_nwk_addr_t(body.short_addr.u16)

            # wait 1 min
            # Collision on ncp is not likely
            t = threading.Timer(60, self.send_data_cb)
            t.start()


    def send_data_cb(self):
        self.send_data()


    def data_ind(self, ind, ind_len):
        dataind = ncp_hl_data_ind_t.from_buffer(ind.body, 0)
        logger.info("apsde.data.ind len {} fc {:#x} src {:#x}/{:#x} dst {:#x}/{:#x} group {:#x} src ep {} dst ep {} cluster {:#x} profile {:#x} apscnt {} lqi {} rssi {} keyfl {:#x}".format(
            dataind.data_len, dataind.fc, dataind.src_addr, dataind.mac_src_addr, dataind.dst_addr, dataind.mac_dst_addr, dataind.group_addr, dataind.src_endpoint, dataind.dst_endpoint,
            dataind.clusterid, dataind.profileid, dataind.aps_counter, dataind.lqi, dataind.rssi, dataind.key_flags))
        data4dump = list(dataind.data)[:(dataind.data_len)]
        logger.info("data {}".format(list(map(lambda x: hex(x), data4dump))))

    def data_conf(self, rsp, rsp_len):
        if rsp_len <= sizeof(rsp.hdr):
            logger.info("apsde-data.conf status {} {}".format(rsp.hdr.status_category, rsp.hdr.status_code))
        else:
            conf = ncp_hl_apsde_data_conf_t.from_buffer(rsp.body.arr, 0)
            # TODO: all addressing modes
            logger.info("apsde-data.conf status {} {} addr_mode {} dst_addr {} src_endpoint {} dst_endpoint {} tx_time {} ".format(
                rsp.hdr.status_category, rsp.hdr.status_code,
                conf.addr_mode,
                conf.dst_addr.short_addr if conf.addr_mode != ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT else list(map(lambda x: hex(x), list(conf.dst_addr.long_addr))),
                conf.src_endpoint, conf.dst_endpoint, conf.tx_time))
            self.host.ncp_req_apsme_bind_request(ncp_hl_ieee_addr_t(self.ieee_addr), 0x01, 0x0001, ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT, ncp_hl_ieee_addr_t(IEEE_ADDR_D), 0xF0, 0)


    def apsme_bind_rsp(self, rsp, rsp_len):
        logger.info("apsme_bind_rsp. status %d", rsp.hdr.status_code)
        self.send_data()
        self.rsp_switch.pop(ncp_hl_call_code_e.NCP_HL_APSME_BIND)


    def send_data(self):
        params = ncp_hl_data_req_param_t()
        params.addr_mode = ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT
        params.profileid = 0x0002
        params.clusterid = 0x0001
        params.src_endpoint = 1
        params.tx_options = ncp_hl_tx_options_e.NCP_HL_TX_OPT_SECURITY_ENABLED | ncp_hl_tx_options_e.NCP_HL_TX_OPT_ACK_TX
        params.use_alias = 0
        params.alias_src_addr = 0
        params.alias_seq_num = 0
        params.radius = 1
        length = 70
        data_type = c_ubyte * 128
        data = data_type()
        for i in range(0, length):
            data[i] = i % 32 + ord('0')
        self.host.ncp_req_apsde_data_request(params, data, length)

