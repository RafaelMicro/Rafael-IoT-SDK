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

svnrev=`svn info | grep Revision | sed "s,^[^0-9]*\([0-9]\+\)$,\1,"`
date=$(date +%y_%m_%d-%H_%M_%S)
dname="R$svnrev-$date"

cd _results
mkdir "$dname"
echo "Added new directory $dname"
cd $dname
mkdir pcap log
cd ../../

ls -1 *.pcap | while read fpcap ; 
do
   echo "Copy to results folder, file: $fpcap"
   cp $fpcap _results/$dname/pcap/$fpcap
done

ls -1 *.log | while read flog ; 
do
   echo "Copy to results folder, file: $flog"
   cp $flog _results/$dname/log/$flog
done

cd _results
svn add $dname
