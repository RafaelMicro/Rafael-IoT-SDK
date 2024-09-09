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
    kill $gpdPID
    kill $thtPID
    kill $gpsPID
    kill $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

export LD_LIBRARY_PATH=`pwd`/../../../../../ns/ns-3.7/build/debug

#make clean
#make

echo "run nsng"
../../../../../platform/devtools/nsng/nsng nodes_location.cfg &
PipePID=$!

sleep 1

echo "run DUT-GPS"
./dut_gps &
gpsPID=$!
wait_for_start dut_gps
echo DUT-GPS STARTED OK

echo "run TH-GPP"
./th_gpp &
thtPID=$!
wait_for_start th_gpp
echo TH-gpp STARTED OK

echo "run gpd"
./th_gpd &
gpdPID=$!
wait_for_start th_gpd
echo TH-GPD STARTED OK

echo "wait 10 seconds"
sleep 10

echo shutdown...
killch

echo 'converting dump to pcap'
sh conv.sh
echo 'Now verify traffic dump, please!'
