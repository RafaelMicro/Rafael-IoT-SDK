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
            echo 'Somebody has crashed'
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
    kill $c1_pid
    kill $c2_pid
    kill $r1_pid
    kill $ed1_pid
    kill $ns_pid

    sh conv.sh
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

#sh clean
#make

rm -f *log *dump *pcap *nvram

wd=`pwd`
echo "run ns"
../../../../platform/devtools/nsng/nsng $wd/nodes_location.cfg &
ns_pid=$!
sleep 2

echo "run gZC1"
./gzc1 &
c1_pid=$!
wait_for_start 1_gzc1
echo gZC1 STARTED OK $!

echo "run gZC2"
./gzc2 &
c2_pid=$!
wait_for_start 2_gzc2
echo gZC2 STARTED OK $!

sleep 1

echo "run DUT ZR"
./dutzr &
r1_pid=$!
wait_for_start 3_dutzr
echo DUT ZR STARTED OK $!

sleep 1

echo "run DUT ZED"
./dutzed &
ed1_pid=$!
wait_for_start 4_dutzed
echo DUT ZED STARTED OK $!

echo 'Wait 2 minutes'
sleep 30
echo 'Factory reset of DUT ZR and DUT ZC (move to gzc2 network)'
kill $ed1_pid $r1_pid
sleep 10

./dutzr &
r1_pid=$!
wait_for_start 3_dutzr
sleep 1
./dutzed &
ed1_pid=$!
wait_for_start 4_dutzed

sleep 30
echo 'Factory reset of DUT ZR and DUT ZC (move back to gzc1 network)'
kill $ed1_pid $r1_pid
sleep 10

./dutzr &
r1_pid=$!
wait_for_start 3_dutzr
sleep 1
./dutzed &
ed1_pid=$!
wait_for_start 4_dutzed

sleep 30

echo 'shutdown...'
killch

sh conv.sh

echo 'All done, verify traffic dump, please!'

