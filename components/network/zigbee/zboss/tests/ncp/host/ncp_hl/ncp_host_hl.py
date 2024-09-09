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
# PURPOSE: NCP HL protocol library for Host-side Python tests
#

import sys
import threading
import platform
import inspect
from enum import IntEnum, unique
from ctypes import *
import re
import logging

logger = logging.getLogger(__name__)

def ncp_on_nsng():
    """Detect if we are running on a PC using a network simulator."""
    return platform.machine() == "x86_64"

def ncp_hl_dump(data, fmtspec = "02x", sep = ":"):
    return sep.join(format(x, fmtspec) for x in data)

def mkiterable(obj):
    try:
        return iter(obj)
    except:
        return (obj,)

def as_enum(value, cls):
    try:
        return cls(value).name
    except:
        return "#E-UNDEFINED({})".format(value)

class ncp_hl_struct_t(Structure):
    """A base class for all NCP HL structures.

    This class enables to automatically format Structure objects
    to a string by iterating over 'self._fields_' member and printing
    each defined structure field with the format provided in the
    'self._formats_' dictionary. If the dictionary does not contain
    a record for a particular field then use default formatting for it.
    """
    _formats_ = {}

    def __format__(self, fmtspec):

        del fmtspec # unused

        def fmtfield(self, field, named):
            prefix = "{}: ".format(field) if named else ""
            value = getattr(self, field)
            iterable = mkiterable(value)
            fmt = self._formats_.get(field, "")
            return "{}{}".format(prefix, ncp_hl_dump(iterable, fmt))

        # Print the field name only if there is more than one
        # field in the structure
        multiple = len(self._fields_) > 1

        result = ", ".join(fmtfield(self, field, multiple) for field, _ in self._fields_)
        return "S{{ {} }}".format(result)

class ncp_hl_empty_t(ncp_hl_struct_t):
    def __format__(self, fmtspec):
        return "<<empty>>"

# HL protocol headers

# HL protocol packet header
class ncp_hl_header_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("version", c_ubyte),
                ("control", c_ubyte),
                ("call_id", c_ushort)]

# HL request header
class ncp_hl_request_header_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("version", c_ubyte),
                ("control", c_ubyte),
                ("call_id", c_ushort),
                ("tsn", c_ubyte)]


# union to decode a resp body
class ncp_hl_body_u(Union):
    _pack_ = 1
    _fields_ = [("arr", c_ubyte * 8192),
                ("uint32", c_uint)]

class ncp_hl_request_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("hdr", ncp_hl_request_header_t),
                ("body", c_ubyte * 8192)]

    def body_as(self, cls):
        return cls.from_buffer(self.body)

# Resp header
class ncp_hl_response_header_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("version", c_ubyte),
                ("control", c_ubyte),
                ("call_id", c_ushort),
                ("tsn", c_ubyte),
                ("status_category", c_ubyte),
                ("status_code", c_ubyte)]

class ncp_hl_response_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("hdr", ncp_hl_response_header_t),
                ("body", ncp_hl_body_u)]

class ncp_hl_indication_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("version", c_ubyte),
                ("control", c_ubyte),
                ("call_id", c_ushort),
                ("body", c_ubyte * 8192)]

class ncp_hl_uint8_uint32_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("uint8", c_ubyte),
                ("uint32", c_uint)]

class ncp_hl_uint16_uint8_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("uint16", c_ushort),
                ("uint8", c_ubyte)]

class ncp_hl_int8_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("int8", c_byte)]

class ncp_hl_uint8_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("uint8", c_ubyte)]

class ncp_hl_uint16_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("uint16", c_ushort)]

class ncp_hl_uint32_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("uint32", c_uint)]

class ncp_hl_status_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("status_category", c_ubyte),
                ("status_code", c_ubyte)]

class ncp_hl_8b_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("b8", c_ubyte * 8)]

class ncp_hl_ieee_addr_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("b8", c_ubyte * 8)]
    _formats_ = {"b8": "02x"}

    def __init__(self, *args):
        b8_type = type(self.b8)
        if len(args) == 1:
            arg = args[0]
            if type(arg) is b8_type:
                super().__init__(arg)
            elif type(arg) is ncp_hl_ieee_addr_t:
                super().__init__(arg.b8)
            else:
                super().__init__(b8_type(*arg))
        else:
            # Support ncp_hl_ieee_addr_t(0x00, 0x01, 0x02, 0x03, 0x03, 0x05, 0x06, 0x07)
            super().__init__(b8_type(*args))

    def __format__(self, fmtspec):
        return "{{{}}}".format(ncp_hl_dump(reversed(self.b8)))

class ncp_hl_nwk_addr_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("u16", c_ushort)]
    _formats_ = {"u16": "#06x"}


class ncp_hl_addr_u(Union):
    _pack_ = 1
    _fields_ = [("long_addr", ncp_hl_ieee_addr_t),
                ("short_addr", ncp_hl_nwk_addr_t)]

class ncp_hl_module_ver_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("u32", c_ulong)]
    _formats_ = {"u32": "#010x"}

class ncp_hl_channel_list_entry_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("page", c_ubyte),
                ("ch_mask", c_uint)]
    _formats_ = {"ch_mask": "#010x"}

class ncp_hl_channel_list_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("count", c_ubyte),
                ("entries", (32 * ncp_hl_channel_list_entry_t))]
    def __format__(self, fmtspec):
        elist = "; ".join(map(format, self.entries[:self.count]))
        return "npages {} [{}]".format(self.count, elist)

class ncp_hl_channel_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("ch_num", c_ubyte)]

    # NCP Serial Protocol Spec 3.5.8.1 describes the following structure:
    #_fields_ = [("page", c_ubyte),
    #            ("ch_num", c_ubyte)]

class ncp_hl_ext_panid_t(ncp_hl_ieee_addr_t):
    pass

class ncp_hl_pan_id_t(ncp_hl_nwk_addr_t):
    pass

class ncp_hl_local_addr_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("mac_if", c_ubyte),
                ("long_addr", ncp_hl_ieee_addr_t)]

class ncp_hl_keepalive_to_t(ncp_hl_uint32_t):
    pass

class ncp_hl_ed_to_t(ncp_hl_uint8_t):
    pass

class ncp_hl_formation_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("npages", c_ubyte),
                ("page", c_ubyte),
                ("ch_mask", c_uint),
                ("scan_dur", c_ubyte),
                ("distributed", c_ubyte),
                ("distributed_addr", c_ushort)]

class ncp_hl_dev_annce_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("short_addr", c_ushort),
                ("long_addr", c_ubyte * 8),
                ("capability", c_ubyte)]
    _formats_ = {"short_addr": "#06x",
                 "long_addr": "02x",
                 "capability": "#04x"}

class ncp_hl_data_ind_t(ncp_hl_struct_t):
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
    def __format__(self, fmtspec):
        return "data len {} fc {:#x} src {:#x}/{:#x} dst {:#x}/{:#x} group {:#x} src ep {} dst ep {} cluster {:#x} profile {:#x} apscnt {} lqi {} rssi {} keyfl {:#x}".format(
            self.data_len, self.fc, self.src_addr, self.mac_src_addr, self.dst_addr, self.mac_dst_addr,
            self.group_addr, self.src_endpoint, self.dst_endpoint,
            self.clusterid, self.profileid, self.aps_counter, self.lqi, self.rssi, self.key_flags)

class ncp_hl_rx_pkt_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("data_len", c_ushort),
                ("lqi", c_ubyte),
                ("rssi", c_byte),
                ("data", c_ubyte * 128)]

# TODO: Replace with ncp_hl_addr_u
class ncp_hl_addr_t(Union):
    _pack_ = 1
    _fields_ = [("long_addr", c_ubyte * 8),
                ("short_addr", c_ushort)]

class ncp_hl_set_local_ieee_req_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("mac_if", c_ubyte),
                ("long_addr", ncp_hl_ieee_addr_t)]


# Data req parameters - see zb_apsde_data_req_t
class ncp_hl_data_req_param_t(ncp_hl_struct_t):
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

class ncp_hl_data_req_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("param_len", c_ubyte),
                ("data_len", c_ushort),
                ("params", ncp_hl_data_req_param_t),
                ("data", c_ubyte * 128)]

class ncp_hl_apsde_data_conf_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("dst_addr", ncp_hl_addr_t),
                ("dst_endpoint", c_ubyte),
                ("src_endpoint", c_ubyte),
                ("tx_time", c_uint),
                ("addr_mode", c_ubyte)]
    def __format__(self, fmtspec):
        return "addr_mode {} dst_addr {} src_endpoint {} dst_endpoint {} tx_time {}".format(
            self.addr_mode,
            ncp_hl_nwk_addr_t(self.dst_addr.short_addr)
                if self.addr_mode != ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT
                else ncp_hl_ieee_addr_t(self.dst_addr.long_addr),
            self.src_endpoint, self.dst_endpoint, self.tx_time)

class ncp_hl_apsme_bind_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("src_addr", ncp_hl_ieee_addr_t),
                ("src_endpoint", c_ubyte),
                ("clusterid", c_ushort),
                ("dst_addr_mode", c_ubyte),
                ("dst_addr", ncp_hl_addr_u),
                ("dst_endpoint", c_ubyte)]

    def fill(self, src_addr, src_endpoint, clusterid, dst_addr_mode, dst_addr, dst_endpoint):
        self.src_addr = src_addr
        self.src_endpoint = src_endpoint
        self.clusterid = clusterid
        self.dst_addr_mode = dst_addr_mode
        if dst_addr_mode == ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT:
            self.dst_addr.short_addr = dst_addr
        elif dst_addr_mode == ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT:
            self.dst_addr.long_addr = dst_addr
        self.dst_endpoint = dst_endpoint

class ncp_hl_apsme_group_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("group_addr", ncp_hl_nwk_addr_t),
               ("endpoint", c_ubyte)]

class ncp_hl_device_type_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("u8", c_ubyte)]

    def __format__(self, fmtspec):
        return as_enum(self.u8, ncp_hl_device_type_e)

class ncp_hl_relationship_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("u8", c_ubyte)]

    def __format__(self, fmtspec):
        return as_enum(self.u8, ncp_hl_relationship_e)

class ncp_hl_get_neighbor_by_ieee_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("ieee_addr", c_ubyte * 8),
                ("short_addr", c_ushort),
                ("device_type", c_ubyte),
                ("rx_on_when_idle", c_ubyte),
                ("ed_config", c_ushort),
                ("timeout_counter", c_uint),
                ("device_timeout", c_uint),
                ("relationship", c_ubyte),
                ("transmit_failure_cnt", c_ubyte),
                ("lqi", c_ubyte),
                ("outgoing_cost", c_ubyte),
                ("age", c_ubyte),
                ("keepalive_received", c_ubyte),
                ("mac_iface_idx", c_ubyte)]
    def __format__(self, fmtspec):
        return "ieee {} short {} dev_type {} rxonidle {} ed_cfg {} to_cnt {} dev_to {} rel {} tx_fail_cnt {} lqi {} outg_cost {} age {} keepalive_rec {} mac_if {}".format(
                ncp_hl_ieee_addr_t(self.ieee_addr),
                ncp_hl_nwk_addr_t(self.short_addr),
                ncp_hl_device_type_t(self.device_type),
                bool(self.rx_on_when_idle),
                self.ed_config,
                self.timeout_counter,
                self.device_timeout,
                ncp_hl_relationship_t(self.relationship),
                self.transmit_failure_cnt,
                self.lqi,
                self.outgoing_cost,
                self.age,
                bool(self.keepalive_received),
                self.mac_iface_idx)

class ncp_hl_nwk_discovery_req_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("npages", c_ubyte),
                ("page", c_ubyte),
                ("ch_mask", c_uint),
                ("scan_dur", c_ubyte)]

class ncp_hl_nwk_discovery_dsc_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("extpanid", ncp_hl_ext_panid_t),
                ("panid", ncp_hl_pan_id_t),
                ("nwk_update_id", c_ubyte),
                ("page", c_ubyte),
                ("channel", c_ubyte),
                ("flags", c_ubyte)]

class ncp_hl_nwk_discovery_rsp_hdr_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("network_count", c_ubyte)]

class ncp_hl_nwk_nlme_join_req_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("extpanid", ncp_hl_ext_panid_t),
                ("rejoin", c_ubyte),
                ("npages", c_ubyte),
                ("page", c_ubyte),
                ("ch_mask", c_uint),
                ("scan_dur", c_ubyte),
                ("capability", c_ubyte),
                ("secur_enable", c_ubyte)]

class ncp_hl_nwk_nlme_join_rsp_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("short_addr", ncp_hl_nwk_addr_t),
                ("extpanid", ncp_hl_ieee_addr_t),
                ("page", c_ubyte),
                ("channel", c_ubyte),
                ("enh_beacon", c_ubyte),
                ("mac_iface_idx", c_ubyte)]

@unique
class ncl_hl_zdo_addr_req_type_e(IntEnum):
    NCP_HL_ZDO_SINGLE_DEV_REQ  = 0
    NCP_HL_ZDO_EXTENDED_REQ     = 1

class ncp_hl_zdo_nwk_addr_req_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("dst_addr", ncp_hl_nwk_addr_t),
                ("ieee_addr", ncp_hl_ieee_addr_t),
                ("request_type", c_ubyte),
                ("start_index", c_ubyte)]

class ncp_hl_zdo_ieee_addr_req_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("dst_addr", ncp_hl_nwk_addr_t),
                ("nwk_addr", ncp_hl_nwk_addr_t),
                ("request_type", c_ubyte),
                ("start_index", c_ubyte)]

class ncp_hl_zdo_addr_rsp_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("ieee_addr_remote_dev", ncp_hl_ieee_addr_t),
                ("short_addr_remote_dev", ncp_hl_nwk_addr_t),
                ("num_assoc_dev", c_ubyte),
                ("start_index", c_ubyte),
                ("nwk_addr_assoc_dev_list", ncp_hl_nwk_addr_t * 64)] # should be 256 entries but max body size is 200 bytes

    def __format__(self, fmtspec):
        def format_alist(alist, count):
            return ncp_hl_dump(alist[:count], "#06x")

        result = "ieee_addr_remote_dev {} short_addr_remote_dev {}".format(self.ieee_addr_remote_dev, self.short_addr_remote_dev)
        if fmtspec == 'e':
            result += " num_assoc_dev {}".format(self.num_assoc_dev)
            if (self.num_assoc_dev != 0):
                alist = format_alist(self.nwk_addr_assoc_dev_list, self.num_assoc_dev)
                result += " start_index {} nwk_addr_assoc_dev_list [{}]".format(self.start_index, alist)
        return result

class ncp_hl_zdo_power_desc_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("power_desc_flags", c_ushort)]
    _formats_ = {"power_desc_flags": "#06x"}

class ncp_hl_zdo_node_desc_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("node_desc_flags", c_ushort),
                ("mac_capability_flags", c_ubyte),
                ("manufacturer_code", c_ushort),
                ("max_buf_size", c_ubyte),
                ("max_incoming_transfer_size", c_ushort),
                ("server_mask", c_ushort),
                ("max_outgoing_transfer_size", c_ushort),
                ("desc_capability_field", c_ubyte)]
    _formats_ = {"node_desc_flags": "#04x",
                 "mac_capability_flags": "04x",
                 "manufacturer_code": "#06x",
                 "server_mask": "#06x",
                 "desc_capability_field": "#04x"}

class ncp_hl_set_simple_desc_hdr_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("endpoint", c_ubyte),
                ("profile_id", c_ushort),
                ("device_id", c_ushort),
                ("device_version", c_ubyte),
                ("in_clu_count", c_ubyte),
                ("out_clu_count", c_ubyte)]

class ncp_hl_set_simple_desc_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("hdr", ncp_hl_set_simple_desc_hdr_t),
                ("clusters", c_ushort * 30)]

def ncp_hl_make_simple_desc(ep_num, profile_id, dev_id, dev_ver, cl_list_in, cl_list_out):
    desc = ncp_hl_set_simple_desc_t()
    desc.hdr.endpoint = ep_num
    desc.hdr.profile_id = profile_id
    desc.hdr.device_id = dev_id
    desc.hdr.device_version = dev_ver
    desc.hdr.in_clu_count = len(cl_list_in)
    desc.hdr.out_clu_count = len(cl_list_out)
    desc.clusters = (c_ushort * len(desc.clusters))(*cl_list_in, *cl_list_out)
    return desc

class ncp_hl_active_ep_desc_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("active_ep_cnt", c_ubyte),
                ("active_ep_list", c_ubyte * 128)]

class ncp_hl_match_desc_hdr_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("addr_of_interest", c_ushort),
                ("profile_id", c_ushort),
                ("in_clu_count", c_ubyte),
                ("out_clu_count", c_ubyte)]

class ncp_hl_match_desc_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("hdr", ncp_hl_match_desc_hdr_t),
                ("clusters", c_ushort * 30)]

class ncp_hl_match_desc_rsp_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("match_len", c_ubyte),
                ("match_list", c_ubyte * 128)]

@unique
class ncp_hl_addr_mode_e(IntEnum):
    NCP_HL_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT = 0
    NCP_HL_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT = 1
    NCP_HL_ADDR_MODE_16_ENDP_PRESENT = 2
    NCP_HL_ADDR_MODE_64_ENDP_PRESENT = 3

class ncp_hl_bind_req_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("target_short_addr", ncp_hl_nwk_addr_t),
                ("src_long_addr", ncp_hl_ieee_addr_t),
                ("src_endpoint", c_ubyte),
                ("cluster_id", c_ushort),
                ("dst_addr_mode", c_ubyte),
                ("dst_addr", ncp_hl_addr_u),
                ("dst_endpoint", c_ubyte)]

    def fill_common(self, target_addr, src_addr, src_endp, cluster_id, dst_addr_mode):
        self.target_short_addr = target_addr
        self.src_long_addr = src_addr
        self.src_endpoint = src_endp
        self.cluster_id = cluster_id
        self.dst_addr_mode = dst_addr_mode

    def fill_grp(self, target_addr, src_addr, src_endp, cluster_id, grp_addr):
        self.fill_common(target_addr, src_addr, src_endp, cluster_id,
                ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT)
        self.dst_addr.short_addr = grp_addr
        self.dst_endpoint = 0

    def fill_ep(self, target_addr, src_addr, src_endp, cluster_id, dst_addr, dst_endp):
        self.fill_common(target_addr, src_addr, src_endp, cluster_id,
                ncp_hl_addr_mode_e.NCP_HL_ADDR_MODE_64_ENDP_PRESENT)
        self.dst_addr.long_addr = dst_addr
        self.dst_endpoint = dst_endp

class NCP_HL_LEAVE_FLAGS(Union):
    REMOVE_CHILDREN = (1 << 6)
    REJOIN = (1 << 7)

class ncp_hl_mgmt_leave_req_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("dst_addr", ncp_hl_nwk_addr_t),
                ("device_addr", ncp_hl_ieee_addr_t),
                ("leave_flags", c_ubyte)]

class ncp_hl_mgmt_permit_joining_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("dst_addr", ncp_hl_nwk_addr_t),
                ("duration", c_ubyte)]

# 1b suite (KEC_CS1 - 1, KEC_CS2 - 2)
# 22 (CS1) or 37 (CS2) b ca_public_key
# 48 or 74 b certificate
# 21 or 36 b private_key

class ncp_hl_secur_add_cert_cs1_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("cs_type", c_ubyte),
                ("ca_public_key", c_ubyte * 22),
                ("certificate", c_ubyte * 48),
                ("private_key", c_ubyte * 21)]

class ncp_hl_secur_add_cert_cs2_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("cs_type", c_ubyte),
                ("ca_public_key", c_ubyte * 37),
                ("certificate", c_ubyte * 74),
                ("private_key", c_ubyte * 36)]

class ncp_hl_secur_del_cert_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("suite", c_ubyte),
                ("issuer", c_ubyte * 8),
                ("ieee_addr", ncp_hl_ieee_addr_t)]

    def fill(self, suite, issuer, ieee_addr):
        self.suite = suite
        memmove(self.issuer, issuer, len(issuer))
        self.ieee_addr = ieee_addr

class ncp_hl_secur_child_cbke_ind_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("status_category", c_ubyte),
                ("status_code", c_ubyte),
                ("short_addr", c_ushort),
                ("long_addr", c_ubyte * 8)]

class ncp_hl_af_set_node_desc_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("device_type", c_ubyte),
               ("mac_cap", c_ubyte),
               ("manufacturer_code", c_ushort)]

class ncp_hl_af_set_power_desc_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("cur_pwr_mode", c_ubyte),
               ("available_pwr_srcs", c_ubyte),
               ("cur_pwr_src", c_ubyte),
               ("cur_pwr_src_lvl", c_ubyte)]

class ncp_hl_nwk_leave_ind_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("ieee_addr", ncp_hl_ieee_addr_t),
                ("rejoin", c_ubyte)]

class ncp_hl_nwk_pan_id_conflict_ind_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("panid_count", c_ushort),
                ("panids", c_ushort * 32)]

class ncp_hl_nwk_key_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("nwk_key", c_ubyte * 16),
                ("key_number", c_ubyte)]

class ncp_hl_zdo_rejoin_req_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("extpanid", ncp_hl_ext_panid_t),
                ("mask", c_uint),
                ("secure_rejoin", c_ubyte)]

class ncp_hl_ic_rsp_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("ic_len", c_ubyte),
                ("ic", c_ubyte * 18)]

class ncp_hl_serial_number_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("serial", c_ubyte * 16 )]

class ncp_hl_payload_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("len", c_ubyte),
                ("data", c_ubyte * 127 )]

class ncp_hl_ota_fw_payload_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("len", c_ushort),
                ("data", c_ubyte * 8000 )]

class ncp_hl_big_pkt_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("len", c_ushort),
                ("data", c_ubyte * 8183 )] # size of data excepting size of header and len

class ncp_hl_nwk_keys_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("nwk_key", ncp_hl_nwk_key_t * 3 )]

    def __format__(self, fmtspec):
        res = ""
        for i in range(3):
           res += "nwk_key {} seq {} ".format(ncp_hl_dump(self.nwk_key[i].nwk_key),
                                                  self.nwk_key[i].key_number)
        return res

class ncp_hl_aps_key_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("key", c_ubyte * 16 )]

    def __format__(self, fmtspec):
        return "aps_key {}".format(ncp_hl_dump(self.key))

@unique
class ncp_hl_device_type_e(IntEnum):
    ZC  = 0
    ZR  = 1
    ZED = 2
    UNKNOWN = 3

@unique
class ncp_hl_relationship_e(IntEnum):
    PARENT          = 0
    CHILD           = 1
    SIBLING         = 2
    NONE            = 3
    PREV_CHILD      = 4
    UNAUTH_CHILD    = 5

# TODO: use IntFlag as a base class
@unique
class ncl_hl_mac_capability_e(IntEnum):
    NCP_HL_CAP_DEVICE_TYPE      = 1<<1 # 1 if !zed
    NCP_HL_CAP_POWER_SOURCE     = 1<<2 # 1 if mains powered
    NCP_HL_CAP_RX_ON_WHEN_IDLE  = 1<<3
    NCP_HL_CAP_ALLOCATE_ADDRESS = 1<<7
    NCP_HL_CAP_ROUTER           = NCP_HL_CAP_DEVICE_TYPE|NCP_HL_CAP_POWER_SOURCE|NCP_HL_CAP_RX_ON_WHEN_IDLE

@unique
class ncl_hl_pkt_type_e(IntEnum):
    NCP_HL_REQUEST      = 0
    NCP_HL_RESPONSE     = 1
    NCP_HL_INDICATION   = 2

class ncp_hl_call_category_interval_e(IntEnum):
    NCP_HL_CATEGORY_INTERVAL = 0x100

@unique
class ncp_hl_ver_e(IntEnum):
    NCP_HL_VERSION = 0

@unique
class ncp_hl_call_category_e(IntEnum):
    NCP_HL_CATEGORY_CONFIGURATION = 0
    NCP_HL_CATEGORY_AF            = ncp_hl_call_category_interval_e.NCP_HL_CATEGORY_INTERVAL,
    NCP_HL_CATEGORY_ZDO           = ncp_hl_call_category_interval_e.NCP_HL_CATEGORY_INTERVAL * 2
    NCP_HL_CATEGORY_APS           = ncp_hl_call_category_interval_e.NCP_HL_CATEGORY_INTERVAL * 3
    NCP_HL_CATEGORY_NWKMGMT       = ncp_hl_call_category_interval_e.NCP_HL_CATEGORY_INTERVAL * 4
    NCP_HL_CATEGORY_SECUR         = ncp_hl_call_category_interval_e.NCP_HL_CATEGORY_INTERVAL * 5
    NCP_HL_CATEGORY_MANUF_TEST    = ncp_hl_call_category_interval_e.NCP_HL_CATEGORY_INTERVAL * 6
    NCP_HL_CATEGORY_OTA           = ncp_hl_call_category_interval_e.NCP_HL_CATEGORY_INTERVAL * 7

@unique
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
    NCP_HL_GET_RX_ON_WHEN_IDLE            = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 18
    NCP_HL_SET_RX_ON_WHEN_IDLE            = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 19
    NCP_HL_GET_JOINED                     = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 20
    NCP_HL_GET_AUTHENTICATED              = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 21
    NCP_HL_GET_ED_TIMEOUT                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 22
    NCP_HL_SET_ED_TIMEOUT                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 23
    NCP_HL_ADD_VISIBLE_DEV                = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 24
    NCP_HL_ADD_INVISIBLE_SHORT            = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 25
    NCP_HL_RM_INVISIBLE_SHORT             = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 26
    NCP_HL_SET_NWK_KEY                    = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 27
    NCP_HL_GET_SERIAL_NUMBER              = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 28
    NCP_HL_GET_VENDOR_DATA                = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 29
    NCP_HL_GET_NWK_KEYS                   = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 30
    NCP_HL_GET_APS_KEY_BY_IEEE            = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 31
    NCP_HL_BIG_PKT_TO_NCP                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 32
    NCP_HL_BIG_PKT_FROM_NCP               = ncp_hl_call_category_e.NCP_HL_CATEGORY_CONFIGURATION + 33
    NCP_HL_AF_SET_SIMPLE_DESC             = ncp_hl_call_category_e.NCP_HL_CATEGORY_AF + 1
    NCP_HL_AF_DEL_EP                      = ncp_hl_call_category_e.NCP_HL_CATEGORY_AF + 2
    NCP_HL_AF_SET_NODE_DESC               = ncp_hl_call_category_e.NCP_HL_CATEGORY_AF + 3
    NCP_HL_AF_SET_POWER_DESC              = ncp_hl_call_category_e.NCP_HL_CATEGORY_AF + 4
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
    NCP_HL_ZDO_REJOIN                     = ncp_hl_call_category_e.NCP_HL_CATEGORY_ZDO + 13
    NCP_HL_APSDE_DATA_REQ                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_APS + 1
    NCP_HL_APSME_BIND                     = ncp_hl_call_category_e.NCP_HL_CATEGORY_APS + 2
    NCP_HL_APSME_UNBIND                   = ncp_hl_call_category_e.NCP_HL_CATEGORY_APS + 3
    NCP_HL_APSME_ADD_GROUP                = ncp_hl_call_category_e.NCP_HL_CATEGORY_APS + 4
    NCP_HL_APSME_RM_GROUP                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_APS + 5
    NCP_HL_APSDE_DATA_IND                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_APS + 6
    NCP_HL_APSME_RM_ALL_GROUPS            = ncp_hl_call_category_e.NCP_HL_CATEGORY_APS + 7
    NCP_HL_NWK_FORMATION                  = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 1
    NCP_HL_NWK_DISCOVERY                  = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 2
    NCP_HL_NWK_NLME_JOIN                  = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 3
    NCP_HL_NWK_PERMIT_JOINING             = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 4
    NCP_HL_NWK_GET_IEEE_BY_SHORT          = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 5
    NCP_HL_NWK_GET_SHORT_BY_IEEE          = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 6
    NCP_HL_NWK_GET_NEIGHBOR_BY_IEEE       = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 7
    NCP_HL_NWK_STARTED_IND                = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 8
    NCP_HL_NWK_JOINED_IND                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 9
    NCP_HL_NWK_JOIN_FAILED_IND            = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 10
    NCP_HL_NWK_LEAVE_IND                  = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 11
    NCP_HL_GET_ED_KEEPALIVE_TIMEOUT       = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 12
    NCP_HL_SET_ED_KEEPALIVE_TIMEOUT       = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 13
    NCP_HL_PIM_SET_FAST_POLL_INTERVAL     = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 14
    NCP_HL_PIM_SET_LONG_POLL_INTERVAL     = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 15
    NCP_HL_PIM_START_FAST_POLL            = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 16
    NCP_HL_PIM_START_LONG_POLL            = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 17
    NCP_HL_PIM_START_POLL                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 18
    NCP_HL_PIM_SET_ADAPTIVE_POLL          = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 19
    NCP_HL_PIM_STOP_FAST_POLL             = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 20
    NCP_HL_PIM_STOP_POLL                  = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 21
    NCP_HL_PIM_ENABLE_TURBO_POLL          = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 22
    NCP_HL_PIM_DISABLE_TURBO_POLL         = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 23
    NCP_HL_NWK_GET_FIRST_NBT_ENTRY        = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 24
    NCP_HL_NWK_GET_NEXT_NBT_ENTRY         = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 25
    NCP_HL_NWK_PAN_ID_CONFLICT_RESOLVE    = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 26
    NCP_HL_NWK_PAN_ID_CONFLICT_IND        = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 27
    NCP_HL_NWK_ADDRESS_UPDATE_IND         = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 28
    NCP_HL_NWK_START_WITHOUT_FORMATION    = ncp_hl_call_category_e.NCP_HL_CATEGORY_NWKMGMT + 29
    NCP_HL_SECUR_SET_LOCAL_IC             = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 1
    NCP_HL_SECUR_ADD_IC                   = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 2
    NCP_HL_SECUR_DEL_IC                   = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 3
    NCP_HL_SECUR_ADD_CERT                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 4
    NCP_HL_SECUR_DEL_CERT                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 5
    NCP_HL_SECUR_START_KE                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 6
    NCP_HL_SECUR_START_PARTNER_LK         = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 7
    NCP_HL_SECUR_CHILD_KE_FINISHED_IND    = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 8
    NCP_HL_SECUR_PARTNER_LK_FINISHED_IND  = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 9
    NCP_HL_SECUR_JOIN_USES_IC             = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 10
    NCP_HL_SECUR_GET_IC_BY_IEEE           = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 11
    NCP_HL_SECUR_GET_CERT                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 12
    NCP_HL_SECUR_GET_LOCAL_IC             = ncp_hl_call_category_e.NCP_HL_CATEGORY_SECUR + 13
    NCP_HL_MANUF_MODE_START               = ncp_hl_call_category_e.NCP_HL_CATEGORY_MANUF_TEST + 1
    NCP_HL_MANUF_MODE_END                 = ncp_hl_call_category_e.NCP_HL_CATEGORY_MANUF_TEST + 2
    NCP_HL_MANUF_SET_CHANNEL              = ncp_hl_call_category_e.NCP_HL_CATEGORY_MANUF_TEST + 3
    NCP_HL_MANUF_GET_CHANNEL              = ncp_hl_call_category_e.NCP_HL_CATEGORY_MANUF_TEST + 4
    NCP_HL_MANUF_SET_POWER                = ncp_hl_call_category_e.NCP_HL_CATEGORY_MANUF_TEST + 5
    NCP_HL_MANUF_GET_POWER                = ncp_hl_call_category_e.NCP_HL_CATEGORY_MANUF_TEST + 6
    NCP_HL_MANUF_START_TONE               = ncp_hl_call_category_e.NCP_HL_CATEGORY_MANUF_TEST + 7
    NCP_HL_MANUF_STOP_TONE                = ncp_hl_call_category_e.NCP_HL_CATEGORY_MANUF_TEST + 8
    NCP_HL_MANUF_START_STREAM_RANDOM      = ncp_hl_call_category_e.NCP_HL_CATEGORY_MANUF_TEST + 9
    NCP_HL_MANUF_STOP_STREAM_RANDOM       = ncp_hl_call_category_e.NCP_HL_CATEGORY_MANUF_TEST + 10
    NCP_HL_MANUF_SEND_SINGLE_PACKET       = ncp_hl_call_category_e.NCP_HL_CATEGORY_MANUF_TEST + 11
    NCP_HL_MANUF_START_TEST_RX            = ncp_hl_call_category_e.NCP_HL_CATEGORY_MANUF_TEST + 12
    NCP_HL_MANUF_STOP_TEST_RX             = ncp_hl_call_category_e.NCP_HL_CATEGORY_MANUF_TEST + 13
    NCP_HL_MANUF_RX_PACKET_IND            = ncp_hl_call_category_e.NCP_HL_CATEGORY_MANUF_TEST + 14
    NCP_HL_MANUF_CALIBRATION              = ncp_hl_call_category_e.NCP_HL_CATEGORY_MANUF_TEST + 15
    NCP_HL_OTA_RUN_BOOTLOADER             = ncp_hl_call_category_e.NCP_HL_CATEGORY_OTA + 1
    NCP_HL_OTA_START_UPGRADE_IND          = ncp_hl_call_category_e.NCP_HL_CATEGORY_OTA + 2
    NCP_HL_OTA_SEND_PORTION_FW            = ncp_hl_call_category_e.NCP_HL_CATEGORY_OTA + 3

@unique
class NCP_HL_STATUS_CATEGORY(IntEnum):
    GENERIC     = 0
    SYSTEM      = 1
    MAC         = 2
    NWK         = 3
    APS         = 4
    ZDO         = 5
    CBKE        = 6
    INTERVAL    = 256

def status_id(category, code):
    return (category * NCP_HL_STATUS_CATEGORY.INTERVAL) + code

@unique
class NCP_HL_STATUS(IntEnum):
    RET_OK                              = 0 # Same as status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 0)
    RET_ERROR                           = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 1)
    RET_BLOCKED                         = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 2)
    RET_EXIT                            = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 3)
    RET_BUSY                            = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 4)
    RET_EOF                             = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 5)
    RET_OUT_OF_RANGE                    = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 6)
    RET_EMPTY                           = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 7)
    RET_CANCELLED                       = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 8)

    RET_INVALID_PARAMETER_1             = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 10),
    RET_INVALID_PARAMETER_2             = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 11),
    RET_INVALID_PARAMETER_3             = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 12),
    RET_INVALID_PARAMETER_4             = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 13),
    RET_INVALID_PARAMETER_5             = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 14),
    RET_INVALID_PARAMETER_6             = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 15),
    RET_INVALID_PARAMETER_7             = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 16),
    RET_INVALID_PARAMETER_8             = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 17),
    RET_INVALID_PARAMETER_9             = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 18),
    RET_INVALID_PARAMETER_10            = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 19),
    RET_INVALID_PARAMETER_11_OR_MORE    = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 20),
    RET_PENDING                         = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 21),
    RET_NO_MEMORY                       = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 22),
    RET_INVALID_PARAMETER               = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 23),
    RET_OPERATION_FAILED                = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 24),
    RET_BUFFER_TOO_SMALL                = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 25),
    RET_END_OF_LIST                     = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 26),
    RET_ALREADY_EXISTS                  = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 27),
    RET_NOT_FOUND                       = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 28),
    RET_OVERFLOW                        = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 29),
    RET_TIMEOUT                         = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 30),
    RET_NOT_IMPLEMENTED                 = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 31),
    RET_NO_RESOURCES                    = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 32),
    RET_UNINITIALIZED                   = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 33),
    RET_NO_SERVER                       = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 34),
    RET_INVALID_STATE                   = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 35),

    RET_CONNECTION_FAILED               = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 37),
    RET_CONNECTION_LOST                 = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 38),

    RET_UNAUTHORIZED                    = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 40),
    RET_CONFLICT                        = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 41),
    RET_INVALID_FORMAT                  = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 42),
    RET_NO_MATCH                        = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 43),
    RET_PROTOCOL_ERROR                  = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 44),
    RET_VERSION                         = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 45),
    RET_MALFORMED_ADDRESS               = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 46),
    RET_COULD_NOT_READ_FILE             = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 47),
    RET_FILE_NOT_FOUND                  = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 48),
    RET_DIRECTORY_NOT_FOUND             = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 49),
    RET_CONVERSION_ERROR                = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 50),
    RET_INCOMPATIBLE_TYPES              = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 51),
    RET_FILE_CORRUPTED                  = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 56),
    RET_PAGE_NOT_FOUND                  = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 57),

    RET_ILLEGAL_REQUEST                 = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 62),

    RET_INVALID_GROUP                   = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 64),
    RET_TABLE_FULL                      = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 65),

    RET_IGNORE                          = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 69),
    RET_AGAIN                           = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 70),
    RET_DEVICE_NOT_FOUND                = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 71),
    RET_OBSOLETE                        = status_id(NCP_HL_STATUS_CATEGORY.GENERIC, 72),

    # ZigBee Specification 2.4.5 - ZDP Enumeration Description
    ZDP_INV_REQUESTTYPE                 = status_id(NCP_HL_STATUS_CATEGORY.ZDO, 0x80),
    ZDP_DEVICE_NOT_FOUND                = status_id(NCP_HL_STATUS_CATEGORY.ZDO, 0x81),
    ZDP_INVALID_EP                      = status_id(NCP_HL_STATUS_CATEGORY.ZDO, 0x82),
    ZDP_NOT_ACTIVE                      = status_id(NCP_HL_STATUS_CATEGORY.ZDO, 0x83),
    ZDP_NOT_SUPPORTED                   = status_id(NCP_HL_STATUS_CATEGORY.ZDO, 0x84),
    ZDP_TIMEOUT                         = status_id(NCP_HL_STATUS_CATEGORY.ZDO, 0x85),
    ZDP_NO_MATCH                        = status_id(NCP_HL_STATUS_CATEGORY.ZDO, 0x86),
    ZDP_NO_ENTRY                        = status_id(NCP_HL_STATUS_CATEGORY.ZDO, 0x88),
    ZDP_NO_DESCRIPTOR                   = status_id(NCP_HL_STATUS_CATEGORY.ZDO, 0x89),
    ZDP_INSUFFICIENT_SPACE              = status_id(NCP_HL_STATUS_CATEGORY.ZDO, 0x8a),
    ZDP_NOT_PERMITTED                   = status_id(NCP_HL_STATUS_CATEGORY.ZDO, 0x8b),
    ZDP_TABLE_FULL                      = status_id(NCP_HL_STATUS_CATEGORY.ZDO, 0x8c),
    ZDP_NOT_AUTHORIZED                  = status_id(NCP_HL_STATUS_CATEGORY.ZDO, 0x8d),
    ZDP_INVALID_INDEX                   = status_id(NCP_HL_STATUS_CATEGORY.ZDO, 0x8f),

    ZB_APS_STATUS_ILLEGAL_REQUEST       = status_id(NCP_HL_STATUS_CATEGORY.APS, 0xa3),
    ZB_APS_STATUS_INVALID_BINDING       = status_id(NCP_HL_STATUS_CATEGORY.APS, 0xa4),
    ZB_APS_STATUS_INVALID_GROUP         = status_id(NCP_HL_STATUS_CATEGORY.APS, 0xa5),
    ZB_APS_STATUS_INVALID_PARAMETER     = status_id(NCP_HL_STATUS_CATEGORY.APS, 0xa6),
    ZB_APS_STATUS_NO_ACK                = status_id(NCP_HL_STATUS_CATEGORY.APS, 0xa7),
    ZB_APS_STATUS_NO_BOUND_DEVICE       = status_id(NCP_HL_STATUS_CATEGORY.APS, 0xa8),
    ZB_APS_STATUS_NO_SHORT_ADDRESS      = status_id(NCP_HL_STATUS_CATEGORY.APS, 0xa9),
    ZB_APS_STATUS_NOT_SUPPORTED         = status_id(NCP_HL_STATUS_CATEGORY.APS, 0xaa),
    ZB_APS_STATUS_SECURED_LINK_KEY      = status_id(NCP_HL_STATUS_CATEGORY.APS, 0xab),
    ZB_APS_STATUS_SECURED_NWK_KEY       = status_id(NCP_HL_STATUS_CATEGORY.APS, 0xac),
    ZB_APS_STATUS_SECURITY_FAIL         = status_id(NCP_HL_STATUS_CATEGORY.APS, 0xad),
    ZB_APS_STATUS_TABLE_FULL            = status_id(NCP_HL_STATUS_CATEGORY.APS, 0xae),
    ZB_APS_STATUS_UNSECURED             = status_id(NCP_HL_STATUS_CATEGORY.APS, 0xaf),
    ZB_APS_STATUS_UNSUPPORTED_ATTRIBUTE = status_id(NCP_HL_STATUS_CATEGORY.APS, 0xb0),

    MAC_PAN_AT_CAPACITY		            = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0x01),
    MAC_PAN_ACCESS_DENIED	            = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0x02),
    MAC_COUNTER_ERROR		            = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xDB),
    MAC_IMPROPER_KEY_TYPE	            = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xDC),
    MAC_IMPROPER_SECURITY_LEVEL	        = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xDD),
    MAC_UNSUPPORTED_LEGACY              = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xDE),
    MAC_UNSUPPORTED_SECURITY	        = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xDF),
    MAC_BEACON_LOSS                     = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xE0),
    MAC_CHANNEL_ACCESS_FAILURE	        = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xE1),
    MAC_DISABLE_TRX_FAILURE             = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xE3),
    MAC_SECURITY_ERROR		            = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xE4),
    MAC_FRAME_TOO_LONG		            = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xE5),
    MAC_INVALID_GTS                     = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xE6),
    MAC_INVALID_HANDLE		            = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xE7),
    MAC_INVALID_PARAMETER               = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xE8),
    MAC_NO_ACK			                = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xE9),
    MAC_NO_BEACON                       = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xEA),
    MAC_NO_DATA			                = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xEB),
    MAC_NO_SHORT_ADDR                   = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xEC),
    MAC_OUT_OF_CAP                      = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xED),
    MAC_PAN_ID_CONFLICT		            = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xEE),
    MAC_REALIGNMENT                     = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xEF),
    MAC_TRANSACTION_EXPIRED             = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xF0),
    MAC_TRANSACTION_OVERFLOW	        = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xF1),
    MAC_TX_ACTIVE                       = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xF2),
    MAC_UNAVAILABLE_KEY		            = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xF3),
    MAC_UNSUPPORTED_ATTRIBUTE	        = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xF4),
    MAC_INVALID_ADDR    	            = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xF5),
    MAC_PAST_TIME                       = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xF7),
    MAC_INVALID_INDEX		            = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xF9),
    MAC_LIMIT_REACHED		            = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xFA),
    MAC_READ_ONLY                       = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xFB),
    MAC_SCAN_IN_PROGRESS                = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xFC),
    MAC_UNKNOWN_FRAME_TYPE              = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xFD),
    MAC_PENDING                         = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xC0),
    MAC_FAILURE                         = status_id(NCP_HL_STATUS_CATEGORY.MAC, 0xC1)

@unique
class ncp_hl_role_e(IntEnum):
    NCP_HL_ZC  = 0
    NCP_HL_ZR  = 1
    NCP_HL_ZED = 2

class ncp_hl_role_t(ncp_hl_struct_t):
    _pack_ = 1
    _fields_ = [("u8", c_ubyte)]

    def __format__(self, fmtspec):
        return as_enum(self.u8, ncp_hl_device_type_e)

# TODO: use IntFlag as a base class
@unique
class ncp_hl_tx_options_e(IntEnum):
    NCP_HL_TX_OPT_SECURITY_ENABLED = 1
    NCP_HL_TX_OPT_ACK_TX           = 4
    NCP_HL_TX_OPT_FRAG_PERMITTED   = 8
    NCP_HL_TX_OPT_INC_EXT_NONCE    = 10

@unique
class ncp_hl_current_power_mode_e(IntEnum):
    NCP_HL_CUR_PWR_MODE_SYNC_ON_WHEN_IDLE       = 0
    NCP_HL_CUR_PWR_MODE_COME_ON_PERIODICALLY    = 1
    NCP_HL_CUR_PWR_MODE_COME_ON_WHEN_STIMULATED = 2

# TODO: use IntFlag as a base class
@unique
class ncp_hl_power_srcs_e(IntEnum):
    NCP_HL_PWR_SRCS_CONSTANT             = 1
    NCP_HL_PWR_SRCS_RECHARGEABLE_BATTERY = 1<<1
    NCP_HL_PWR_SRCS_DISPOSABLE_BATTERY   = 1<<2

@unique
class ncp_hl_power_source_level(IntEnum):
    NCP_HL_PWR_SRC_LVL_CRITICAL = 0
    NCP_HL_PWR_SRC_LVL_33       = 4
    NCP_HL_PWR_SRC_LVL_66       = 8
    NCP_HL_PWR_SRC_LVL_100      = 12

@unique
class ncp_hl_on_off_e(IntEnum):
    OFF = 0
    ON  = 1

@unique
class ncp_hl_reset_opt_e(IntEnum):
    NO_OPTIONS    = 0
    NVRAM_ERASE   = 1
    FACTORY_RESET = 2

@unique
class ncp_hl_rst_state_e(IntEnum):
    TURNED_ON                    = 0
    NVRAM_ERASE                  = 1
    NVRAM_HAS_ERASED             = 2
    RESET_TO_FACTORY_DEFAULT     = 3
    HAS_RESET_TO_FACTORY_DEFAULT = 4
    RESET_WITHOUT_OPTIONS        = 5
    HAS_RESET_WITHOUT_OPTIONS    = 6

@unique
class ncl_hl_kec_cs_e(IntEnum):
    CS1      = 1
    CS2      = 2
    BOTH     = CS1 | CS2

@unique
class ncp_hl_ota_upgrade_e(IntEnum):
    IDLE                    = 0
    RUN_BOOTLOADER          = 1
    BOOTLOADER_IS_RUN       = 2
    IMAGE_SENDING           = 3

ncp_hl_pattern = re.compile("ncp_hl_(.+)_t")

def ncp_hl_typename(cls):
    name = cls.__name__
    m = ncp_hl_pattern.match(name)
    return m.group(1) if m else name

def ncp_hl_frame(frame):
    return frame or inspect.currentframe().f_back.f_back

def frame_name(frame):
    return frame.f_code.co_name

# TODO: Make ncp_hl_rsp_status_t class and add a formatting for it
def ncp_hl_status(rsphdr):
    if rsphdr.status_code != 0:
        category = rsphdr.status_category
        code = status_id(category, rsphdr.status_code)
        return "{}:{}".format(
                as_enum(category, NCP_HL_STATUS_CATEGORY),
                as_enum(code, NCP_HL_STATUS))
    return "OK"

def ncp_hl_body(pkt, cls):
    return cls.from_buffer(pkt.body) if cls is not None else ncp_hl_empty_t()
    #return None if cls is None else cls.from_buffer(pkt.body)

def ncp_hl_body_info(body):
    return "    {}: {}".format(ncp_hl_typename(type(body)), body) if sizeof(body) != 0 else None

def ncp_hl_rsp_info(rsp, cls = None, frame = None):
    caller = frame_name(ncp_hl_frame(frame))
    status = ncp_hl_status(rsp.hdr)
    if status == "OK":
        body = ncp_hl_body(rsp, cls)
    else:
        body = ncp_hl_body(rsp, None)
    return ("  {}: {} body size {}".format(caller, status, sizeof(body)),
            ncp_hl_body_info(body))

def ncp_hl_ind_info(ind, cls, frame = None):
    caller = frame_name(ncp_hl_frame(frame))
    body = ncp_hl_body(ind, cls)
    return ("  {}: body size {}".format(caller, sizeof(body)),
            ncp_hl_body_info(body))

class ncp_host:
    callme_cb_t = CFUNCTYPE(None)

    def py_callme_cb(self):
#        logger.debug("py_callme_cb called. Wakeup.")
        self.callme = 1
        self.event.set()

    def __init__(self, so_name, rsp_switch, ind_switch, required_packets, ignore_apsde_data):
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
        self.required_packets = required_packets
        self.ignore_apsde_data = ignore_apsde_data

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
#        logger.info("ncp_host_ll_quant ret {} received {} alarm {}".format(ret, self.received, self.alarm))
        if self.received.value > 0:
            self.handle_ncp_pkt()

    def unhandled_rsp(self, rsp, rsp_len):
        logger.warn("UNHANDLED NCP RESPONSE!")

    def unhandled_ind(self, ind, ind_len):
        logger.warn("UNHANDLED NCP INDICATION!")

    def handle_ncp_pkt(self):
        if self.rsp.hdr.control == ncl_hl_pkt_type_e.NCP_HL_RESPONSE:
            func = self.rsp_switch.get(self.rsp.hdr.call_id, self.unhandled_rsp)
            if self.rsp.hdr.status_code != 0:
                logger.error("NCP call returned failure status {}".format(as_enum(status_id(self.rsp.hdr.status_category, self.rsp.hdr.status_code), NCP_HL_STATUS)))
            logger.info("rx rsp: ver {} ctl {} id {:#06x} tsn {} sts {}:{}".format(
                self.rsp.hdr.version, self.rsp.hdr.control,
                self.rsp.hdr.call_id, self.rsp.hdr.tsn,
                self.rsp.hdr.status_category, self.rsp.hdr.status_code))
            logger.info("  {} -> {}".format(
                as_enum(self.rsp.hdr.call_id, ncp_hl_call_code_e), func.__name__))
            func(self.rsp, self.received.value)
        elif self.rsp.hdr.control == ncl_hl_pkt_type_e.NCP_HL_INDICATION:
            self.ind = ncp_hl_indication_t.from_buffer(self.rsp, 0)
            func = self.ind_switch.get(self.ind.call_id, self.unhandled_ind)
            logger.info("rx ind: ver {} ctl {} id {:#06x}".format(
                self.ind.version, self.ind.control, self.ind.call_id))
            logger.info("  {} -> {}".format(
                as_enum(self.ind.call_id, ncp_hl_call_code_e), func.__name__))
            func(self.ind, self.received.value)
        self.check_required_packets(self.rsp.hdr.call_id)

    def check_required_packets(self, captured_packet):
        if self.ignore_apsde_data and (captured_packet == ncp_hl_call_code_e.NCP_HL_APSDE_DATA_REQ or captured_packet == ncp_hl_call_code_e.NCP_HL_APSDE_DATA_IND):
            pass
        elif len(self.required_packets) > 0:
            if captured_packet == self.required_packets[0]:
                logger.info("Packet {} received".format(as_enum(captured_packet, ncp_hl_call_code_e)))
                del self.required_packets[0]
            else:
                logger.error("Waiting for {}, captured call {}".format(as_enum(self.required_packets[0], ncp_hl_call_code_e), as_enum(captured_packet, ncp_hl_call_code_e)))

            remaining_count = len(self.required_packets)
            if remaining_count == 0:
                logger.info("ALL REQUIRED NCP CALLS CAPTURED")
            else:
                logger.info("Waiting for {} more ncp calls.".format(remaining_count))


    def wait_for_ncp(self):
#        logger.debug("waiting for {} ms".format(self.alarm.value))
        self.event.wait(self.alarm.value / 1000.)
        self.event.clear()
#        logger.debug("waiting complete")

    def create_next_ncp_req(self, call_id):
        self.inc_tsn()
        req = ncp_hl_request_t() # TODO: Add constructor
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, call_id, self.tsn)
        return req

    def ncp_req_no_arg(self, req):
        self.inc_tsn()
        hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, req, self.tsn)
        self.run_ll_quant(hdr, sizeof(hdr))

    def ncp_req_int8_arg(self, callid, arg):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, callid, self.tsn)
        body = ncp_hl_int8_t.from_buffer(req.body, 0)
        body.int8 = arg
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_uint8_arg(self, callid, arg):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, callid, self.tsn)
        body = ncp_hl_uint8_t.from_buffer(req.body, 0)
        body.uint8 = arg
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_uint16_arg(self, callid, arg):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, callid, self.tsn)
        body = ncp_hl_uint16_t.from_buffer(req.body, 0)
        body.uint16 = arg
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_uint32_arg(self, callid, arg):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, callid, self.tsn)
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

    def ncp_req_reset(self, option):
        self.ncp_req_uint8_arg(ncp_hl_call_code_e.NCP_HL_NCP_RESET, option)

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

    def ncp_req_get_local_ieee_addr(self):
        # TODO: add interface#
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_GET_LOCAL_IEEE_ADDR)

    def ncp_req_set_local_ieee_addr(self, addr):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_SET_LOCAL_IEEE_ADDR, self.tsn)
        body = ncp_hl_set_local_ieee_req_t.from_buffer(req.body, 0)
        body.mac_if = 0
        body.long_addr = ncp_hl_ieee_addr_t(addr)
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_get_keepalive_timeout(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_GET_KEEPALIVE_TIMEOUT)

    def ncp_req_get_ed_timeout(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_GET_ED_TIMEOUT)

    def ncp_req_get_tx_power(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_GET_TX_POWER)

    def ncp_req_get_rx_on_when_idle(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_GET_RX_ON_WHEN_IDLE)

    def ncp_req_get_joined(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_GET_JOINED)

    def ncp_req_get_authenticated(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_GET_AUTHENTICATED)

    def ncp_req_add_visible_dev(self, ieee_addr):
        self.ncp_req_8b_arg(ncp_hl_call_code_e.NCP_HL_ADD_VISIBLE_DEV, ieee_addr)

    def ncp_req_add_invisible_short(self, short_addr):
        self.ncp_req_uint16_arg(ncp_hl_call_code_e.NCP_HL_ADD_INVISIBLE_SHORT, short_addr)

    def ncp_req_rm_invisible_short(self, short_addr):
        self.ncp_req_uint16_arg(ncp_hl_call_code_e.NCP_HL_RM_INVISIBLE_SHORT, short_addr)

    def ncp_req_set_nwk_key(self, nwk_key, key_number):
        req = self.create_next_ncp_req(ncp_hl_call_code_e.NCP_HL_SET_NWK_KEY)
        body = ncp_hl_nwk_key_t.from_buffer(req.body, 0)
        memmove(body.nwk_key, nwk_key, len(nwk_key))
        body.key_number = key_number
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_get_serial_number(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_GET_SERIAL_NUMBER)

    def ncp_req_get_vendor_data(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_GET_VENDOR_DATA)

    def ncp_req_get_nwk_keys(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_GET_NWK_KEYS)

    def ncp_req_get_aps_key_by_ieee(self, ieee_addr):
        self.ncp_req_8b_arg(ncp_hl_call_code_e.NCP_HL_GET_APS_KEY_BY_IEEE, ieee_addr)


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

    def ncp_req_set_ed_timeout(self, timeo):
        self.ncp_req_uint8_arg(ncp_hl_call_code_e.NCP_HL_SET_ED_TIMEOUT, timeo)

    def ncp_req_set_tx_power(self, pwr):
        self.ncp_req_uint32_arg(ncp_hl_call_code_e.NCP_HL_SET_TX_POWER, pwr)

    def ncp_req_set_rx_on_when_idle(self, value):
        self.ncp_req_uint8_arg(ncp_hl_call_code_e.NCP_HL_SET_RX_ON_WHEN_IDLE, value)

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
        # NCP znd ZBOSS supports more than 1 channel, but our project is 2.4GHz only
        body.npages = 1
        body.page = page
        body.ch_mask = mask
        body.scan_dur = scan_dur
        # Actually that field is meaningless: netork type is already defined by device role.
        body.distributed = distributed
        body.distributed_addr = distributed_addr
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_nwk_discovery(self, page, mask, scan_dur):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_NWK_DISCOVERY, self.tsn)
        # channel list:
        # 1b n entries
        # [n] 1b page 4b mask
        #
        # 1b scan duration
        # 1b DistributedNetwork (0)
        # 2b DistributedNetworkAddress (0)
        body = ncp_hl_nwk_discovery_req_t.from_buffer(req.body, 0)
        # NCP and ZBOSS supports more than 1 page, but our project is 2.4GHz only
        body.npages = 1
        body.page = page
        body.ch_mask = mask
        body.scan_dur = scan_dur
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_nwk_join_req(self, extpanid, rejoin, page, mask, scan_dur, capability, secur_enable):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_NWK_NLME_JOIN, self.tsn)
        body = ncp_hl_nwk_nlme_join_req_t.from_buffer(req.body, 0)
        body.extpanid = extpanid
        body.rejoin = rejoin
        # NCP and ZBOSS supports more than 1 page, but our project is 2.4GHz only
        body.npages = 1
        body.page = page
        body.ch_mask = mask
        body.scan_dur = scan_dur
        body.capability = capability
        body.secur_enable = secur_enable
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_nwk_start_without_formation(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_NWK_START_WITHOUT_FORMATION)

    def nwk_dsc_stack_profile(self, flags):
        return (flags >> 4) & 0xf

    def nwk_dsc_permit_joining(self, flags):
        return flags & 1

    def nwk_dsc_router_capacity(self, flags):
        return (flags >> 1) & 1

    def nwk_dsc_end_device_capacity(self, flags):
        return (flags >> 2) & 1

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
        logger.debug("apsde-data.req len {} tsn {}".format(data_len, self.tsn))
        # 3 = sizeof param_len & data_len
        self.run_ll_quant(req, sizeof(req.hdr) + 3 + sizeof(body.params) + data_len)

    def ncp_req_apsme_binding(self, src_addr, src_endpoint, clusterid, dst_addr_mode, dst_addr, dst_endpoint, call_id):
        req = self.create_next_ncp_req(call_id)
        body = ncp_hl_apsme_bind_t.from_buffer(req.body, 0)
        body.fill(src_addr, src_endpoint, clusterid, dst_addr_mode, dst_addr, dst_endpoint)
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_apsme_bind_request(self, src_addr, src_endpoint, clusterid, dst_addr_mode, dst_addr, dst_endpoint):
        self.ncp_req_apsme_binding(src_addr, src_endpoint, clusterid, dst_addr_mode, dst_addr, dst_endpoint, ncp_hl_call_code_e.NCP_HL_APSME_BIND)

    def ncp_req_apsme_unbind_request(self, src_addr, src_endpoint, clusterid, dst_addr_mode, dst_addr, dst_endpoint):
        self.ncp_req_apsme_binding(src_addr, src_endpoint, clusterid, dst_addr_mode, dst_addr, dst_endpoint, ncp_hl_call_code_e.NCP_HL_APSME_UNBIND)

    def ncp_req_apsme_group(self, group, ep, call_id):
        req = self.create_next_ncp_req(call_id)
        body = ncp_hl_apsme_group_t.from_buffer(req.body, 0)
        body.group_addr = group
        body.endpoint = ep
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_apsme_add_group(self, group, ep):
        self.ncp_req_apsme_group(group, ep, ncp_hl_call_code_e.NCP_HL_APSME_ADD_GROUP)

    def ncp_req_apsme_rm_group(self, group, ep):
        self.ncp_req_apsme_group(group, ep, ncp_hl_call_code_e.NCP_HL_APSME_RM_GROUP)

    def ncp_req_apsme_rm_all_groups(self, ep):
        self.ncp_req_uint8_arg(ncp_hl_call_code_e.NCP_HL_APSME_RM_ALL_GROUPS, ep)

    def ncp_req_secur_add_ic(self, addr, ic):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_SECUR_ADD_IC, self.tsn)
        memmove(req.body, addr, len(addr))
        _ic = (c_ubyte * len(addr)).from_buffer(req.body, len(addr))
        memmove(_ic, ic, len(ic))
        self.run_ll_quant(req, sizeof(req.hdr)+len(addr)+len(ic))

    def ncp_req_secur_set_ic(self, ic):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_SECUR_SET_LOCAL_IC, self.tsn)
        memmove(req.body, ic, len(ic))
        self.run_ll_quant(req, sizeof(req.hdr)+len(ic))

    def ncp_req_secur_del_ic(self, ieee_addr):
        self.ncp_req_8b_arg(ncp_hl_call_code_e.NCP_HL_SECUR_DEL_IC, ieee_addr)

    def ncp_req_secur_get_ic_by_ieee(self, ieee_addr):
        self.ncp_req_8b_arg(ncp_hl_call_code_e.NCP_HL_SECUR_GET_IC_BY_IEEE, ieee_addr)

    def ncp_req_secur_join_uses_ic(self, enable):
        self.ncp_req_uint8_arg(ncp_hl_call_code_e.NCP_HL_SECUR_JOIN_USES_IC, enable)

    def ncp_req_secur_get_local_ic(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_SECUR_GET_LOCAL_IC)


    def ncp_req_secur_set_ecc_cert(self, use_cs, ca_public_key, certificate, private_key):
        self.inc_tsn()
        req = ncp_hl_request_t()
        # Packet structure:
        # 1b suite (KEC_CS1 - 1, KEC_CS2 - 2)
        # 22 (CS1) or 37 (CS2) b ca_public_key
        # 48 or 74 b certificate
        # 21 or 36 b private_key
        #
        if use_cs == 1:
            body = ncp_hl_secur_add_cert_cs1_t.from_buffer(req.body, 0)
        else:
            body = ncp_hl_secur_add_cert_cs2_t.from_buffer(req.body, 0)
        body.cs_type = use_cs
        memmove(body.ca_public_key, ca_public_key, len(ca_public_key))
        memmove(body.certificate, certificate, len(certificate))
        memmove(body.private_key, private_key, len(private_key))
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_SECUR_ADD_CERT, self.tsn)
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_secur_del_cert(self, suite, issuer, ieee_addr):
        req = self.create_next_ncp_req(ncp_hl_call_code_e.NCP_HL_SECUR_DEL_CERT)
        body = req.body_as(ncp_hl_secur_del_cert_t)
        body.fill(suite, issuer, ieee_addr)
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_secur_get_cert(self, suite, issuer, ieee_addr):
        req = self.create_next_ncp_req(ncp_hl_call_code_e.NCP_HL_SECUR_GET_CERT)
        body = req.body_as(ncp_hl_secur_del_cert_t)
        body.fill(suite, issuer, ieee_addr)
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_secur_start_ke(self, cs):
        self.ncp_req_uint8_arg(ncp_hl_call_code_e.NCP_HL_SECUR_START_KE, cs)

    def ncp_req_nwk_get_ieee_by_short(self, short_addr):
        self.ncp_req_uint16_arg(ncp_hl_call_code_e.NCP_HL_NWK_GET_IEEE_BY_SHORT, short_addr)

    def ncp_req_nwk_get_short_by_ieee(self, ieee_addr):
        self.ncp_req_8b_arg(ncp_hl_call_code_e.NCP_HL_NWK_GET_SHORT_BY_IEEE, ieee_addr)

    def ncp_req_nwk_get_neighbor_by_ieee(self, ieee_addr):
        self.ncp_req_8b_arg(ncp_hl_call_code_e.NCP_HL_NWK_GET_NEIGHBOR_BY_IEEE, ieee_addr)

    def ncp_req_nwk_get_first_nbt_entry(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_NWK_GET_FIRST_NBT_ENTRY)

    def ncp_req_nwk_get_next_nbt_entry(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_NWK_GET_NEXT_NBT_ENTRY)

    def ncp_req_nwk_pan_id_resolve(self, panid_count, panids):
        req = self.create_next_ncp_req(ncp_hl_call_code_e.NCP_HL_NWK_PAN_ID_CONFLICT_RESOLVE)
        body = req.body_as(ncp_hl_nwk_pan_id_conflict_ind_t)
        body.panid_count = panid_count
        memmove(body.panids, panids, panid_count*2)
        self.run_ll_quant(req, sizeof(req.hdr) + 2 + body.panid_count*2)

    def ncp_req_pim_set_long_poll_interval(self, quart_sec):
        self.ncp_req_uint32_arg(ncp_hl_call_code_e.NCP_HL_PIM_SET_LONG_POLL_INTERVAL, quart_sec)

    def ncp_req_pim_set_fast_poll_interval(self, quart_sec):
        self.ncp_req_uint16_arg(ncp_hl_call_code_e.NCP_HL_PIM_SET_FAST_POLL_INTERVAL, quart_sec)

    def ncp_req_pim_start_fast_poll(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_PIM_START_FAST_POLL)

    def ncp_req_pim_stop_fast_poll(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_PIM_STOP_FAST_POLL)

    def ncp_req_pim_start_poll(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_PIM_START_POLL)

    def ncp_req_pim_stop_poll(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_PIM_STOP_POLL)

    def ncp_req_pim_enable_turbo_poll(self, msec):
        self.ncp_req_uint32_arg(ncp_hl_call_code_e.NCP_HL_PIM_ENABLE_TURBO_POLL, msec)

    def ncp_req_pim_disable_turbo_poll(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_PIM_DISABLE_TURBO_POLL)

    def ncp_req_zdo_nwk_addr(self, dst_addr, ieee_addr, request_type, start_index):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_ZDO_NWK_ADDR_REQ, self.tsn)
        body = ncp_hl_zdo_nwk_addr_req_t.from_buffer(req.body, 0)
        body.dst_addr = dst_addr
        body.ieee_addr = ieee_addr
        body.request_type = request_type
        body.start_index = start_index
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_zdo_ieee_addr(self, dst_addr, nwk_addr, request_type, start_index):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_ZDO_IEEE_ADDR_REQ, self.tsn)
        body = ncp_hl_zdo_ieee_addr_req_t.from_buffer(req.body, 0)
        body.dst_addr = dst_addr
        body.nwk_addr = nwk_addr
        body.request_type = request_type
        body.start_index = start_index
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_zdo_power_desc(self, short_addr):
        self.ncp_req_uint16_arg(ncp_hl_call_code_e.NCP_HL_ZDO_POWER_DESC_REQ, short_addr)

    def ncp_req_zdo_node_desc(self, short_addr):
        self.ncp_req_uint16_arg(ncp_hl_call_code_e.NCP_HL_ZDO_NODE_DESC_REQ, short_addr)

    def ncp_req_set_simple_desc(self, cmd):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_AF_SET_SIMPLE_DESC, self.tsn)
        body = ncp_hl_set_simple_desc_t.from_buffer(req.body, 0)
        body.hdr = cmd.hdr
        body.clusters = cmd.clusters
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body.hdr) + (cmd.hdr.in_clu_count + cmd.hdr.out_clu_count) * 2)

    def ncp_req_af_del_ep(self, ep):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_AF_DEL_EP, self.tsn)
        body = ncp_hl_uint8_t.from_buffer(req.body, 0)
        body.uint8 = ep
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_zdo_simple_desc(self, short_addr, ep):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_ZDO_SIMPLE_DESC_REQ, self.tsn)
        body = ncp_hl_uint16_uint8_t.from_buffer(req.body, 0)
        body.uint16 = short_addr
        body.uint8 = ep
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_zdo_active_ep_desc(self, short_addr):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_ZDO_ACTIVE_EP_REQ, self.tsn)
        body = ncp_hl_uint16_t.from_buffer(req.body, 0)
        body.uint16 = short_addr
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_zdo_match_desc(self, cmd):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_ZDO_MATCH_DESC_REQ, self.tsn)
        body = ncp_hl_match_desc_t.from_buffer(req.body, 0)
        body.hdr = cmd.hdr
        body.clusters = cmd.clusters
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body.hdr) + (cmd.hdr.in_clu_count + cmd.hdr.out_clu_count) * 2)

    def ncp_req_zdo_bind_grp(self, target_addr, src_addr, src_endp, cluster_id, grp_addr):
        req = self.create_next_ncp_req(ncp_hl_call_code_e.NCP_HL_ZDO_BIND_REQ)
        body = req.body_as(ncp_hl_bind_req_t)
        body.fill_grp(target_addr, src_addr, src_endp, cluster_id, grp_addr)
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_zdo_bind_ep(self, target_addr, src_addr, src_endp, cluster_id, dst_addr, dst_endp):
        req = self.create_next_ncp_req(ncp_hl_call_code_e.NCP_HL_ZDO_BIND_REQ)
        body = req.body_as(ncp_hl_bind_req_t)
        body.fill_ep(target_addr, src_addr, src_endp, cluster_id, dst_addr, dst_endp)
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_zdo_unbind_grp(self, target_addr, src_addr, src_endp, cluster_id, grp_addr):
        req = self.create_next_ncp_req(ncp_hl_call_code_e.NCP_HL_ZDO_UNBIND_REQ)
        body = req.body_as(ncp_hl_bind_req_t)
        body.fill_grp(target_addr, src_addr, src_endp, cluster_id, grp_addr)
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_zdo_unbind_ep(self, target_addr, src_addr, src_endp, cluster_id, dst_addr, dst_endp):
        req = self.create_next_ncp_req(ncp_hl_call_code_e.NCP_HL_ZDO_UNBIND_REQ)
        body = req.body_as(ncp_hl_bind_req_t)
        body.fill_ep(target_addr, src_addr, src_endp, cluster_id, dst_addr, dst_endp)
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_zdo_mgmt_leave(self, dst_addr, device_addr, leave_flags):
        req = self.create_next_ncp_req(ncp_hl_call_code_e.NCP_HL_ZDO_MGMT_LEAVE_REQ)
        body = req.body_as(ncp_hl_mgmt_leave_req_t)
        body.dst_addr = ncp_hl_nwk_addr_t(dst_addr)
        body.device_addr = ncp_hl_ieee_addr_t(device_addr)
        body.leave_flags = leave_flags
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_zdo_permit_joining(self, dst_addr, duration):
        req = self.create_next_ncp_req(ncp_hl_call_code_e.NCP_HL_ZDO_PERMIT_JOINING_REQ)
        body = ncp_hl_mgmt_permit_joining_t.from_buffer(req.body, 0)
        body.dst_addr = dst_addr
        body.duration = duration
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_af_set_node_desc(self, dev_type, cap, manufacturer_code):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_AF_SET_NODE_DESC, self.tsn)
        body = ncp_hl_af_set_node_desc_t.from_buffer(req.body, 0)
        body.device_type = dev_type
        body.mac_cap = cap
        body.manufacturer_code = manufacturer_code
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_af_set_power_desc(self, cur_pwr_mode, available_pwr_srcs, cur_pwr_src, cur_pwr_src_lvl):
        self.inc_tsn()
        req = ncp_hl_request_t()
        req.hdr = ncp_hl_request_header_t(ncp_hl_ver_e.NCP_HL_VERSION, ncl_hl_pkt_type_e.NCP_HL_REQUEST, ncp_hl_call_code_e.NCP_HL_AF_SET_POWER_DESC, self.tsn)
        body = ncp_hl_af_set_power_desc_t.from_buffer(req.body, 0)
        body.cur_pwr_mode = cur_pwr_mode
        body.available_pwr_srcs = available_pwr_srcs
        body.cur_pwr_src = cur_pwr_src
        body.cur_pwr_src_lvl = cur_pwr_src_lvl
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_establish_partner_lk(self, partner_addr):
        self.ncp_req_uint16_arg(ncp_hl_call_code_e.NCP_HL_SECUR_START_PARTNER_LK, partner_addr)

    def ncp_req_zdo_rejoin(self, extpanid, mask, secure_rejoin):
        req = self.create_next_ncp_req(ncp_hl_call_code_e.NCP_HL_ZDO_REJOIN)
        body = ncp_hl_zdo_rejoin_req_t.from_buffer(req.body, 0)
        body.extpanid = extpanid
        body.mask = mask
        body.secure_rejoin = secure_rejoin
        self.run_ll_quant(req, sizeof(req.hdr) + sizeof(body))

    def ncp_req_manuf_mode_start(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_MANUF_MODE_START)

    def ncp_req_manuf_mode_end(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_MANUF_MODE_END)

    def ncp_req_manuf_set_channel(self, channel):
        self.ncp_req_uint8_arg(ncp_hl_call_code_e.NCP_HL_MANUF_SET_CHANNEL, channel)

    def ncp_req_manuf_get_channel(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_MANUF_GET_CHANNEL)

    def ncp_req_manuf_set_power(self, power):
        self.ncp_req_int8_arg(ncp_hl_call_code_e.NCP_HL_MANUF_SET_POWER, power)

    def ncp_req_manuf_get_power(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_MANUF_GET_POWER)

    def ncp_req_manuf_start_tone(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_MANUF_START_TONE)

    def ncp_req_manuf_stop_tone(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_MANUF_STOP_TONE)

    def ncp_req_manuf_start_stream(self, seed):
        self.ncp_req_uint16_arg(ncp_hl_call_code_e.NCP_HL_MANUF_START_STREAM_RANDOM, seed)

    def ncp_req_manuf_stop_stream(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_MANUF_STOP_STREAM_RANDOM)

    def ncp_req_manuf_send_packet(self, payload, length):
        req = self.create_next_ncp_req(ncp_hl_call_code_e.NCP_HL_MANUF_SEND_SINGLE_PACKET)
        body = ncp_hl_payload_t.from_buffer(req.body, 0)
        memmove(body.data, payload, len(payload))
        body.len = length
        self.run_ll_quant(req, sizeof(req.hdr) + body.len + 1)

    def ncp_req_manuf_start_test_rx(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_MANUF_START_TEST_RX)

    def ncp_req_manuf_stop_test_rx(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_MANUF_STOP_TEST_RX)

    def ncp_req_ota_run_bootloader(self):
        self.ncp_req_no_arg(ncp_hl_call_code_e.NCP_HL_OTA_RUN_BOOTLOADER)

    def ncp_req_ota_send_portion_fw(self, payload, length):
        req = self.create_next_ncp_req(ncp_hl_call_code_e.NCP_HL_OTA_SEND_PORTION_FW)
        body = ncp_hl_ota_fw_payload_t.from_buffer(req.body, 0)
        memmove(body.data, payload, len(payload))
        body.len = length
        self.run_ll_quant(req, sizeof(req.hdr) + body.len + 2)

    def ncp_req_big_pkt_to_ncp(self, start, end, length):
        #[start, end)
        req = self.create_next_ncp_req(ncp_hl_call_code_e.NCP_HL_BIG_PKT_TO_NCP)
        body = ncp_hl_big_pkt_t.from_buffer(req.body, 0)
        q = start
        for i in range(length):
            if q >= end:
                q = start
            body.data[i] = q            
            q += 1
        body.len = length
        self.run_ll_quant(req, sizeof(req.hdr) + length + 2)

