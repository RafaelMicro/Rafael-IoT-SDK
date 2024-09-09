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
    kill $r_PID 
    kill $c_PID 
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
sleep 3

echo "run ZC"
./zc ${PIPE_NAME}0.write ${PIPE_NAME}0.read &
c_PID=$!
wait_for_start zc
echo ZC STARTED OK

echo "run ZR"
./zr ${PIPE_NAME}1.write ${PIPE_NAME}1.read &
r_PID=$!
wait_for_start zr
echo ZR STARTED OK $!

sleep 60

echo shutdown...
killch

sh conv.sh

echo 'Now verify traffic dump, please!'
