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

kill_zr() {
    kill $ed_pid
}

killch() {
    kill $ed_pid
    kill $PipePID
    kill $c_pid
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

rm -f *.log *.dump *.pcap ns.txt *.*~ *.nvram core*

#sh clean
#make

echo "run ns"
../../../platform/devtools/nsng/nsng &
PipePID=$!
echo sim started, pid $PipePID

echo

sleep 1

echo "run dut_zed"
./dut_zed &
ed_pid=$!
wait_for_start dut_zed
echo "dut_zed started"
sleep 1

echo

sleep 5

echo "run th_zc"
./th_zc &
c_pid=$!
wait_for_start th_zc
echo "th_zc started"

echo

sleep 1

sleep 30

echo "kill dut_zed"

kill $ed_pid

echo "dut_zed killed"

sleep 1

echo "run dut_zed"
./dut_zed &
ed_pid=$!
wait_for_start dut_zed
echo "dut_zed started"
sleep 1

echo

sleep 10

echo "kill th_zc"

kill $c_pid

echo "th_zc killed"

sleep 1

echo "kill dut_zed"

echo

kill $ed_pid

echo "dut_zed killed"

sleep 1

echo

echo "run dut_zed"
./dut_zed &
ed_pid=$!
wait_for_start dut_zed
echo "dut_zed started"
sleep 1

echo

sleep 5

echo "run th_zc"
./th_zc &
c_pid=$!
wait_for_start th_zc
echo "th_zc started"

sleep 1

echo

sleep 10

echo shutdown...
killch

#sh conv.sh

echo 'Now verify traffic dump, please!'
