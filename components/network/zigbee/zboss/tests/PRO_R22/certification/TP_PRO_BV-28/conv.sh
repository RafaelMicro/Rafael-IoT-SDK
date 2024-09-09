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

set - `ls *zc*dump`
../../../../devtools/dump_converter/dump_converter -ns $1 zc.pcap
set - `ls *zed*dump`
../../../../devtools/dump_converter/dump_converter -ns $1 zed1.pcap
../../../../devtools/dump_converter/dump_converter -ns $2 zed2.pcap
../../../../devtools/dump_converter/dump_converter -ns $3 zed3.pcap
../../../../devtools/dump_converter/dump_converter -ns $4 zed4.pcap
../../../../devtools/dump_converter/dump_converter -ns $5 zed5.pcap
../../../../devtools/dump_converter/dump_converter -ns $6 zed6.pcap
../../../../devtools/dump_converter/dump_converter -ns $7 zed7.pcap
../../../../devtools/dump_converter/dump_converter -ns $8 zed8.pcap
../../../../devtools/dump_converter/dump_converter -ns $9 zed9.pcap
../../../../devtools/dump_converter/dump_converter -ns $10 zed10.pcap

rm -f *.dump

#sh save-res.sh
