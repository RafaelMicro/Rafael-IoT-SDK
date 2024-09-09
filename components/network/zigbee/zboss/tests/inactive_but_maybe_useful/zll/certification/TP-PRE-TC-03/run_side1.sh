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

echo "run ZR1 device... "
./zr1 &
routerPID=$!

#run ZED
#work
read -p "wait run ZED and 40 sec" inputline

echo 'power off zr1... '
kill $routerPID

read -p "wait stop ZED" inputline

echo "run ZR2 device... "
echo "ZED must be stpped"
./zr2 &
routerPID=$!

echo STARTED OK

#run ZED
#work
read -p "wait run ZED and 40 sec" inputline

echo 'shutdown...'
kill $routerPID

echo 'Now verify traffic dump, please!'
