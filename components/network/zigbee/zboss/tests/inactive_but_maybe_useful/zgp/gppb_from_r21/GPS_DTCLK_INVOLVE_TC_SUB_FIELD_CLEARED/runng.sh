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
    [ -z "$th_gpdPID" ]    || kill $th_gpdPID
    [ -z "$th_gppPID" ]    || kill $th_gppPID
    [ -z "$th_zcPID" ]    || kill $th_zcPID
    [ -z "$dut_gpsPID" ]   || kill $dut_gpsPID
    [ -z "$PipePID" ]      || kill $PipePID
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

start_gpd() {
  echo "run TH-GPD"
  ./th_gpd &
  th_gpdPID=$!
  wait_for_start th_gpd
  echo "TH-GPD STARTED OK"
}

echo "run nsng"
../../../../../platform/devtools/nsng/nsng nodes_location.cfg &
PipePID=$!

sleep 1

echo "run TH-ZC"
./th_zc &
th_zcPID=$!
wait_for_start th_zc
echo "TH-ZC STARTED OK"

sleep 1

echo "run TH-GPP"
./th_gpp &
th_gppPID=$!
wait_for_start th_gpp
echo "TH-GPP STARTED OK"

sleep 1

echo "run DUT-GPS"
./dut_gps &
dut_gpsPID=$!
wait_for_start dut_gps
echo "DUT-gps STARTED OK"

echo "Wait 17 seconds"
sleep 17

start_gpd
echo "Wait 120 seconds"
sleep 120

echo shutdown...
killch

sh conv.sh

echo 'Now verify traffic dump, please!'
