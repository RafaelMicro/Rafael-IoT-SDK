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
            echo 'Somebody has crashed (ns?)'
            killch
            exit 1
        fi
        s=`grep Device ${nm}*.log`
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
    kill $endPID $routerPID1 $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

sh clean

echo "make"

make

echo "run ns"

../../../../devtools/network_simulator/network_simulator --nNode=4 --pipeName=/tmp/zt >ns.txt 2>&1 &
PipePID=$!
sleep 1

echo -n "run ZR1 device... "
./zr1 /tmp/zt1.write /tmp/zt1.read &
routerPID1=$!
wait_for_start zr1

echo STARTED OK
sleep 2

echo -n "run ZED... "
./zed /tmp/zt3.write /tmp/zt3.read &
endPID=$!
wait_for_start zed

echo STARTED OK

sleep 8

echo 'power off zr1'
echo 'pre steps finished'

kill $routerPID1 $endPID

echo -n "run ZR1 device... "
./zr1 /tmp/zt1.write /tmp/zt1.read &
routerPID1=$!
wait_for_start zr1

echo STARTED OK
sleep 2

echo -n "run ZED... "
./zed /tmp/zt3.write /tmp/zt3.read &
endPID=$!
wait_for_start zed

echo STARTED OK
sleep 15

echo 'power off zr1'

kill $routerPID1

echo -n "run ZR2 device"
./zr2 /tmp/zt2.write /tmp/zt2.read &
routerPID2=$!
wait_for_start zr2

echo STARTED OK
sleep 30

echo 'power off zr2'
kill $routerPID2

echo -n "run ZR1 device"
./zr1 /tmp/zt1.write /tmp/zt1.read &
routerPID1=$!
wait_for_start zr1
echo STARTED OK

sleep 30

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch

sh conv.sh

echo 'Now verify traffic dump, please!'

if grep "Test finished. Status: OK" zed*.log
then
    echo "DONE. Test Passed OK\n"
else
    echo "Error. Test FAILED\n"
fi
