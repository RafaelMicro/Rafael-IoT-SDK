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
# PURPOSE: Trivial Host-side test for nsng shared library.
#
# start nsng, load .so, wait for NCP process connect, send one request, read resp

import sys
import threading
from enum import IntEnum
from ctypes import *
import logging

logger = logging.getLogger(__name__)


# HL protocol headers

# HL protocol packet header
class ncp_hl_header_t(Structure):
    _pack_ = 1
    _fields_ = [("version", c_ubyte),
                ("control", c_ubyte),
                ("call_id", c_ushort)]

# HL request header
class ncp_hl_request_header_t(Structure):
    _pack_ = 1
    _fields_ = [("version", c_ubyte),
                ("control", c_ubyte),
                ("call_id", c_ushort),
                ("tsn", c_ubyte)]


# union to decode a resp body
class ncp_hl_body_u(Union):
    _pack_ = 1
    _fields_ = [("arr", c_ubyte * 200),
                ("uint32", c_uint)]

class ncp_hl_request_t(Structure):
    _pack_ = 1
    _fields_ = [("hdr", ncp_hl_request_header_t),
                ("body", c_ubyte * 200)]

# Resp header
class ncp_hl_response_header_t(Structure):
    _pack_ = 1
    _fields_ = [("version", c_ubyte),
                ("control", c_ubyte),
                ("call_id", c_ushort),
                ("tsn", c_ubyte),
                ("status_category", c_ubyte),
                ("status_code", c_ubyte)]

class ncp_hl_response_t(Structure):
    _pack_ = 1
    _fields_ = [("hdr", ncp_hl_response_header_t),
                ("body", ncp_hl_body_u)]
    
class ncp_hl_indication_t(Structure):
    _pack_ = 1
    _fields_ = [("version", c_ubyte),
                ("control", c_ubyte),
                ("call_id", c_ushort),
                ("body", c_ubyte * 200)]
    
class ncp_hl_uint8_uint32_t(Structure):
    _pack_ = 1
    _fields_ = [("uint8", c_ubyte),
                ("uint32", c_uint)]

class ncp_hl_uint8_t(Structure):
    _pack_ = 1
    _fields_ = [("uint8", c_ubyte)]

class ncp_hl_uint32_t(Structure):
    _pack_ = 1
    _fields_ = [("uint32", c_uint)]

class ncp_hl_8b_t(Structure):
    _pack_ = 1
    _fields_ = [("b8", c_ubyte * 8)]

class ncp_hl_formation_t(Structure):
    _pack_ = 1
    _fields_ = [("npages", c_ubyte),
                ("page", c_ubyte),
                ("ch_mask", c_uint),
                ("scan_dur", c_ubyte),
                ("distributed", c_ubyte),
                ("distributed_addr", c_ushort)]

class ncp_hl_dev_annce_t(Structure):
    _pack_ = 1
    _fields_ = [("short_addr", c_ushort),
                ("long_addr", c_ubyte * 8),
                ("capability", c_ubyte)]

class ncp_hl_data_ind_t(Structure):
    _pack_ = 1
    _fields_ = [("param_len", c_ubyte),
                ("data_len", c_ushort),
                # Parameters - see zb_apsde_data_indication_t
                ("fc", c_ubyte),
                ("src_addr", c_ushort),
                ("dst_addr", c_ushort),
                ("group_addr", c_ushort),
                ("dst_endpoint", c_ubyte),
                ("src_endpoint", c_ubyte),
                ("clusterid", c_ushort),
                ("profileid", c_ushort),
                ("aps_counter", c_ubyte),
                ("mac_src_addr", c_ushort),
                ("mac_dst_addr", c_ushort),
                ("lqi", c_ubyte),
                ("rssi", c_ubyte),
                ("key_flags", c_ubyte),
                ("data", c_ubyte * 128)]


class ncp_hl_addr_t(Union):
    _pack_ = 1
    _fields_ = [("long_addr", c_ubyte * 8),
                ("short_addr", c_ushort)]

# Data req parameters - see zb_apsde_data_req_t
class ncp_hl_data_req_param_t(Structure):
    _pack_ = 1
    _fields_ = [("dst_addr", ncp_hl_addr_t),
                ("profileid", c_ushort),
                ("clusterid", c_ushort),
                ("dst_endpoint", c_ubyte),
                ("src_endpoint", c_ubyte),
                ("radius", c_ubyte),
                ("addr_mode", c_ubyte),
# TX options bitmask:
#                                  0x01 = Security enabled transmission
#                                  0x02 = Use NWK key (obsolete)
#                                  0x04 = Acknowledged transmission
#                                  0x08 = Fragmentation permitted.
                ("tx_options", c_ubyte),
                ("use_alias", c_ubyte),
                ("alias_src_addr", c_ushort),
                ("alias_seq_num", c_ubyte)]
    
class ncp_hl_data_req_t(Structure):
    _pack_ = 1
    _fields_ = [("param_len", c_ubyte),
                ("data_len", c_ushort),
                ("params", ncp_hl_data_req_param_t),
                ("data", c_ubyte * 128)]
    

class ncp_hl_apsde_data_conf_t(Structure):
    _pack_ = 1
    _fields_ = [("dst_addr", ncp_hl_addr_t),
                ("dst_endpoint", c_ubyte),
                ("src_endpoint", c_ubyte),
                ("status", c_ushort),
                ("tx_time", c_uint),
                ("addr_mode", c_ubyte)]
    
class ncl_hl_pkt_type_e(IntEnum):
    NCP_HL_REQUEST      = 0
    NCP_HL_RESPONSE     = 1
    NCP_HL_INDICATION   = 2

class ncp_hl_call_category_interval_e(IntEnum):
    NCP_HL_CATEGORY_INTERVAL = 0x100

class ncp_hl_ver_e(IntEnum):
    NCP_HL_VERSION = 0

class ncp_hl_call_category_e(IntEnum):
    NCP_HL_CATEGORY_CONFIGURATION = 0
    NCP_HL_CATEGORY_AF            = ncp_hl_call_category_interval_e.NCP_HL_CATEGORY_INTERVAL,
    NCP_HL_CATEGORY_ZDO           = ncp_hl_call_category_interval_e.NCP_HL_CATEGORY_INTERVAL * 2
    NCP_HL_CATEGORY_APS           = ncp_hl_call_category_interval_e.NCP_HL_CATEGORY_INTERVAL * 3
    NCP_HL_CATEGORY_NWKMGMT       = ncp_hl_call_category_interval_e.NCP_HL_CATEGORY_INTERVAL * 4
    NCP_HL_CATEGORY_SECUR         = ncp_hl_call_category_interval_e.NCP_HL_CATEGORY_INTERVAL * 5
    NCP_HL_CATEGORY_MANUF_TEST    = ncp_hl_call_category_interval_e.NCP_HL_CATEGORY_INTERVAL * 6
    NCP_HL_CATEGORY_OTA           = ncp_hl_call_category_interval_e.NCP_HL_CATEGORY_INTERVAL * 7

class ncp_hl_call_code_e(IntEnum):
    NCP_HL_GET_MODULE_VERSION             = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 1
    NCP_HL_NCP_RESET                      = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 2
    NCP_HL_NCP_FACTORY_RESET              = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 3
    NCP_HL_GET_ZIGBEE_ROLE                = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 4
    NCP_HL_SET_ZIGBEE_ROLE                = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 5
    NCP_HL_GET_ZIGBEE_CHANNEL_MASK        = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 6
    NCP_HL_SET_ZIGBEE_CHANNEL_MASK        = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 7
    NCP_HL_GET_ZIGBEE_CHANNEL             = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 8
    NCP_HL_GET_PAN_ID                     = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 9
    NCP_HL_SET_PAN_ID                     = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 10
    NCP_HL_GET_LOCAL_IEEE_ADDR            = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 11
    NCP_HL_SET_LOCAL_IEEE_ADDR            = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 12
    NCP_HL_SET_TRACE                      = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 13
    NCP_HL_GET_KEEPALIVE_TIMEOUT          = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 14
    NCP_HL_SET_KEEPALIVE_TIMEOUT          = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 15
    NCP_HL_GET_TX_POWER                   = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 16
    NCP_HL_SET_TX_POWER                   = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 17
    NCP_HL_AF_ADD_EP                      = ncp_hl_call_category_e.NCP_HL_CATEGORY_AF + 1
    NCP_HL_AF_DEL_EP                      = ncp_hl_call_category_e.NCP_HL_CATEGORY_AF + 2
    NCP_HL_AF_SET_SIMPLE_DESC             = ncp_hl_call_category_e.NCP_HL_CATEGORY_AF + 3
    NCP_HL_AF_SET_NODE_DESC               = ncp_hl_call_category_e.NCP_HL_CATEGORY_AF + 4
    NCP_HL_AF_SET_POWER_DESC              = ncp_hl_call_category_e.NCP_HL_CATEGORY_AF + 5
    NCP_HL_ZDO_NWK_ADDR_REQ               = ncp_hl_call_category_e.NCP_HL_CATEGORY_ZDO + 1
    NCP_HL_ZDO_IEEE_ADDR_REQ              = ncp_hl_call_category_e.NCP_HL_CATEGORY_ZDO + 2
    NCP_HL_ZDO_POWER_DESC_REQ             = ncp_hl_call_category_e.NCP_HL_CATEGORY_ZDO + 3
    NCP_HL_ZDO_NODE_DESC_REQ              = ncp_hl_call_category_e.NCP_HL_CATEGORY_ZDO + 4
    NCP_HL_ZDO_SIMPLE_DESC_REQ            = ncp_hl_call_category_e.NCP_HL_CATEGORY_ZDO + 5
    NCP_HL_ZDO_ACTIVE_EP_REQ              = ncp_hl_call_category_e.NCP_HL_CATEGORY_ZDO + 6
    NCP_HL_ZDO_MATCH_DESC_REQ             = ncp_hl_call_category_e.NCP_HL_CATEGORY_ZDO + 7
    NCP_HL_ZDO_BIND_REQ                   = ncp_hl_call_category_e.NCP_HL_CATEGORY_ZDO + 8
    NCP_HL_ZDO_UNBIND_REQ                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_ZDO + 9
    NCP_HL_ZDO_MGMT_LEAVE_REQ             = ncp_hl_call_category_e.NCP_HL_CATEGORY_ZDO + 10
    NCP_HL_ZDO_PERMIT_JOINING_REQ         = ncp_hl_call_category_e.NCP_HL_CATEGORY_ZDO + 11
    NCP_HL_ZDO_DEV_ANNCE_IND              = ncp_hl_call_category_e.NCP_HL_CATEGORY_ZDO + 12
    NCP_HL_APSDE_DATA_REQ                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_APS + 1
    NCP_HL_APSME_BIND                     = ncp_hl_call_category_e.NCP_HL_CATEGORY_APS + 2
    NCP_HL_APSME_UNBIND                   = ncp_hl_call_category_e.NCP_HL_CATEGORY_APS + 3
    NCP_HL_APSME_ADD_GROUP                = ncp_hl_call_category_e.NCP_HL_CATEGORY_APS + 4
    NCP_HL_APSME_RM_GROUP                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_APS + 5
    NCP_HL_APSDE_DATA_IND                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_APS + 6
    NCP_HL_NWK_FORMATION                  = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 1
    NCP_HL_NWK_ACTIVE_SCAN                = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 2
    NCP_HL_NWK_ASSOCIATION                = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 3
    NCP_HL_NWK_REJOIN                     = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 4
    NCP_HL_NWK_PERMIT_JOINING             = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 5
    NCP_HL_NWK_GET_IEEE_BY_SHORT          = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 6
    NCP_HL_NWK_GET_SHORT_BY_IEEE          = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 7
    NCP_HL_NWK_GET_NEIGHBOR_BY_IEEE       = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 8
    NCP_HL_NWK_STARTED_IND                = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 8
    NCP_HL_NWK_JOINED_IND                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 10
    NCP_HL_NWK_LEAVE_IND                  = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 11
    NCP_HL_GET_ED_KEEPALIVE_TIMEOUT       = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 12
    NCP_HL_SET_ED_KEEPALIVE_TIMEOUT       = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 13
    NCP_HL_PIM_SET_FAST_POLL_INTERVAL     = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 14
    NCP_HL_PIM_SET_LONG_POLL_INTERVAL     = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 15
    NCP_HL_PIM_START_FAST_POLL            = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 16
    NCP_HL_PIM_START_LONG_POLL            = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 17
    NCP_HL_PIM_START_POLL                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 18
    NCP_HL_PIM_SET_ADAPTIVE_POLL          = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 19
    NCP_HL_SECUR_SET_LOCAL_IC             = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 1
    NCP_HL_SECUR_ADD_IC                   = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 2
    NCP_HL_SECUR_SET_LOCAL_CERT           = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 3
    NCP_HL_SECUR_ADD_CERT                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 4
    NCP_HL_SECUR_DEL_CERT                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 5
    NCP_HL_SECUR_START_KE                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 6
    NCP_HL_SECUR_START_PARTNER_LK         = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 7
    NCP_HL_SECUR_CHILD_KE_FINISHED_IND    = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 8
    NCP_HL_SECUR_PARTNER_LK_FINISHED_IND  = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 9

class ncp_hl_role_e(IntEnum):
    NCP_HL_ZC  = 0
    NCP_HL_ZR  = 1
    NCP_HL_ZED = 2

class ncp_hl_addr_mode_e(IntEnum):
    NCP_HL_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT = 0
    NCP_HL_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT = 1
    NCP_HL_ADDR_MODE_16_ENDP_PRESENT = 2
    NCP_HL_ADDR_MODE_64_ENDP_PRESENT = 3


class ncp_hl_tx_options_e(IntEnum):
    NCP_HL_TX_OPT_SECURITY_ENABLED = 1
    NCP_HL_TX_OPT_ACK_TX           = 4
    NCP_HL_TX_OPT_FRAG_PERMITTED   = 8
    NCP_HL_TX_OPT_INC_EXT_NONCE    = 10

class ncp_host:
    callme_cb_t = CFUNCTYPE(None)

    def py_callme_cb(self):
        logger.debug("py_callme_cb called. Wakeup.")
        self.callme = 1
        self.event.set()

    def __init__(self, so_name, rsp_switch, ind_switch):
        self.tsn = 0
        self.rsp = ncp_hl_response_t();
        self.received = c_uint(0)
        self.alarm = c_uint(0)
        self.rsp_switch = rsp_switch
        self.ind_switch = ind_switch
        self.event = threading.Event()
        self.nsng_lib = CDLL(so_name)
        self.c_callme_cb = self.callme_cb_t(self.py_callme_cb)
        self.nsng_lib.ncp_host_ll_proto_init(self.c_callme_cb)

    def inc_tsn(self):
        self.tsn = self.tsn + 1
        # Note: tsn 0xff is reserved for indications
        if self.tsn == 0xff:
            self.tsn = 0
        
    def run_ll_quant(self, send_pkt, send_pkt_size):
        if send_pkt is None:
            pkt = POINTER(c_char_p)()
        else:
            pkt = pointer(send_pkt)

        ret = self.nsng_lib.ncp_host_ll_quant(pkt, c_uint(send_pkt_size), pointer(self.rsp), sizeof(self.rsp), pointer(self.received), pointer(self.alarm))
        logger.info("ncp_host_ll_quant ret {} received {} alarm {}".format(ret, self.received, self.alarm))
        if self.received.value > 0:
            self.handle_ncp_pkt()

    def handle_ncp_pkt(self):
        if self.rsp.hdr.control == ncl_hl_pkt_type_e.NCP_HL_RESPONSE:
            logger.info("rx rsp: version {} control {} call_id {} tsn {} status_category {} status_code {}".format(
                self.rsp.hdr.version, self.rsp.hdr.control, self.rsp.hdr.call_id,
                self.rsp.hdr.tsn, self.rsp.hdr.status_category, self.rsp.hdr.status_code))
            func = self.rsp_switch.get(self.rsp.hdr.call_id)
            func(self.rsp, self.received.value)
        elif self.rsp.hdr.control == ncl_hl_pkt_type_e.NCP_HL_INDICATION:
            self.ind = ncp_hl_indication_t.from_buffer(self.rsp, 0)
            logger.info("rx ind: version {} control {} call_id {}".format(
                self.ind.version, self.ind.control, self.ind.call_id))
            func = self.ind_switch.get(self.ind.call_id)
            func(self.ind, self.received.value)
            pass

    def wait_for_ncp(self):
#        logger.debug("waiting for {} ms".format(self.alarm.value))
        self.event.wait(self.alarm.value / 1000.)
        self.event.clear()
#        logger.debug("waiting complete")

    def ncp_req_no_arg(self, req):
        self.inc_tsn()
        hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, req, self.tsn)
        self.run_ll_quant(hdr, sizeof(hdr))

    def ncp_req_uint8_arg(self, callid, arg):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, callid, self.tsn)
        body = ncp_hl_uint8_t.from_buffer(req.body, 0)
        body.uint8 = arg
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))
    
    def ncp_req_uint32_arg(self, callid, arg):
        self.inc_tsn()
        req = ncp_hl_request_t()
        erq.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, callid, self.tsn)
        body = ncp_hl_uint32_t.from_buffer(req.body, 0)
        body.uint32 = arg
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_8b_arg(self, callid, arg):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, callid, self.tsn)
        body = ncp_hl_8b_t.from_buffer(req.body, 0)
        raw_bytes = (c_ubyte * 8).from_buffer_copy(arg)
        body.b8 = raw_bytes
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

        # access to serial API
    def ncp_req_get_module_version(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION)
        
    def ncp_req_reset(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_NCP_RESET)

    def ncp_req_factory_reset(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_NCP_FACTORY_RESET)

    def ncp_req_get_zigbee_role(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_ROLE)

    def ncp_req_get_channel_mask(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL_MASK)

    def ncp_req_get_channel(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_GET_ZIGBEE_CHANNEL)

    def ncp_req_get_pan_id(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_GET_PAN_ID)

    def ncp_req_set_local_ieee_addr(self, addr):
        self.ncp_req_8b_arg(ncp_hl_call_code_e.NCP_HL_SET_LOCAL_IEEE_ADDR, addr)

    def ncp_req_get_keepalive_timeout(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_GET_KEEPALIVE_TIMEOUT)

    def ncp_req_get_tx_power(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_GET_TX_POWER)

    def ncp_req_set_zigbee_role(self, role):
        self.ncp_req_uint8_arg(ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE, role)

        # 1-st pri
    def ncp_req_set_zigbee_channel_mask(self, page, mask):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK, self.tsn)
        body = ncp_hl_uint8_uint32_t.from_buffer(req.body, 0)
        body.uint8 = page
        body.uint32 = mask
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_set_pan_id(self, panid):
        self.ncp_req_uint16_arg(ncp_hl_call_code_e.NCP_HL_SET_PAN_ID, panid)

    def ncp_req_set_keepalive_timeout(self, timeo):
        self.ncp_req_uint32_arg(ncp_hl_call_code_e.NCP_HL_SET_KEEPALIVE_TIMEOUT, timeo)

    def ncp_req_set_tx_power(self, pwr):
        self.ncp_req_uint32_arg(ncp_hl_call_code_e.NCP_HL_SET_TX_POWER, pwr)

    def ncp_req_add_ep(self, ep):
        self.ncp_req_uint8_arg(ncp_hl_call_code_e.NCP_HL_AF_ADD_EP, ep)

    def ncp_req_del_ep(self, ep):
        self.ncp_req_uint8_arg(ncp_hl_call_code_e.NCP_HL_AF_DEL_EP, ep)

        # ZC - done
    def ncp_req_nwk_formation(self, page, mask, scan_dur, distributed, distributed_addr):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_NWK_FORMATION, self.tsn)
        # channel list:
        # 1b n entries
        # [n] 1b page 4b mask
        #
        # 1b scan duration
        # 1b DistributedNetwork (0)
        # 2b DistributedNetworkAddress (0)
        body = ncp_hl_formation_t.from_buffer(req.body, 0)
        body.npages = 1
        body.page = page
        body.ch_mask = mask
        body.scan_dur = scan_dur
        # Actually that field is meaningless: netork type is already defined by device role.
        body.distributed = distributed
        body.distributed_addr = distributed_addr
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_nwk_permit_joining(self, arg):
        self.ncp_req_uint8_arg(ncp_hl_call_code_e.NCP_HL_NWK_PERMIT_JOINING, arg)

        # 1-st pri
    def ncp_req_active_scan(self, duration, page, mask):
        ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_NWK_ACTIVE_SCAN)

    def ncp_req_association(self, chan_page, channel, panid, short_addr, capability):
        ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_NWK_ASSOCIATION)

    def ncp_req_apsde_data_request(self, params, data, data_len):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ, self.tsn)
        body = ncp_hl_data_req_t.from_buffer(req.body, 0)
        body.param_len = sizeof(params)
        body.data_len = data_len
        body.params = params
        body.data = data
        logger.debug("apsde-data.req len {}".format(data_len))
        # 3 = sizeof param_len & data_len
        self.run_ll_quant(req, sizeof(req.hdr) + 3 + sizeof(body.params) + data_len)

#
# Test itself
#

CHANNEL_MASK = 0x800            # channel 11
MY_IEEE_ADDR = bytes([0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb])

class zdo_startup_zc_test:

    def __init__(self):
        self.rsp_switch = {
            ncp_hl_call_code_e.NCP_HL_GET_MODULE_VERSION : self.get_module_version_rsp,
            ncp_hl_call_code_e.NCP_HL_NCP_RESET : self.ncp_reset_resp,
            ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_CHANNEL_MASK : self.channel_mask_set,
            ncp_hl_call_code_e.NCP_HL_SET_ZIGBEE_ROLE : self.zigbee_role_set,
            ncp_hl_call_code_e.NCP_HL_NWK_FORMATION : self.formation_complete,
            ncp_hl_call_code_e.NCP_HL_NWK_PERMIT_JOINING : self.nwk_permit_joining_complete,
            ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ :self.data_conf
        }

        self.ind_switch = {
            ncp_hl_call_code_e.NCP_HL_ZDO_DEV_ANNCE_IND : self.dev_annce_ind,
            ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND : self.data_ind,
        }

    def run(self):
        self.host = ncp_host("../ncp_ll_nsng.so", self.rsp_switch, self.ind_switch)
        self.host.ncp_req_get_module_version()
        # main loop
        while True:
            self.host.wait_for_ncp()
            self.host.run_ll_quant(None, 0)
        pass
            
    def get_module_version_rsp(self, rsp, rsp_len):
        ver = rsp.body.uint32
        logger.info("NCP ver 0x%x", ver)
        self.host.ncp_req_set_local_ieee_addr(MY_IEEE_ADDR)

    def set_long_addr_done(self, rsp, rsp_len):
        logger.info("set_long_addr_done status %d", rsp.hdr.status_code)
        self.host.ncp_req_set_zigbee_channel_mask(0, CHANNEL_MASK)
        
    def ncp_reset_resp(self, rsp, rsp_len):
        logger.info("ncp_reset_resp status %d", rsp.hdr.status_code)

    def channel_mask_set(self, rsp, rsp_len):
        logger.info("channel mask set. status %d", rsp.hdr.status_code)
        self.host.ncp_req_set_zigbee_role(ncp_hl_role_e.NCP_HL_ZC)

    def zigbee_role_set(self, rsp, rsp_len):
        logger.info("zb role set. status %d", rsp.hdr.status_code)
        self.host.ncp_req_nwk_formation(0, CHANNEL_MASK, 5, 0, 0)

    def formation_complete(self, rsp, rsp_len):
        logger.info("Formation complete. status %d", rsp.hdr.status_code)
        self.host.ncp_req_nwk_permit_joining(0xfe)

    def nwk_permit_joining_complete(self, rsp, rsp_len):
        logger.info("nwk_permit_joining_complete status %d", rsp.hdr.status_code)

    def dev_annce_ind(self, ind, ind_len):
        annce = ncp_hl_dev_annce_t.from_buffer(ind.body, 0)
        logger.info("device annce: dev {:#x} ieee {} cap {:#x}".format(annce.short_addr, list(map(lambda x: hex(x), list(annce.long_addr))), annce.capability))

    def data_ind(self, ind, ind_len):
        dataind = ncp_hl_data_ind_t.from_buffer(ind.body, 0)
        logger.info("apsde.data.ind len {} fc {:#x} src {:#x}/{:#x} dst {:#x}/{:#x} group {:#x} src ep {} dst ep {} cluster {:#x} profile {:#x} apscnt {} lqi {} rssi {} keyfl {:#x}".format(
            dataind.data_len, dataind.fc, dataind.src_addr, dataind.mac_src_addr, dataind.dst_addr, dataind.mac_dst_addr, dataind.group_addr, dataind.src_endpoint, dataind.dst_endpoint,
            dataind.clusterid, dataind.profileid, dataind.aps_counter, dataind.lqi, dataind.rssi, dataind.key_flags))
        data4dump = list(dataind.data)[:(dataind.data_len-1)]
        logger.info("data {}".format(list(map(lambda x: hex(x), data4dump))))
        # Send packet back
        params = ncp_hl_data_req_param_t()
        params.dst_addr.short_addr = dataind.src_addr
        params.addr_mode = ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_16_ENDP_PRESENT
        params.profileid = dataind.profileid
        params.clusterid = dataind.clusterid
        params.dst_endpoint = dataind.src_endpoint
        params.src_endpoint = dataind.dst_endpoint
        params.radius = 30
        params.tx_options = ncp_hl_tx_options_e.NCP_HL_TX_OPT_SECURITY_ENABLED | ncp_hl_tx_options_e.NCP_HL_TX_OPT_ACK_TX
        params.use_alias = 0
        params.alias_src_addr = 0
        params.alias_seq_num = 0
        self.host.ncp_req_apsde_data_request(params, dataind.data, dataind.data_len)

    def data_conf(self, rsp, rsp_len):
        if rsp_len <= sizeof(rsp.hdr):
            logger.info("apsde-data.conf status {} {}".format(rsp.hdr.status_category, rsp.hdr.status_code))
        else:
            conf = ncp_hl_apsde_data_conf_t.from_buffer(rsp.body.arr, 0)
            # TODO: long/short addr
            logger.info("apsde-data.conf status {} addr_mode {} dst_addr {} src_endpoint {} dst_endpoint {} tx_time {} ".format(
                        conf.status, conf.addr_mode,
                        conf.dst_addr.short_addr if conf.addr_mode != ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT else list(map(lambda x: hex(x), list(conf.dst_addr.long_addr))),
                        conf.src_endpoint, conf.dst_endpoint, conf.tx_time))
        
class ncp_hl_apsde_data_conf_t(Structure):
    _pack_ = 1
    _fields_ = [("dst_addr", ncp_hl_addr_t),
                ("dst_endpoint", c_ubyte),
                ("src_endpoint", c_ubyte),
                ("status", c_ushort),
                ("tx_time", c_uint),
                ("addr_mode", c_ubyte)]

        
def main():

    logger = logging.getLogger(__name__)
    loggerFormat = '[%(levelname)8s : %(name)40s]\t%(asctime)s:\t%(message)s'
    loggerFormatter = logging.Formatter(loggerFormat)
    loggerLevel = logging.DEBUG
    logging.basicConfig(format=loggerFormat, level=loggerLevel)

    test = zdo_startup_zc_test()
    test.run()
    pass    


if __name__ == "__main__":
    main()
