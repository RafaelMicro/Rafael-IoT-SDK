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
# PURPOSE: Host-side test implements ZED to issue Single poll commands
#
# As a ZC can use standalone SE test esi_device from samples/se/energy_service_interface
#

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
from time import sleep

logger = logging.getLogger(__name__)
log_file_name = "zbs_poll_zed.log"

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19

class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZED)
        self.auto_poll = None
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION: self.get_module_version_rsp_se,
            ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE: self.set_rx_on_when_idle_rsp,
            ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY: self.nwk_discovery_complete,
            ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN: self.nwk_join_complete,
            ncp_hl_call_code_e.NCP_HL_PIM_STOP_POLL: self.pim_stop_poll_rsp, 
            ncp_hl_call_code_e.NCP_HL_PIM_START_POLL: self.pim_start_poll_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_SINGLE_POLL: self.pim_single_poll_rsp,
            ncp_hl_call_code_e.NCP_HL_PIM_SET_LONG_POLL_INTERVAL: self.pim_set_long_p_intv_rsp,
        })

        self.update_indication_switch({
            ncp_hl_call_code_e.NCP_HL_ZDO_DEV_ANNCE_IND: self.dev_annce_ind,
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
                                 [ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE,        NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_STOP_POLL,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_SINGLE_POLL,            NCP_HL_STATUS.RET_INVALID_STATE], # if not joined, 1st request
                                 [ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_SET_LONG_POLL_INTERVAL, NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_SINGLE_POLL,            NCP_HL_STATUS.MAC_NO_DATA], # if default auto-poll running
                                 [ncp_hl_call_code_e.NCP_HL_PIM_STOP_POLL,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_SINGLE_POLL,            NCP_HL_STATUS.MAC_NO_DATA], # single poll(s) from host and no data
                                 [ncp_hl_call_code_e.NCP_HL_PIM_SINGLE_POLL,            NCP_HL_STATUS.MAC_NO_DATA],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_SINGLE_POLL,            NCP_HL_STATUS.MAC_NO_DATA],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_START_POLL,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_SINGLE_POLL,            NCP_HL_STATUS.RET_INVALID_STATE], # if host not stoped poll before NCP_HL_PIM_SINGLE_POLL
                                 ]

        self.use_cs = 2
        self.sin_poll_n = 0  # request NCP_HL_PIM_SINGLE_POLL number, consistently increases

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
        self.host.ncp_req_nwk_discovery(0, CHANNEL_MASK, 5)

    def nwk_discovery_complete(self, rsp, rsp_len):
        self.rsp_log(rsp, ncp_hl_nwk_discovery_rsp_hdr_t)
        dh = ncp_hl_nwk_discovery_rsp_hdr_t.from_buffer_copy(rsp.body)
        for i in range(0, dh.network_count):
            dsc = ncp_hl_nwk_discovery_dsc_t.from_buffer_copy(rsp.body, sizeof(dh) + i * sizeof(ncp_hl_nwk_discovery_dsc_t))
            self.rsp_log(rsp, ncp_hl_nwk_discovery_dsc_t)
        # In our test we have only 1 network, so join to the first one
        dsc = ncp_hl_nwk_discovery_dsc_t.from_buffer_copy(rsp.body, sizeof(dh))
        # make single poll when not joined, shall be return status RET_INVALID_STATE
        self.host.ncp_req_pim_stop_poll()
        sleep(1)
        self.host.ncp_req_pim_single_poll()
        sleep(1)
        # Join thru association to the pan and channel just found
        self.host.ncp_req_nwk_join_req(dsc.extpanid, 0, dsc.page, (1 << dsc.channel), 5,
                                       ncl_hl_mac_capability_e.NCP_HL_CAP_ALLOCATE_ADDRESS,
                                       0)

    def nwk_join_complete(self, rsp, rsp_len):
        self.rsp_log(rsp)
        # set long poll interval = 5 sec
        self.host.ncp_req_pim_set_long_poll_interval(5 * 4)
        body = ncp_hl_nwk_nlme_join_rsp_t.from_buffer_copy(rsp.body)
        if rsp.hdr.status_code == 0:
            logger.debug("joined ok short_addr {:#x} extpanid {} page {} channel {} enh_beacon {} mac_if {}".format(
                body.short_addr, body.extpanid,
                body.page, body.channel, body.enh_beacon, body.mac_iface_idx))
            sleep(15)
            self.host.ncp_req_pim_single_poll()

    def on_nwk_get_short_by_ieee_rsp(self, short_addr):
        self.host.ncp_req_zdo_node_desc(short_addr)

    def zdo_node_desc_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if rsp.hdr.status_code == 0:
            self.rsp_log(rsp, ncp_hl_zdo_node_desc_t)


    def dev_annce_ind(self, ind, ind_len):
        annce = ncp_hl_dev_annce_t.from_buffer_copy(ind.body, 0)
        logger.info("device annce: dev {:#x} ieee {} cap {:#x}".format(annce.short_addr, list(
            map(lambda x: hex(x), list(annce.long_addr))), annce.capability))
        # I am a non-sleepy ZED. In that test the only possible device annce is from another joined device
        self.partner_addr = annce.short_addr
        self.partner_ieee = annce.long_addr

    def pim_stop_poll_rsp(self, rsp, rsp_len): 
        self.rsp_log(rsp)

    def pim_start_poll_rsp(self, rsp, rsp_len): 
        self.rsp_log(rsp)
    
    def pim_set_long_p_intv_rsp(self, rsp, rsp_len): 
        self.rsp_log(rsp)

    def status_from(inp):
        status_code = inp & 0xFF
        category = (inp >> 8) & 0xFF
        if status_code != 0:
            code = status_id(category, status_code)
            return "{}:{}".format(
                    as_enum(category, NCP_HL_STATUS_CATEGORY),
                    as_enum(code, NCP_HL_STATUS))
        return "OK"

    def pim_single_poll_rsp(self, rsp, rsp_len):
        logger.info("single poll #{}, return sts {}"
            .format(self.sin_poll_n, as_enum(status_id(rsp.hdr.status_category, rsp.hdr.status_code), NCP_HL_STATUS)))
        if self.sin_poll_n == 0:
            # signe poll before join, do nothing
            pass
        elif self.sin_poll_n == 1:
            # need check wireshark for worked auto-poll after join
            sleep(15)
            self.host.ncp_req_pim_stop_poll()
        elif self.sin_poll_n == 4:
            self.host.ncp_req_pim_start_poll()
        # skip sin_poll_n = 0. it's test when not joined
        if self.sin_poll_n > 0 and self.sin_poll_n < 5:
            sleep(3)
            self.host.ncp_req_pim_single_poll()
        self.sin_poll_n = self.sin_poll_n + 1


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
