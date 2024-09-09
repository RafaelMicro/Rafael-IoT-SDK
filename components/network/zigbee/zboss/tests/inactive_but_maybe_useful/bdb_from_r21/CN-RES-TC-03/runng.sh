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
        return
#        killch
#        exit 1
    fi
}

killch() {
    kill $c1_pid
    kill $c2_pid
    kill $ed_pid
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
../../../../platform/devtools/nsng/nsng &
ns_pid=$!
sleep 2

echo "run ZC2"
./zc2 &
c2_pid=$!
wait_for_start 1_zc2

echo "shutdown ZC2"
kill $c2_pid

echo "run ZC1"
./zc1 &
c1_pid=$!
wait_for_start 2_zc1

echo "run DUT ZED"
./zed &
ed_pid=$!
wait_for_start 3_zed

sleep 15

echo "shutdown DUT ZED"
kill $ed_pid

echo "Wait 3 minutes, ZC1 will close PAN"
sleep 190

echo "Restart ZC2"
./zc2 &
c2_pid=$!
wait_for_start 1_zc2

echo "Restart DUT ZED"
./zed &
ed_pid=$!
wait_for_start 3_zed

sleep 15

echo 'shutdown...'
killch

sh conv.sh

echo 'All done, verify traffic dump, please!'

