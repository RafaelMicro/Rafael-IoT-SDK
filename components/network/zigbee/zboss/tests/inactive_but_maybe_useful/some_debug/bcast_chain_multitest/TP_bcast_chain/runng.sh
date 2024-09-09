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
    if echo $s | grep -e OK
    then
        return
    else
        echo $s
        killch
        exit 1
    fi
}

killch() {
    echo "Kill ZC"
    kill $pid1 

    echo "Kill ZED"
    kill $pid2 

    echo "Kill ZR1"
    kill $pid3 

    echo "Kill ZR2"
    kill $pid4

    echo "Kill ZR3"
    kill $pid5 

    echo "Kill ZR4"
    kill $pid6 

    echo "Kill ZR5"
    kill $pid7 

    echo "Kill NS"
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

../../../platform/devtools/nsng/nsng &
PipePID=$!
sleep 1

echo "run zc"
./bcast_chain_zc &
pid1=$!
wait_for_start 1_zc
echo ZC STARTED OK

echo "run zr1"
./bcast_chain_zr1 &
pid2=$!
wait_for_start 2_zr1
echo ZR1 STARTED OK

sleep 15

echo "run zr2"
./bcast_chain_zr2 &
pid3=$!
wait_for_start 3_zr2
echo ZR2 STARTED OK

sleep 15

echo "run zed1"
./bcast_chain_zed1 &
pid4=$!
wait_for_start 4_zed1
echo ZED1 STARTED OK

sleep 15

echo "run zr3"
./bcast_chain_zr3 &
pid5=$!
wait_for_start 5_zr3
echo ZR3 STARTED OK

sleep 15

echo "run zr4"
./bcast_chain_zr4 &
pid6=$!
wait_for_start 6_zr4
echo ZR4 STARTED OK

sleep 15

echo "run zed2"
./bcast_chain_zed2 &
pid7=$!
wait_for_start 7_zed2
echo ZED2 STARTED OK

sleep 15

echo "Test started..."

sleep 300

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch
