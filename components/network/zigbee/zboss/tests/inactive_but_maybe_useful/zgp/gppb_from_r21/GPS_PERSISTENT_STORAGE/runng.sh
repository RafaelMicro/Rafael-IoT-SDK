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
    kill $th_gpdPID
    kill $th_toolPID
    kill $dut_gpsPID
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
#sh make_test.sh

rm -f *.log *.dump *.pcap *.nvram

echo "run nsng"
../../../../../platform/devtools/nsng/nsng nodes_location.cfg &
PipePID=$!

sleep 1

echo "run TH-TOOL"
./th_tool &
th_toolPID=$!
wait_for_start th_tool
echo "TH-TOOL STARTED OK"
sleep 2

echo "run DUT-GPS"
./dut_gps &
dut_gpsPID=$!
wait_for_start dut_gps
echo "DUT-gps STARTED OK"
sleep 1

echo "Wait 10 seconds"
sleep 10

echo "run TH-GPD"
./th_gpd &
th_gpdPID=$!
wait_for_start th_gpd
echo "TH-GPD STARTED OK"

echo "Wait 20 seconds"
sleep 20

echo "Reboot DUT, wait 5 seconds"
kill $dut_gpsPID
sleep 5

echo "Turn on DUT"
./dut_gps &
dut_gpsPID=$!
wait_for_start dut_gps
echo "DUT-gps STARTED OK"

echo "Wait 30 seconds"
sleep 30


echo shutdown...
killch

sh conv.sh

echo 'Now verify traffic dump, please!'
