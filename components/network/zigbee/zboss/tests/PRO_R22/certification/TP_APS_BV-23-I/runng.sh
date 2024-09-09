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
        s=`grep Device zdo_${nm}*.log`
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
    echo "shutdown DUT ZR1"
    kill $r1_PID
    echo "shutdown gZR2"
    kill $r2_PID
    echo "shutdown gZR3"
    kill $r3_PID
    echo "shutdown ZC"
    kill $coordPID 
    echo "shutdown NS"
    kill $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

#sh clean
#make

rm -f *log *dump *pcap *nvram

echo "run ns-3"
../../../../platform/devtools/nsng/nsng &
PipePID=$!
sleep 3

echo "run ZC"
./zc ${PIPE_NAME}0.write ${PIPE_NAME}0.read &
coordPID=$!
wait_for_start zc
echo ZC STARTED OK

echo "run DUT ZR1"
./zr1 ${PIPE_NAME}1.write ${PIPE_NAME}1.read &
r1_PID=$!
wait_for_start zr1
echo DUT ZR1 STARTED OK

echo "run gZR2"
./zr2 ${PIPE_NAME}2.write ${PIPE_NAME}2.read &
r2_PID=$!
wait_for_start zr2
echo gZR2 STARTED OK

echo "run DUT ZR3"
./zr3 ${PIPE_NAME}3.write ${PIPE_NAME}3.read &
r3_PID=$!
wait_for_start zr3
echo gZR3 STARTED OK

sleep 180

echo shutdown...
killch

echo "All done, verify trafic log, please!"
