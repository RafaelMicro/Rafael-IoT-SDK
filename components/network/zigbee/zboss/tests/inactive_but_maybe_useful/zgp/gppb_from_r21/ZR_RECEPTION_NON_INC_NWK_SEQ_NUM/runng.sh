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
    kill $th_gppPID
    kill $dut_zrPID
    kill $th_gptPID
    kill $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

killdut() {
    kill $dut_zrPID
}

trap killch_ex TERM INT

export LD_LIBRARY_PATH=`pwd`/../../../../../ns/ns-3.7/build/debug

make clean
sh make_test.sh

echo "run nsng"
../../../../../platform/devtools/nsng/nsng nodes_location.cfg &
PipePID=$!

sleep 1

echo "run TH-gpt"
./th_gpt &
th_gptPID=$!
wait_for_start th_gpt
echo TH-gpt STARTED OK

echo "run DUT-zr"
./dut_zr &
dut_zrPID=$!
wait_for_start dut_zr
echo DUT-zr STARTED OK

sleep 1

echo "kill dut..."
killdut
echo "...done"

echo "run TH-gpp"
./th_gpp &
th_gppPID=$!
wait_for_start th_gpp
echo TH-gpp STARTED OK

echo "run th-gpd"
./th_gpd &
th_gpdPID=$!
echo TH-gpd STARTED OK

echo "wait derived groupcast commissioning..."
sleep 5
echo "...done"

sh cln_zr.sh

echo "run DUT-zr"
./dut_zr &
dut_zrPID=$!
wait_for_start dut_zr
echo DUT-zr STARTED OK

sleep 10

echo shutdown...
killch

sh conv.sh

echo 'Now verify traffic dump, please!'
