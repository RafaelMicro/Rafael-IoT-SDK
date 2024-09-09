#!/usr/bin/env python3.7

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
# PURPOSE: Host-side test which tests binding table size while using an offset.
#

# Test consists of two parts:
# First one: check that it's possible to create exactly LOCAL_BINDING_TABLE_SIZE_SRC
# bindings for unique source EPs. LOCAL_BINDING_TABLE_SIZE_SRC is the size between 0 and
# the REMOTE_BIND_OFFSET defined, which divides the binding table overall size in one part for
# LOCAL bindigs only and other to REMOTE bindigs only
# After that clean up binding table by
# unbinding all the bindings which were previously created.
# Second part: check that it's possible to create exactly LOCAL_BINDING_TABLE_SIZE_DST
# bindings for unique destination EPs.
#
# Hardcoded values LOCAL_BINDING_TABLE_SIZE_SRC and LOCAL_BINDING_TABLE_SIZE_DST correspond
# to the size between 0 and REMOTE_BIND_OFFSET where REMOTE_BIND_OFFSET needs to be smaller than
# the binding table size the configuration (ZB_APS_SRC_BINDING_TABLE_SIZE and
# ZB_APS_DST_BINDING_TABLE_SIZE).
#
# Note that WARNINGs in test output are expected, because not all
# BINDING_TABLE_SIZE_* bind/unbind packages are specified in expected packets list.

from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner
import os

logger = logging.getLogger(__name__)
#log_file_name = "zbs_aps_binding_table_limit.log"
log_file_name = os.path.basename(__file__).split(".")[0] + ".log"

#
# Test itself
#

CHANNEL_MASK = 0x80000  # channel 19
TEST_SRC_EP = 255
TEST_DST_EP = 255

# size of binding table reserved for local bindings
# bind ids allowed are from [0:REMOTE_BIND_OFFSET]
REMOTE_BIND_OFFSET = 75
LOCAL_BINDING_TABLE_SIZE_SRC = REMOTE_BIND_OFFSET
LOCAL_BINDING_TABLE_SIZE_DST = REMOTE_BIND_OFFSET

class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_APSME_BIND: self.apsme_bind_rsp,
            ncp_hl_call_code_e.NCP_HL_APSME_RM_BIND_ENTRY_BY_ID: self.apsme_rm_bind_id_rsp,
            ncp_hl_call_code_e.NCP_HL_SET_REMOTE_BIND_OFFSET: self.set_remote_bind_offset_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_REMOTE_BIND_OFFSET: self.get_remote_bind_offset_rsp
        })

        self.update_indication_switch({
        })

        # Note that return expected return codes are ignored in this test.
        # Pass criterion is amount of successfully created bindings, not statuses of particular requests.
        # Not all expected packages are specified in `required_packets` list.
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
                                 [ncp_hl_call_code_e.NCP_HL_SET_REMOTE_BIND_OFFSET,     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_REMOTE_BIND_OFFSET,     NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSME_BIND,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSME_RM_BIND_ENTRY_BY_ID,  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSME_BIND,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSME_RM_BIND_ENTRY_BY_ID,  NCP_HL_STATUS.RET_OK],
                                 ]

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

        self.src_test_status = "not started"
        self.dst_test_status = "not started"
        self.next_src = 0x01
        self.next_dst = 0x01
        self.bind_id = 0
        self.bind_id_unbind = LOCAL_BINDING_TABLE_SIZE_DST - 1

        self.test_ieee_addr = bytes([0x00, 0x00, 0xef, 0xcd, 0xab, 0x50, 0x50, 0x50])

        self.ignore_unexpected_status = 1 # Ignore failure statuses. Test procedure will handle errors by itself.

    def begin(self):
        # this test tests the capability to locally fill all the binding table, setting a remote bind offset cannot change
        # any of this behaviour because no remote bindings are expected so by introducing it here we make sure this feature
        # cannot interfere with local binding functionality
        self.host.ncp_apsme_set_remote_bind_offset(REMOTE_BIND_OFFSET)

    def set_remote_bind_offset_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.host.ncp_apsme_get_remote_bind_offset()

    def get_remote_bind_offset_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        offset = ncp_hl_uint8_t.from_buffer_copy(rsp.body, 0).uint8
        logger.debug("remote offset = {}".format(offset))
        if offset != REMOTE_BIND_OFFSET:
            logger.error("remote offset set and get do not match")

        self.host.ncp_req_nwk_discovery(0, self.channel_mask, 5)

    def on_nwk_join_complete(self, body):
        self.short_addr = ncp_hl_nwk_addr_t(body.short_addr.u16)
        self.src_test_status = "in progress"
        self.host.ncp_req_apsme_bind_request(ncp_hl_ieee_addr_t(self.ieee_addr), self.next_src, 0x0003,
                                                 ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT,
                                                 ncp_hl_ieee_addr_t(self.test_ieee_addr), TEST_DST_EP, self.bind_id)
        self.bind_id = self.bind_id + 1


    def apsme_bind_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        body = ncp_hl_bind_rsp_t.from_buffer_copy(rsp.body, 0)
        logger.info("New binding id:{}".format(body.id))
        self.bind_id_unbind = LOCAL_BINDING_TABLE_SIZE_DST - 1
        # error returned from previous binding operation mean that test for sources or destinations was finished.
        if (rsp.hdr.status_code != 0):
            # check, which part of test was finished and evaluate the result
            if (self.src_test_status == "in progress"):
                # decrement counter since previous bind operation was failed
                self.next_src = self.next_src - 1
                if (self.next_src == LOCAL_BINDING_TABLE_SIZE_SRC):
                    self.src_test_status = "passed"
                    logger.info("Status for sources: test {} ({} of {} bindings created).".format(
                                self.src_test_status, self.next_src, LOCAL_BINDING_TABLE_SIZE_SRC))
                else:
                    self.src_test_status = "failed"
                    logger.error("Status for sources: test {} ({} of {} bindings created).".format(
                                self.src_test_status, self.next_src, LOCAL_BINDING_TABLE_SIZE_SRC))
                self.host.ncp_apsme_rm_bind_entry_by_id(self.bind_id_unbind)
                self.bind_id_unbind = self.bind_id_unbind - 1
            else:
                if (self.dst_test_status == "in progress"):
                    # decrement counter since previous bind operation was failed
                    self.next_dst = self.next_dst - 1
                    if (self.next_dst == LOCAL_BINDING_TABLE_SIZE_DST):
                        self.dst_test_status = "passed"
                        logger.info("Status for destinations: test {} ({} of {} bindings created).".format(
                                    self.dst_test_status, self.next_dst, LOCAL_BINDING_TABLE_SIZE_DST))
                    else:
                        self.dst_test_status = "failed"
                        logger.error("Status for destinations: test {} ({} of {} bindings created).".format(
                                    self.dst_test_status, self.next_dst, LOCAL_BINDING_TABLE_SIZE_DST))
                    # send unbind command to show that test is finished
                    self.host.ncp_apsme_rm_bind_entry_by_id(self.bind_id_unbind)
                    self.bind_id_unbind = self.bind_id_unbind - 1

        # if at least one part of test is not finished yet, proceed with creating of bindings
        if (self.src_test_status == "in progress"):
            self.next_src = self.next_src + 1
            self.host.ncp_req_apsme_bind_request(ncp_hl_ieee_addr_t(self.ieee_addr), self.next_src, 0x0003,
                                                 ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT,
                                                 ncp_hl_ieee_addr_t(self.test_ieee_addr), TEST_DST_EP, self.bind_id)
            self.bind_id = self.bind_id + 1
        else:
            if (self.dst_test_status == "in progress"):
                self.next_dst = self.next_dst + 1
                self.host.ncp_req_apsme_bind_request(ncp_hl_ieee_addr_t(self.ieee_addr), TEST_SRC_EP, 0x0003,
                                                     ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT,
                                                     ncp_hl_ieee_addr_t(self.test_ieee_addr), self.next_dst, self.bind_id)
                self.bind_id = self.bind_id + 1

    def apsme_rm_bind_id_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        self.bind_id = 0
        if (self.next_src > 1):
            # clean up the binding table
            self.next_src = self.next_src - 1
            self.host.ncp_apsme_rm_bind_entry_by_id(self.bind_id_unbind)
            self.bind_id_unbind = self.bind_id_unbind - 1
        else:
            # start second part of test if it wasn't started yet
            if (self.dst_test_status == "not started"):
                self.dst_test_status = "in progress"
                self.host.ncp_req_apsme_bind_request(ncp_hl_ieee_addr_t(self.ieee_addr), TEST_SRC_EP, 0x0003,
                                                     ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT,
                                                     ncp_hl_ieee_addr_t(self.test_ieee_addr), self.next_dst, self.bind_id)
                self.bind_id = self.bind_id + 1
            # otherwise terminate the test
            else:
                pass

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
