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
            echo 'Somebody has crashed?'
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
    kill $zr1_pid $zr_pid $zr2_pid $zed_pid $ns_pid
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

#sh clean
#make

rm -f *.log *.dump *.nvram *.pcap

echo "run network simulator"
../../../../platform/devtools/nsng/nsng &
ns_pid=$!
sleep 1

echo "run zr1"
./zr1 &
zr1_pid=$!
wait_for_start 1_zr1
echo ZR1 STARTED OK

echo "run dut zr"
./dutzr &
zr_pid=$!
wait_for_start 2_dutzr
echo DUT ZR STARTED OK

sleep 20
kill $zr1_pid

echo "run zr2"
./zr2 &
zr2_pid=$!
wait_for_start 3_zr2
echo ZR2 STARTED OK

sleep 20
kill $zr_pid

echo "run zed"
./zed &
zed_pid=$!
wait_for_start 4_zed
echo ZED STARTED OK

killch

if grep "Device STARTED OK" zdo_4_zed*.log
then
  echo "DONE. TEST PASSED!!!"
else
  echo "ERROR. TEST FAILED!!!"
fi

sh conv.sh

echo 'Now verify traffic dump, please!'
