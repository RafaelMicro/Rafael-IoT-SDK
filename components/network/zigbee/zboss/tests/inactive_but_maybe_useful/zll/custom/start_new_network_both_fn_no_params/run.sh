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
    kill $endPID $coordPID $PipePID
    rm -f /tmp/zt[01].*
}

killch_ex() {
    killch
    echo Interrupted by user!
    rm -f /tmp/zt[01].*
    set - `ls *dump`
    ../../../../devtools/dump_converter/dump_converter -ns $2 zed.pcap
    ../../../../devtools/dump_converter/dump_converter -ns $1 zr.pcap
    exit 1
}

trap killch_ex TERM INT


rm -f *.log *.pcap *.dump core /tmp/zt[01].* *.nvram

echo "make"

make clean
make

echo "run ns"

../../../../devtools/network_simulator/network_simulator --nNode=3 --pipeName=/tmp/zt >ns.txt 2>&1 &
PipePID=$!
sleep 1

echo -n "run ZR device... "
./zr /tmp/zt1.write /tmp/zt1.read &
coordPID=$!
wait_for_start zr

echo STARTED OK

echo -n "run ZED... "
./zed /tmp/zt2.write /tmp/zt2.read &
endPID=$!
wait_for_start zed

echo STARTED OK

sleep 100

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch


set - `ls *dump`
../../../../devtools/dump_converter/dump_converter -ns $1 zed.pcap
../../../../devtools/dump_converter/dump_converter -ns $2 zr.pcap

echo 'Now verify traffic dump, please!'

if grep "Test finished. Status: OK" zed*.log
then
    echo "DONE. Test Passed OK\n"
else
    echo "Error. Test FAILED\n"
    wireshark zr.pcap &
fi
