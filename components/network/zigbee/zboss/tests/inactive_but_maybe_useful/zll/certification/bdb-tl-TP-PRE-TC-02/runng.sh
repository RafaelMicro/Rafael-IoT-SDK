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
    kill $endPID $routerPID $PipePID
    rm -f /tmp/zt[0123].*
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

rm -f *.log *.pcap *.nvram *.dump
#sh clean
#echo "make"
#make

echo "run ns"

../../../../platform/devtools/nsng/nsng &
PipePID=$!
sleep 1

echo -n "run ZR device... "
./zr /tmp/zt1.write /tmp/zt1.read &
routerPID=$!
wait_for_start zr

echo STARTED OK

echo -n "run ZED2... "
./zed2 /tmp/zt3.write /tmp/zt3.read &
endPID=$!
wait_for_start zed2

echo STARTED OK

sleep 20

echo 'power off zed2 zr...'
kill $endPID $routerPID

mv zr*.log prev_zr.log
mv zr*.dump prev_zr.dump


echo -n "run ZR device again... "
./zr /tmp/zt1.write /tmp/zt1.read &
routerPID=$!
wait_for_start zr

echo STARTED OK

echo -n "run ZED1... "
./zed1 /tmp/zt2.write /tmp/zt2.read &
endPID=$!
wait_for_start zed1

echo STARTED OK

sleep 20


if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch

sh conv.sh

echo 'Now verify traffic dump, please!'

if grep "Test finished. Status: OK" zed1*.log
then
    echo "DONE. Test Passed OK\n"
else
    echo "Error. Test FAILED\n"
fi
