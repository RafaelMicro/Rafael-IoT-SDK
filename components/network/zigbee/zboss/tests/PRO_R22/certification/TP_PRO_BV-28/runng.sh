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
declare -a zed_pids

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
    [ -z "$gzc_pid" ] || kill $gzc_pid

    echo Kill End devices
    [ -z "$gzc_pids" ] || kill ${zed_pids[*]}

    echo Kill NS
    [ -z "$PipePID" ] || kill $PipePID
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


for j in `seq 1 10`
do
    echo Test Iteration $j
    
    echo "run gZC"
    ./zc ${PIPE_NAME}0.write ${PIPE_NAME}0.read &
    gzc_pid=$!
    wait_for_start zc
    
    sleep 1

    for i in `seq 1 10`
    do
        echo run ZED$i
        ./zed ${PIPE_NAME}$i.write ${PIPE_NAME}$i.read $i &
        zed_pids[$i]=$!
        wait_for_start zed$i
        sleep 1
    done

    echo "Reboot devices"
    kill $gzc_pid
    kill ${zed_pids[*]}
    sleep 1
done

sleep 5


echo shutdown...
killch

sh conv.sh

echo "All done, verify traffic, please!"
