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
    kill $endPID $coordPID $PipePID
    rm -f /tmp/$pipe_name[01].*
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

rm -f *.nvram *.log *.pcap *.dump core

echo "run ns-3"
../../platform/devtools/nsng/nsng nodes_location.cfg &
PipePID=$!
sleep 1

echo "run harness... "
./scenes_zc &
coordPID=$!
wait_for_start scenes_zc

echo STARTED OK

echo "run test device under test..."
./scenes_zed &
endPID=$!
wait_for_start scenes_zed

echo STARTED OK

sleep 10

echo "kill device under test..."
kill $endPID
mv scenes_zed.log scenes_zed-0.log

echo "restart device under test..."
./scenes_zed &
endPID=$!
wait_for_start scenes_zed

echo STARTED OK

sleep 7

echo "restart device ZC..."
kill $coordPID
mv scenes_zc.log scenes_zc-0.log

sleep 5

./scenes_zc &
coordPID=$!
wait_for_start scenes_zc

echo STARTED OK

sleep 15

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch


# set - `ls *dump`
# ../../../../devtools/dump_converter/dump_converter -ns $2 zed.pcap
# ../../../../devtools/dump_converter/dump_converter -ns $1 zc.pcap

echo 'Now verify traffic dump, please!'


if grep "Test finished. Status: OK" scenes_zed*.log
then
    echo "DONE. Test Passed OK\n"
else
    echo "Error. Test FAILED\n"
fi

