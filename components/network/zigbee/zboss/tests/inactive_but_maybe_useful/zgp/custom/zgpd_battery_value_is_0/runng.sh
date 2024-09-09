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

decode_dump() {
    set - `ls NS*dump`
    ../../../../devtools/dump_converter/dump_converter -ns $1 ns.pcap
    set - `ls zc*dump`
    ../../../../devtools/dump_converter/dump_converter -ns $1 c.pcap
    set - `ls zgpd*dump`
    ../../../../devtools/dump_converter/dump_converter -ns $1 d.pcap
}


mypid=$$

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
    kill $devicePID $coordPID $PipePID
    decode_dump
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

sh clean

make clean

make

rm -f *.nvram *.log *.pcap *.dump core *.core

echo "run ns"

../../../../platform/devtools/nsng/nsng &
PipePID=$!
sleep 1

echo -n "run sink... "
../devices/simple_coordinator/coordinator 1 1 1 &
coordPID=$!
wait_for_start zc

echo -n "run ZGPD... "
./device 1 1 1&
devicePID=$!
wait_for_start zgpd

echo "STARTED OK"

#for i in $devicePID $coordPID ; do
	#wait $i
#done

sleep 80

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch

#sh conv.sh

echo 'Now verify traffic dump, please!'

if grep "Test finished. Status: OK" zgpd*.log
then
    echo "DONE. Test Passed OK\n"
else
    echo "Error. Test FAILED\n"
fi
