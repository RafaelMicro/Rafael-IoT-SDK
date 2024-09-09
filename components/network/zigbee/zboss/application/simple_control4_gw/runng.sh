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

PIPE_NAME=&

wait_for_start() {
    nm=$1
    s=''
    while [ A"$s" = A ]
    do
        sleep 1
        s=`grep Device ${nm}*.log`
    done
    if echo $s | grep OK
    then
        return
    else
        echo $s
        killch
        exit 1
    fi
}

killch() {
    kill $zrPID $cPID $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

sh clean
make

echo "run ns-3"
../../platform/devtools/nsng/nsng nodes_location.cfg &
PipePID=$!

sleep 1

echo "run coordinator"
./simple_control4_gw &
cPID=$!
wait_for_start simple_control4_gw
echo ZC STARTED OK

sleep 5

echo "run zed"
./../smart_plug_v2/sp_device/sp_c4_device &
zrPID=$!
wait_for_start sp_c4_device
echo ZED STARTED OK

sleep 30

kill $zrPID
echo "restart zed"
./../smart_plug_v2/sp_device/sp_c4_device &
zrPID=$!
wait_for_start sp_c4_device
echo ZED STARTED OK

sleep 30

kill $cPID
echo "restart coordinator"
./simple_control4_gw &
cPID=$!
wait_for_start simple_control4_gw
echo ZC STARTED OK

sleep 30

echo shutdown...
killch

echo 'Now verify traffic dump, please!'
