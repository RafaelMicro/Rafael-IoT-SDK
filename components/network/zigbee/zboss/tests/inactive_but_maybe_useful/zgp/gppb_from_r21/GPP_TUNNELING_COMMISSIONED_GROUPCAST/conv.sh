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
# PURPOSE:

#set - `ls *dump`
set - `ls NS*dump`
../../../../../devtools/dump_converter/dump_converter  $1 ns.pcap
set - `ls *dut_gppb*dump`
../../../../../devtools/dump_converter/dump_converter  $1 dut_gppb.pcap
set - `ls *th_gpd*dump`
../../../../../devtools/dump_converter/dump_converter  $1 th_gpd.pcap
set - `ls *th_gpt*dump`
../../../../../devtools/dump_converter/dump_converter  $1 th_gpt.pcap

#rm -f *.dump

#sh save-res.sh
