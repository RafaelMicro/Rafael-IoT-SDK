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
    kill $thPID
    kill $gpsPID
    kill $gpdPID
    kill $gppPID
    kill $PipePID
}

killdut() {
    kill $gppPID
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
gppPID=$!
wait_for_start dut_gppb
echo DUT-GPPB STARTED OK

echo "run TH-tool"
./th_tool &
thPID=$!
wait_for_start th_tool
echo TH-tool STARTED OK

echo "run TH-gps"
./th_gps &
gpsPID=$!
wait_for_start th_gps
echo TH-gps STARTED OK

echo "run TH-gpd"
./th_gpd &
gpdPID=$!

sleep 15

echo "Switch off the DUT-GPP for approximately 5 seconds"

killdut

sleep 5

echo "Switch the DUT-GPP back on"
./dut_gppb &
gppPID=$!
wait_for_start dut_gppb
echo DUT-GPPB STARTED OK

sleep 20

echo shutdown...
killch

sh conv.sh

echo 'Now verify traffic dump, please!'
