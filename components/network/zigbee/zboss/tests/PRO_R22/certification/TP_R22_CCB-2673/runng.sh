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
    kill $ns_pid
    kill $gzc_pid
    kill $gzr1_pid
    kill $gzr2_pid
    kill $dutzr3_pid
    kill $gzed_pid
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
./gzc &
gzc_pid=$!
wait_for_start 1_zc
echo GZC STARTED OK

sleep 10

echo "run gzr1"
./gzr1 &
gzr1_pid=$!
wait_for_start 2_gzr1
echo GZR1 STARTED OK

sleep 10

echo "run gzr2"
./gzr2 &
gzr2_pid=$!
wait_for_start 3_gzr2
echo GZR2 STARTED OK

sleep 10

echo "run dutzr3"
./dutzr3 &
dutzr3_pid=$!
wait_for_start 4_dutzr3
echo GZR2 STARTED OK

sleep 10

echo "run gzed"
./gzed &
gzed_pid=$!
wait_for_start 5_gzed
echo GZED STARTED OK

sleep 50

killch

sh conv.sh

echo 'Now verify traffic dump, please!'
