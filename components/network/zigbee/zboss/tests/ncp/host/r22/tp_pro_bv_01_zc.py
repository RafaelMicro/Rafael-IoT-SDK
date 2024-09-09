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
# PURPOSE: Host-side test
#
# tp_pro_bv_01 test in ZC role, Host side.

from host.base_test import *
from host.ncp_hl import *

logger = logging.getLogger(__name__)

#
# Test itself
#

class Test(BaseTest):

    def __init__(self, channel_mask):
        PAN_ID = 0x1aaa
        super().__init__(channel_mask, ncp_hl_role_e.NCP_HL_ZC, PAN_ID)


    def begin(self):
        self.host.ncp_req_nwk_formation(0, self.channel_mask, 5, 0, 0)


    def on_dev_annce_ind(self, annce):
        self.host.ncp_req_nwk_get_ieee_by_short(annce.short_addr)
