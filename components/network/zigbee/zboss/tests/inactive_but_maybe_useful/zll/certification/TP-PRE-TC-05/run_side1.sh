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
#

rm -f *.log *.dump *.pcap *.nvram ns*.txt *.*~

echo "run ZR device... "
./zr &
routerPID=$!

#run ZED1
#work
read -p "wait run ZED1 and 40 sec" inputline

echo 'power off zr... '
kill $routerPID

read -p "wait stop ZED1" inputline

echo "run ZED2 device"
echo "ZED1 must be stopped"
./zed2 &
endPID2=$!

#run ZED1
#work
read -p "wait run ZED1 and 40 sec" inputline

echo 'shutdown...'
kill $endPID2

echo 'Now verify traffic dump, please!'
