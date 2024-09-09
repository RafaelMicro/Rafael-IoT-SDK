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

source ./config.mk

devs_pids=()

start_device () {
    pushd $TARGET_DIR
    ./zdo_start_$1 &
    devs_pids+=($!)
    wait_for_start $1
    popd
}

wait_for_start() {
    nm=$1
    s=''
    while [ A"$s" = A ]
    do
        sleep 1
        if [ -f core ]
        then
            echo 'Somebody has crashed (ns?)'
            kill_all
            exit 1
        fi
        s=$(grep Device zdo_${nm}*.log)
    done
    if echo $s | grep OK
    then
        return
    else
        echo $s
        kill_all
        exit 1
    fi
}

kill_all() {
    for pid in ${devs_pids[@]}; do
        kill $pid
    done
    kill $nsng_pid
}

handle_keyboard_interrupt() {
    kill_all
    echo Interrupted by user!
    exit 1
}

clear() {
    rm -f $TARGET_DIR/*.nvram $TARGET_DIR/*.log $TARGET_DIR/*.dump *.pcap NS.log NS.dump
}

trap handle_keyboard_interrupt TERM INT

clear

echo "Configuration: ZC, $ZR_CNT ZRs"
echo "Waiting duration after all devices start: $TEST_DURATION"

echo "run ns"
#../../platform/devtools/nsng/nsng ../../devtools/nsng/nodes_location.cfg &
$NSNG &
nsng_pid=$!
sleep 1
echo


echo "run coordinator"
start_device zc
echo ZC STARTED OK
echo

for num in $(seq 1 $ZR_CNT); do
    echo run zr$num
    start_device zr$num
    echo ZR$num STARTED OK
    echo
done

echo "Waiting $TEST_DURATION seconds..."

sleep $TEST_DURATION

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
kill_all


echo 'Now verify traffic dump, please!'
