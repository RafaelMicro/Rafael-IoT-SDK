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
# PURPOSE: Host-side test implements geting the number of binding entries in binding table,
#   reads each one by index and clearing the binding table.
#



from host.ncp_hl import *
from host.base_test import BaseTest
from host.test_runner import TestRunner

logger = logging.getLogger(__name__)
log_file_name = "zbs_aps_bind_table_read_clear_zr.log"

#
# Test itself
#

# Test steps:
#   1. Bind_req (expected BIND_ID = 0)
#   2. Unbind_req for BIND of 1. (removes BIND_ID = 0)
#   3. Bind_req (expects BIND_ID = 0)
#   4. Remove Bind by ID (removes BIND_ID = 0)
#   5. Bind_req for a specific ID = 0 (expects BIND_ID = 0)
#   6. Clear Bind Table (removes BIND_ID = 0)
#   7. Bind_req (expects BIND_ID = 0)
#   8. Bind_req for a specific ID=45 (expects BIND_ID = 45)
#   9. Bind_req for a specific ID=90 (expects BIND_ID = 90)
#   10. Bind_req for a specific ID=90 (expects BIND_ID = 91)
#   11. Get remote_bind_offset (expects 0xff <==> -1)
#   12. Set remote_bind_offset = 75
#   13. Get remote_bind_offset (expects 75)
#   14. Remove Bind by ID=90 (old BIND_ID that is now not allowed for remote bindings, it must succeed to remove)
#   15. Get BIND entry by ID=91 (must be able to read it correctly despite being an old entry with an ID > remote_bind_offset)
#   16. Unbind_req for BIND payload of 10. (expects it to be removed successfuly)
#   17. Bind_req for a specific ID = 80 (expected to fail since remote_bind_offset is 75)
#   18. Bind_req for a specific ID = TABLE_SIZE + 1 (must fail since all ID are in the interval 0:TBL_SIZE)



CHANNEL_MASK = 0x80000  # channel 19
TEST_SRC_EP = 255
TEST_DST_EP = 255

REMOTE_BIND_OFFSET = 75
BIND_TBL_SIZE = 150


class Test(BaseTest):

    def __init__(self, channel_mask):
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZR)
        self.update_response_switch({
            ncp_hl_call_code_e.NCP_HL_APSME_BIND: self.apsme_bind_rsp,
            ncp_hl_call_code_e.NCP_HL_APSME_GET_BIND_ENTRY_BY_ID: self.get_bind_entry_rsp,
            ncp_hl_call_code_e.NCP_HL_APSME_CLEAR_BIND_TABLE: self.clear_bind_table_rsp,
            ncp_hl_call_code_e.NCP_HL_APSME_RM_BIND_ENTRY_BY_ID: self.aps_rm_bind_by_id_rsp,
            ncp_hl_call_code_e.NCP_HL_SET_REMOTE_BIND_OFFSET: self.set_remote_bind_offset_rsp,
            ncp_hl_call_code_e.NCP_HL_GET_REMOTE_BIND_OFFSET: self.get_remote_bind_offset_rsp
        })

        self.update_indication_switch({
        })

        self.required_packets = [[ncp_hl_call_code_e.NCP_HL_NCP_RESET_IND,                   NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NCP_RESET,                       NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR,             NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL_MASK,         NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL,              NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_PAN_ID,                      NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_ROLE,                 NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_PIM_START_POLL,                  NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY,                   NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN,                   NCP_HL_STATUS.RET_OK],
                                 [ncp_hl_call_code_e.NCP_HL_APSME_BIND,                      NCP_HL_STATUS.RET_OK], # 1
                                 [ncp_hl_call_code_e.NCP_HL_APSME_RM_BIND_ENTRY_BY_ID,       NCP_HL_STATUS.RET_OK], # 2
                                 [ncp_hl_call_code_e.NCP_HL_APSME_BIND,                      NCP_HL_STATUS.RET_OK], # 3
                                 [ncp_hl_call_code_e.NCP_HL_APSME_RM_BIND_ENTRY_BY_ID,       NCP_HL_STATUS.RET_OK], # 4
                                 [ncp_hl_call_code_e.NCP_HL_APSME_BIND,                      NCP_HL_STATUS.RET_OK], # 5
                                 [ncp_hl_call_code_e.NCP_HL_APSME_CLEAR_BIND_TABLE,          NCP_HL_STATUS.RET_OK], # 6
                                 [ncp_hl_call_code_e.NCP_HL_APSME_BIND,                      NCP_HL_STATUS.RET_OK], # 7
                                 [ncp_hl_call_code_e.NCP_HL_APSME_BIND,                      NCP_HL_STATUS.RET_OK], # 8
                                 [ncp_hl_call_code_e.NCP_HL_APSME_BIND,                      NCP_HL_STATUS.RET_OK], # 9
                                 [ncp_hl_call_code_e.NCP_HL_APSME_BIND,                      NCP_HL_STATUS.RET_OK], # 10
                                 [ncp_hl_call_code_e.NCP_HL_GET_REMOTE_BIND_OFFSET,          NCP_HL_STATUS.RET_OK], # 11
                                 [ncp_hl_call_code_e.NCP_HL_SET_REMOTE_BIND_OFFSET,          NCP_HL_STATUS.RET_OK], # 12
                                 [ncp_hl_call_code_e.NCP_HL_GET_REMOTE_BIND_OFFSET,          NCP_HL_STATUS.RET_OK], # 13
                                 [ncp_hl_call_code_e.NCP_HL_APSME_RM_BIND_ENTRY_BY_ID,       NCP_HL_STATUS.RET_OK], # 14
                                 [ncp_hl_call_code_e.NCP_HL_APSME_GET_BIND_ENTRY_BY_ID,      NCP_HL_STATUS.RET_OK], # 15
                                 [ncp_hl_call_code_e.NCP_HL_APSME_RM_BIND_ENTRY_BY_ID,       NCP_HL_STATUS.RET_OK], # 16
                                 [ncp_hl_call_code_e.NCP_HL_APSME_BIND,                      NCP_HL_STATUS.ZB_APS_STATUS_INVALID_PARAMETER], # 17
                                 [ncp_hl_call_code_e.NCP_HL_APSME_BIND,                      NCP_HL_STATUS.ZB_APS_STATUS_INVALID_PARAMETER], # 18
                                 ]

        self.set_base_test_log_file(log_file_name)
        self.set_ncp_host_hl_log_file(log_file_name)

        self.first_bind = True
        self.test_ieee_addr = bytes([0x00, 0x00, 0xef, 0xcd, 0xab, 0x50, 0x50, 0x50])
        self.id_req = 0

        # variable to control each phase of the test
        self.step = 0

    def begin(self):
        self.host.ncp_req_nwk_discovery(0, self.channel_mask, 5)

    def on_nwk_join_complete(self, body):
        self.short_addr = ncp_hl_nwk_addr_t(body.short_addr.u16)
        self.host.ncp_req_apsme_bind_request(ncp_hl_ieee_addr_t(self.ieee_addr), TEST_SRC_EP, 0x0003,
                                                ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT,
                                                ncp_hl_ieee_addr_t(self.test_ieee_addr), TEST_DST_EP, 0)
        self.step = 1

    def aps_rm_bind_by_id_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if self.step == 2:
 # 3
            self.host.ncp_req_apsme_bind_request(ncp_hl_ieee_addr_t(self.ieee_addr), TEST_SRC_EP, 0x0003,
                                                ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT,
                                                ncp_hl_ieee_addr_t(self.test_ieee_addr), TEST_DST_EP, 0)
            self.step = 3
        if self.step == 4:
# 5
            self.host.ncp_req_apsme_bind_request(ncp_hl_ieee_addr_t(self.ieee_addr), TEST_SRC_EP, 0x0003,
                                    ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT,
                                    ncp_hl_ieee_addr_t(self.test_ieee_addr), TEST_DST_EP, 0)
            self.step = 5
# 15
        elif self.step == 14:
            self.host.ncp_apsme_get_bind_entry_by_id(91)
            self.step = 15
# 17
        elif self.step == 16:
            self.host.ncp_req_apsme_bind_request(ncp_hl_ieee_addr_t(self.ieee_addr), TEST_SRC_EP, 0x0006,
                            ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT,
                            ncp_hl_ieee_addr_t(self.test_ieee_addr), TEST_DST_EP, 80)
            self.step = 17


    def apsme_bind_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if self.step == 1:
            body = ncp_hl_bind_rsp_t.from_buffer_copy(rsp.body, 0)
            logger.debug("New binding id: {}".format(body.id))
            if body.id != 0:
                self.error("Binding id is not 0")
            self.host.ncp_apsme_rm_bind_entry_by_id(0)
            self.step = 2

        elif self.step == 3:
            self.host.ncp_apsme_rm_bind_entry_by_id(0)
            self.step = 4
            body = ncp_hl_bind_rsp_t.from_buffer_copy(rsp.body, 0)
            logger.debug("New binding id: {}".format(body.id))
            if body.id != 0:
                logger.error("Binding id is not 0")

        elif self.step == 5:
            body = ncp_hl_bind_rsp_t.from_buffer_copy(rsp.body, 0)
            logger.debug("New binding id: {}".format(body.id))
            if body.id != 0:
                logger.error("Binding id is not 0")

            self.host.ncp_apsme_clear_bind_table()
            self.step = 6
#8
        elif self.step == 7:
            body = ncp_hl_bind_rsp_t.from_buffer_copy(rsp.body, 0)
            logger.debug("New binding id: {}".format(body.id))
            if body.id != 0:
                logger.error("Binding id is not 0")

            self.host.ncp_req_apsme_bind_request(ncp_hl_ieee_addr_t(self.ieee_addr), TEST_SRC_EP, 0x0004,
                            ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT,
                            ncp_hl_ieee_addr_t(self.test_ieee_addr), TEST_DST_EP, 45)
            self.step = 8
#9
        elif self.step == 8:
            body = ncp_hl_bind_rsp_t.from_buffer_copy(rsp.body, 0)
            logger.debug("New binding id: {}".format(body.id))
            if body.id != 45:
                logger.error("Binding id is not 45")
            self.host.ncp_req_apsme_bind_request(ncp_hl_ieee_addr_t(self.ieee_addr), TEST_SRC_EP, 0x0005,
                            ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT,
                            ncp_hl_ieee_addr_t(self.test_ieee_addr), TEST_DST_EP, 90)
            self.step = 9
#10
        elif self.step == 9:
            body = ncp_hl_bind_rsp_t.from_buffer_copy(rsp.body, 0)
            logger.debug("New binding id: {}".format(body.id))
            if body.id != 90:
                logger.error("Binding id is not 90")
            self.host.ncp_req_apsme_bind_request(ncp_hl_ieee_addr_t(self.ieee_addr), TEST_SRC_EP, 0x0006,
                            ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT,
                            ncp_hl_ieee_addr_t(self.test_ieee_addr), TEST_DST_EP, 91)
            self.step = 10
#11
        elif self.step == 10:
            body = ncp_hl_bind_rsp_t.from_buffer_copy(rsp.body, 0)
            logger.debug("New binding id: {}".format(body.id))
            if body.id != 91:
                logger.error("Binding id is not 91")
            self.host.ncp_apsme_get_remote_bind_offset()
            self.step = 11
# 18
        elif self.step == 17:
            self.host.ncp_req_apsme_bind_request(ncp_hl_ieee_addr_t(self.ieee_addr), TEST_SRC_EP, 0x0006,
                            ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT,
                            ncp_hl_ieee_addr_t(self.test_ieee_addr), TEST_DST_EP, BIND_TBL_SIZE + 1)
            self.step = 18
        elif self.step == 18:
            logger.debug("Test End")


    def get_remote_bind_offset_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        offset = ncp_hl_uint8_t.from_buffer_copy(rsp.body, 0).uint8
        logger.debug("remote offset = {}".format(offset))
        if self.step == 11:
            if offset != 0xff:
                logger.error("Offset is incorrect")
            self.host.ncp_apsme_set_remote_bind_offset(REMOTE_BIND_OFFSET)
            self.step = 12
# 14
        elif self.step == 13:
            if offset != REMOTE_BIND_OFFSET:
                logger.error("Offset is in incorrect")
            self.host.ncp_apsme_rm_bind_entry_by_id(90)
            self.step = 14

    def set_remote_bind_offset_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
        if self.step == 12:
            self.host.ncp_apsme_get_remote_bind_offset()
            self.step = 13

    def get_bind_entry_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)
# 16
        if self.step == 15:
            self.host.ncp_apsme_rm_bind_entry_by_id(91)
            self.step = 16

    def clear_bind_table_rsp(self, rsp, rsp_len):
        self.rsp_log(rsp)

        if self.step == 6:
            self.host.ncp_req_apsme_bind_request(ncp_hl_ieee_addr_t(self.ieee_addr), TEST_SRC_EP, 0x0003,
                                                ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT,
                                                ncp_hl_ieee_addr_t(self.test_ieee_addr), TEST_DST_EP, 0)
            self.step = 7


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
