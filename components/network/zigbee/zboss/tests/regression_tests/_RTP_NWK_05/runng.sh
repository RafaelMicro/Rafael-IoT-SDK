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
    kill $c_pid $zed_pid $zed_2_pid $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

echo "run ns"
../../../platform/devtools/nsng/nsng &
PipePID=$!
echo sim started, pid $PipePID
sleep 1

echo

echo "run th_zc"
./th_zc &
c_pid=$!
wait_for_start th_zc
echo ZC STARTED OK
sleep 1

echo

echo "run dut_zed"
./dut_zed &
zed_pid=$!
wait_for_start dut_zed
echo ZED STARTED OK

sleep 60

echo

echo "run dut_zed_2"
./dut_zed_2 &
zed_2_pid=$!
wait_for_start dut_zed_2
echo ZED_2 STARTED OK

echo

sleep 210

echo shutdown...
killch

echo 'Now verify traffic dump, please!'
