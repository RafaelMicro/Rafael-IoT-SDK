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
        s=`grep -Ir Device zdo_${nm}*.log`
    done
}

killch() {
    kill $zc1_PID
    kill $zc2_PID
    kill $zr1_PID
    kill $zr2_PID
    kill $zr3_PID
    kill $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

rm -f *.log *.dump *.pcap ns.txt *.*~ *.nvram core*

echo "run nsng"
../../platform/devtools/nsng/nsng nodes_location.cfg &
PipePID=$!
sleep 1

echo
echo "run coordinator 1"
./zc1 &
zc1_PID=$!
wait_for_start zc1
echo ZC zc1 STARTED OK
sleep 5

echo
echo "run router 1"
./zr1 &
zr1_PID=$!
wait_for_start zr1
echo ZR zr1 STARTED OK
sleep 5

echo
echo "run router 2"
./zr2 &
zr2_PID=$!
wait_for_start zr2
echo ZR zr2 STARTED OK
sleep 5

echo
echo "run coordinator 2"
./zc2 &
zc2_PID=$!
wait_for_start zc2
echo ZC zc2 STARTED OK
sleep 35

echo
echo "run router 3"
./zr3 &
zr3_PID=$!
wait_for_start zr3
echo ZR zr3 STARTED OK


sleep 40
echo
echo shutdown...
killch

echo 'Now verify traffic dump, please!'
