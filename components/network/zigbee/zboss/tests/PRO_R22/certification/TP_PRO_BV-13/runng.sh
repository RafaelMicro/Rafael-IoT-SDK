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
    kill $pid1 $pid2 $pid3 $PipePID
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

../../../../platform/devtools/nsng/nsng &
PipePID=$!
sleep 1

echo "run coordinator"
./zc &
pid1=$!
wait_for_start 1_zc
echo ZC STARTED OK

echo "run zed"
./zed &
pid2=$!
wait_for_start 2_zed
echo ZED STARTED OK

#sleep 5

echo "run zr"
./zr &
pid3=$!
wait_for_start 3_zr
echo ZR STARTED OK

sleep 90

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch

sh conv.sh

echo 'Now verify traffic dump, please!'

