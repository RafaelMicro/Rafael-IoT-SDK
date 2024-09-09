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

dut_role="$1"
#set - `ls *dump`
set - `ls NS*dump`
../../../../devtools/dump_converter/dump_converter -ns $1 ns.pcap

if [ "$dut_role" = "zc" ]
then
set - `ls *dutzc*dump`
../../../../devtools/dump_converter/dump_converter -ns $1 dutzc.pcap
set - `ls *gzr_c*dump`
../../../../devtools/dump_converter/dump_converter -ns $1 gzr_c.pcap
elif [ "$dut_role" = "zr" ]
then
set - `ls *dutzr*dump`
../../../../devtools/dump_converter/dump_converter -ns $1 dutzr.pcap
set - `ls *gzr_d*dump`
../../../../devtools/dump_converter/dump_converter -ns $1 gzr_d.pcap
elif [ "$dut_role" = "zed" ]
then
set - `ls *dutzed*dump`
../../../../devtools/dump_converter/dump_converter -ns $1 dutzed.pcap
set - `ls *gzr_d*dump`
../../../../devtools/dump_converter/dump_converter -ns $1 gzr_d.pcap

fi

#rm -f *.dump

#sh save-res.sh
