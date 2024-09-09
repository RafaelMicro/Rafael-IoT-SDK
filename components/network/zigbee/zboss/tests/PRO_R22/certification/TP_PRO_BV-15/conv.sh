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
../../../../devtools/dump_converter/dump_converter -n2 $1 ns.pcap
set - `ls *zc*dump`
../../../../devtools/dump_converter/dump_converter -n2 $1 zc.pcap
set - `ls *zr1*dump`
../../../../devtools/dump_converter/dump_converter -n2 $1 zr1.pcap
set - `ls *zr2*dump`
../../../../devtools/dump_converter/dump_converter -n2 $1 zr2.pcap
set - `ls *zr3*dump`
../../../../devtools/dump_converter/dump_converter -n2 $1 zr3.pcap
set - `ls *zr4*dump`
../../../../devtools/dump_converter/dump_converter -n2 $1 zr4.pcap

#rm -f *.dump

#sh save-res.sh
