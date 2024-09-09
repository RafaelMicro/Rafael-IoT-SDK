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

echo "run ZR2 device"
./zr2 &
routerPID=$!

#run ZED1
#work
read -p "wait run ZED1 and 40 sec" inputline

echo 'power off zr2...'
kill $routerPID

read -p "wait stop ZED1" inputline

echo "run ZR1 device"
echo "ZED1 must be stopped"
./zr1 &
routerPID=$!

#run ZED2
#work
read -p "wait run ZED2 and 40 sec" inputline

echo 'power off zr1...'
kill $routerPID

mv zr1*.log zr_prev1.log

read -p "wait stop ZED2" inputline

echo "run ZR1 device again"
echo "ZED2 must be stopped"
./zr1 &
routerPID=$!

#run ZED1
#work
read -p "wait run ZED1 and 40 sec" inputline

kill $routerPID
echo 'Now verify traffic dump, please!'
