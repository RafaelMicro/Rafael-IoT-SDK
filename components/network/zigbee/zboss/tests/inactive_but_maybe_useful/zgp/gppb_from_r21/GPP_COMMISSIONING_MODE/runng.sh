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
    kill $th_gps1PID
    kill $th_gps2PID
    kill $th_gpdPID
    kill $dut_gppPID
    kill $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

export LD_LIBRARY_PATH=`pwd`/../../../../../ns/ns-3.7/build/debug

echo "run nsng"
../../../../../platform/devtools/nsng/nsng nodes_location.cfg &
PipePID=$!

sleep 1

echo "run gppb"
./dut_gppb &
dut_gppPID=$!
wait_for_start dut_gppb
echo DUT-GPPB STARTED OK

echo "run TH-GPS1"
./th_gps1 &
th_gps1PID=$!
wait_for_start th_gps1
echo TH-GPS1 STARTED OK

echo "run TH-GPS2"
./th_gps2 &
th_gps2PID=$!
wait_for_start th_gps2
echo TH-GPS2 STARTED OK

echo "run TH-gpd"
./th_gpd &
th_gpdPID=$!

#for test with 20 seconds of default commissioning window
#echo "wait 200 sec"
#sleep 200

#for test with standart value of default commissioning window
echo "wait 700 sec"
sleep 700

echo shutdown...
killch

sh conv.sh

echo 'Now verify traffic dump, please!'
