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
    kill $c_pid
    kill $r1_pid
    kill $ed1_pid
    kill $ed2_pid
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

rm -f *log *dump *pcap

wd=`pwd`
echo "run ns"
../../../../platform/devtools/nsng/nsng $wd/nodes_location.cfg &
ns_pid=$!
sleep 2

echo "run ZC"
./zc &
c_pid=$!
wait_for_start 1_zc
echo ZC STARTED OK $!

echo "run ZR1"
./zr1 &
r1_pid=$!
wait_for_start 2_zr
echo ZR1 STARTED OK $!

echo "run ZED1"
./zed1 &
ed1_pid=$!
wait_for_start 3_zed1
echo ZED1 STARTED OK $!

echo "run ZRED2"
./zed2 &
ed2_pid=$!
wait_for_start 4_zed2
echo ZED2 STARTED OK $!

sleep 10

echo 'shutdown...'
killch

sh conv.sh

echo 'All done, verify traffic dump, please!'

