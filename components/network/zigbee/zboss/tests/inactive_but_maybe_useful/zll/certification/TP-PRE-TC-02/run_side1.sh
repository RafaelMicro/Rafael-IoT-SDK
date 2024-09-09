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

echo -n "run ZR device... "
./zr &
routerPID=$!

# run zed2
# work
read -p "wait run ZED2 and 40 sec" inputline

echo 'power off zr...'
kill $routerPID

mv zr*.log prev_zr.log

read -p "wait stop ZED2" inputline

echo "run ZR device again... "
echo "zed2 must be stpped"
./zr &
routerPID=$!

# run zed1
# work
read -p "wait run ZED1 and 40 sec" inputline

echo 'shutdown...'
kill $routerPID

echo 'Now verify traffic dump, please!'
