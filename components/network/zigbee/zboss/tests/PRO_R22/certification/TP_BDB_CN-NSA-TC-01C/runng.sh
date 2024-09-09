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

    kill $zed_pid
    kill $ns_pid
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
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
    ./dut_c &
    c_pid=$!
    wait_for_start dut
    echo DUR ZC STARTED OK

elif [ "$dut_role" = "zr" ]
then
    echo "run ZR"
    ./dut_r &
    r_pid=$!
    wait_for_start dut
    echo DUR ZR STARTED OK

else
    echo "Select dut role (zc or zr)"
    kill $ns_pid
    exit 1
fi
#1a
echo "run gZED"
./the1 &
zed_pid=$!
wait_for_start the1
echo gZED STARTED OK

sleep 3
#1b
echo "run gZR"
./thr1 &
zr_pid=$!

sleep 0.5

killch $zr_pid
#2a
sleep 16
#2b
echo "run gZR"
./thr1 &
zr_pid=$!

sleep 0.5

killch $zr_pid

#3a
sleep 16

#3b
echo "run gZR"
./thr1 &
zr_pid=$!

sleep 0.5

killch $zr_pid


echo Please wait for test complete...

sleep 15

echo 'shutdown...'
killch


#sh conv.sh $1

echo 'All done, verify traffic dump, please!'

