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
        if [ -f core ]
        then
            echo 'Somebody has crashed (ns?)'
            killch
            exit 1
        fi
        s=`grep Device zdo_${nm}*.log`
    done
    if echo $s | grep started
    then
        return
    else
        echo $s
        killch
        exit 1
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

#sh clean
#make

echo "run ns"
../../../../platform/devtools/nsng/nsng &
PipePID=$!
echo sim started, pid $PipePID

sleep 5

echo "run coordinator"
./zc &
c_pid=$!
# wait_for_start zc
echo ZC STARTED OK
sleep 5

echo "run zr1"
./zr1 &
r1_pid=$!
wait_for_start zr1
echo ZR1 STARTED OK
sleep 5

echo "run zr2"
./zr2 &
r2_pid=$!
wait_for_start zr2
echo ZR2 STARTED OK

sleep 600

echo shutdown...
killch

sh conv.sh

echo 'Now verify traffic dump, please!'
