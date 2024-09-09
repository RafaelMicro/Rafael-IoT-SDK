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
        s=`grep Device *${nm}*.log`
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
    echo Kill gZC
    kill $gzc_pid
    echo Kill gZR1
    kill $gzr1_pid
    echo Kill gZED1
    kill $gzed1_pid

    echo Kill DUT ZC
    kill $dutzc_pid
    echo Kill DUT ZR1
    kill $dutzr1_pid
    echo Kill DUT ZED1
    kill $dutzed1_pid

    echo Kill NS
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

echo "run ns-3"
../../../../platform/devtools/nsng/nsng &
PipePID=$!

sleep 5

echo "run gZC"
./gzc ${PIPE_NAME}0.write ${PIPE_NAME}0.read &
gzc_pid=$!
wait_for_start gzc
echo gZC STARTED OK $!
sleep 1

echo "run gZR1"
./gzr1 ${PIPE_NAME}1.write ${PIPE_NAME}1.read &
gzr1_pid=$!
wait_for_start gzr1
echo gZR1 STARTED OK $!
sleep 1

echo "run gZED1"
./gzed1 ${PIPE_NAME}2.write ${PIPE_NAME}2.read &
gzed1_pid=$!
wait_for_start gzed1
echo gZED1 STARTED OK $!


sleep 30


echo "run DUT ZC"
./dutzc ${PIPE_NAME}3.write ${PIPE_NAME}3.read &
dutzc_pid=$!
wait_for_start dutzc
echo DUT ZC STARTED OK $!
sleep 1

echo "run DUT ZR1"
./dutzr1 ${PIPE_NAME}4.write ${PIPE_NAME}4.read &
dutzr1_pid=$!
wait_for_start dutzr1
echo DUT ZR STARTED OK $!
sleep 1

echo "run DUT ZED1"
./dutzed1 ${PIPE_NAME}5.write ${PIPE_NAME}5.read &
dutzed1_pid=$!
wait_for_start dutzr1
echo ZR STARTED OK $!

sleep 80

echo shutdown...
killch

sh conv.sh

echo "All done, verify traffic, please!"
