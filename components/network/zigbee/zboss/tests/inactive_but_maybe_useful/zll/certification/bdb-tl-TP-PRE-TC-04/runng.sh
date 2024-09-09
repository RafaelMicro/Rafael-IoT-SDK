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
    rm -f /tmp/zt[01].*
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

rm -f *.dump *.log *.pcap *.nvram
#sh clean
#echo "make"
#make

echo "run ns"

../../../../platform/devtools/nsng/nsng &
PipePID=$!
sleep 1

echo -n "run ZR2 device... "
./zr2 &
routerPID=$!
wait_for_start zr2

echo STARTED OK

echo -n "run ZED1 device... "
./zed1 &
endPID=$!
wait_for_start zed1

echo STARTED OK
echo wait 20s
sleep 20

echo 'power off zed1 zr2...'
kill $endPID $routerPID


echo -n "run ZR1 device... "
./zr1 &
routerPID=$!
wait_for_start zr1

echo STARTED OK

echo -n "run ZED2 device... "
./zed2 &
endPID=$!
wait_for_start zed2

echo STARTED OK
echo wait 20s
sleep 20

echo 'power off zed2 zr1...'
kill $endPID $routerPID

mv zed1*.log zed_prev1.log
mv zed1*.dump zed_prev1.dump
mv zr1*.log zr_prev1.log
mv zr1*.dump zr_prev1.dump


echo -n "run ZR1 device again... "
./zr1 &
routerPID=$!
wait_for_start zr1

echo STARTED OK

echo -n "run ZED1 device again ... "
./zed1 &
endPID=$!
wait_for_start zed1

echo STARTED OK
echo wait 20s
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
