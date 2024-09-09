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
     if echo ${nm} | grep ZR2
     then
      echo zr2 - expected fail
     else
        echo $s
        killch
        exit 1
     fi
    fi
}

killch() {
    kill $r2_pid 
    kill $r1_pid
    kill $c_pid
    kill $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

export LD_LIBRARY_PATH=`pwd`/../../../../../ns/ns-3.7/build/debug

#sh clean
#make

echo "run ns-3"
../../../../platform/devtools/nsng/nsng &
PipePID=$!

sleep 5


echo "run coordinator"
./zc ${PIPE_NAME}0.write ${PIPE_NAME}0.read &
c_pid=$!
wait_for_start 1_zc
echo ZC STARTED OK
sleep 1

echo "run ZR1"
./zr1 ${PIPE_NAME}1.write ${PIPE_NAME}1.read &
r1_pid=$!
wait_for_start 2_zr1
echo ZR1 STARTED OK
sleep 1

echo "run ZR2"
./zr2 ${PIPE_NAME}2.write ${PIPE_NAME}2.read &
r2_pid=$!
#wait_for_start 3_zr2
#echo ZR2 STARTED OK
echo TRY TO START ZR2

sleep 45

echo shutdown...
killch

sh conv.sh

echo "Done. Please, verify traffic dump."
