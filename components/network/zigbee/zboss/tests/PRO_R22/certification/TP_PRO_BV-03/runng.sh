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

wait_for_start() {
    nm=$1
    s=''
    while [ A"$s" = A ]
    do
        sleep 1
        if [ -f core ]
        then
            echo 'Somebody has crashed?'
            killch
            exit 1
        fi
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
    kill $c_pid 
    kill $ed_pid 
    kill $ns_pid
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

#sh clean
#make

echo "run network simulator"
../../../../platform/devtools/nsng/nsng &
ns_pid=$!
sleep 1

echo "run coordinator"
./zc &
c_pid=$!
wait_for_start 1_zc
echo ZC STARTED OK

echo "run router"
./zr &
r_pid=$!
wait_for_start 2_zr
echo ZR STARTED OK

sleep 10

echo "run ed"
./zed &
ed_pid=$!
wait_for_start 3_zed
echo ED STARTED OK

sleep 15
echo "hard reset router device..."
date
kill $r_pid

sleep 70

./zr &
r_pid=$!
wait_for_start 2_zr
echo ZR STARTED OK

sleep 30

date

killch

sh conv.sh

echo 'Now verify traffic dump, please!'
