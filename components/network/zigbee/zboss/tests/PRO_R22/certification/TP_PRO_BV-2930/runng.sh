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
    kill $pid_c1 
#    kill $pid_r1 
    kill $pid_r2
    kill $pid_c2
    kill $pid_r3
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

rm -f *nvram *log *dump *pcap

../../../../platform/devtools/nsng/nsng nodes_location.cfg &
PipePID=$!
sleep 1

echo "run coordinator"
./zc1 &
pid_c1=$!
wait_for_start 1_zc1
echo ZC1 STARTED OK $!

#echo "run gZR1"
#./zr1 &
#pid_r1=$!
#wait_for_start 2_zr1
#echo ZR1 STARTED OK $!

echo "run zr2"
./zr2 &
pid_r2=$!
wait_for_start 3_zr2
echo ZR2 STARTED OK $!

sleep 2

echo "run zc2"
./zc2 &
pid_c2=$!
wait_for_start 4_zc2
echo ZC2 STARTED OK $!

sleep 2

echo "run zr3 - it must not start"
./zr3 &
pid_r3=$!

sleep 120

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch

sh conv.sh

echo 'Now verify traffic dump, please!'

