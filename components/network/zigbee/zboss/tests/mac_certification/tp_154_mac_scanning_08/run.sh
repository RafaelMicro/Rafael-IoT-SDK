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
        s=`grep waiting tst_${nm}*.log`
    done
}

wait_for_start_zdo() {
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
        s=`grep Device tst_${nm}*.log`
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
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT


rm -f *.log *.pcap *.dump core

echo "run ns"

../../../devtools/network_simulator/network_simulator --nNode=2 --pipeName=/tmp/st >ns.txt 2>&1 &
PipePID=$!
sleep 1

echo "run coordinator"
./zdo_start_zc /tmp/st0.write /tmp/st0.read &
coordPID=$!
wait_for_start_zdo zc

echo ZC STARTED OK

echo "run ze"
./start_ze /tmp/st1.write /tmp/st1.read &
endPID=$!
wait_for_start ze

echo ZE STARTED OK

sleep 20

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch


set - `ls *dump`
../../../devtools/dump_converter/dump_converter -ns $1 c.pcap
../../../devtools/dump_converter/dump_converter -ns $2 ze.pcap

echo 'Now verify traffic dump, please!'

if grep "pan desc:" tst_ze*.log
then
  echo "DONE. Check Pan Descriptor"
else
  echo "ERROR. TEST FAILED!!!"
fi

