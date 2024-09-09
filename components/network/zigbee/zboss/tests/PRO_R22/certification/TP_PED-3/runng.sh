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

# Retrieve DUT's role
dut_role="$1"

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
    if [ "$dut_role" = "zc" ]
    then
        kill $c_pid
    elif [ "$dut_role" = "zr" ]
    then
        kill $r_pid
    fi

    kill $zed1_pid
    kill $zed2_pid
    kill $ns_pid
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

start_zed_12() {
    echo "run gZED1"
    ./zed1 &
    zed1_pid=$!
    wait_for_start 2_zed1
    echo gZED1 STARTED OK

    echo "run gZED2"
    ./zed2 &
    zed2_pid=$!
    wait_for_start 3_zed2
    echo gZED2 STARTED OK
}

trap killch_ex TERM INT

#sh clean
#make

rm -f *log *dump *pcap *nvram

echo "run ns"
../../../../platform/devtools/nsng/nsng &
ns_pid=$!
sleep 2

if [ "$dut_role" = "zc" ]
then
    echo "run ZC"
    ./zc &
    c_pid=$!
    wait_for_start 1_zc
    echo DUR ZC STARTED OK

    start_zed_12
elif [ "$dut_role" = "zr" ]
then
    echo "run ZR"
    ./zr &
    r_pid=$!
    wait_for_start 1_zr
    echo DUR ZR STARTED OK

    start_zed_12
else
    echo "Select dut role (zc or zr)"
    kill $ns_pid
    exit 1
fi


echo Please wait for test complete...

sleep 120

echo "Power down gZED1..."
kill $zed1_pid
echo gZED1 POWERED DOWN
sleep 150
echo Wait for 2 minutes...

echo "Power up gZED1"
./zed1 &
zed1_pid=$!
wait_for_start 2_zed1
echo gZED1 STARTED OK
sleep 30

echo 'shutdown...'
killch


sh conv.sh $1

echo 'All done, verify traffic dump, please!'

