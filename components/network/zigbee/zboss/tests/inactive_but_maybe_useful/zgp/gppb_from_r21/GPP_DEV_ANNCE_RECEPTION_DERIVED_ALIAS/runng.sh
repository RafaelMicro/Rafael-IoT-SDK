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
    kill $th_zrPID
    kill $dut_gppPID
    kill $th_gpsPID
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

echo "run TH-gps"
./th_gps &
th_gpsPID=$!
wait_for_start th_gps
echo TH-gps STARTED OK
sleep 1

echo "run TH-ZR"
./th_zr &
th_zrPID=$!
wait_for_start th_zr
echo TH-ZR STARTED OK
sleep 1
echo "pause TH ZR"
kill $th_zrPID
sleep 1

echo "run DUT-GPP"
./dut_gpp &
dut_gppPID=$!
wait_for_start dut_gpp
echo DUT-GPP STARTED OK
sleep 2

echo "run th-gpd"
./th_gpd &
th_gpdPID=$!
sleep 10

echo "run TH-ZR"
./th_zr &
th_zrPID=$!
wait_for_start th_zr
echo TH-ZR STARTED OK

sleep 15

echo shutdown...
killch

sh conv.sh

echo 'Now verify traffic dump, please!'
