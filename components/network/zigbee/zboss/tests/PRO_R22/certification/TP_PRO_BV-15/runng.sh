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
    kill $pid1 $pid2 $pid3 $pid4 $pid5 $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

#sh clean
#make
rm -f *.log

wd=`pwd`
echo "run ns"
../../../../platform/devtools/nsng/nsng $wd/nodes_location.cfg &
PipePID=$!
sleep 1

echo "run coordinator"
./zc &
pid1=$!
wait_for_start zc
echo ZC STARTED OK

echo "run zr1"
./zr1 &
pid2=$!
wait_for_start zr1
echo ZR1 STARTED OK $!

echo "run zr2"
./zr2 &
pid3=$!
wait_for_start zr2
echo ZR2 STARTED OK $!

echo "run zr3"
./zr3 &
pid4=$!
wait_for_start zr3
echo ZR3 STARTED OK $!

echo "wait nwkNetworkBroadcastDeliveryTime"
sleep 10

echo "run zr4"
./zr4 &
pid5=$!
wait_for_start zr4
echo ZR4 STARTED OK $!

sleep 90

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch

sh conv.sh

echo 'Now verify traffic dump, please!'

