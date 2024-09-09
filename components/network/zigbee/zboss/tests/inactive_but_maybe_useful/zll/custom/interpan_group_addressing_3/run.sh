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
        s=`grep Device intrp_${nm}*.log`
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
    kill $pid1st $pid2nd $pid3rd $PipePID
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

echo "run ns"

../../../../devtools/network_simulator/network_simulator --nNode=4 --pipeName=/tmp/zt >ns.txt 2>&1 &
PipePID=$!
sleep 1

echo -n "run device #2... "
./intrp_zr2nd /tmp/zt2.write /tmp/zt2.read &
pid2nd=$!
wait_for_start zr2nd

echo -n "run device #3... "
./intrp_zr3rd /tmp/zt3.write /tmp/zt3.read &
pid3rd=$!
wait_for_start zr3rd

echo STARTED OK

echo -n "run device #1... "
./intrp_zc /tmp/zt1.write /tmp/zt1.read &
pid1st=$!
wait_for_start zc

echo STARTED OK

sleep 30

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch


set - `ls *dump`
../../../../devtools/dump_converter/dump_converter -ns $1 zc.pcap
../../../../devtools/dump_converter/dump_converter -ns $2 zr1.pcap
../../../../devtools/dump_converter/dump_converter -ns $3 zr2.pcap

echo 'Now verify traffic dump, please!'

if grep "Test finished. Status: OK" intrp_zc*.log
then
    echo "DONE. Test Passed OK\n"
else
    echo "Error. Test FAILED\n"
fi
