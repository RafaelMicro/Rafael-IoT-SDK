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

    kill $the1_pid
    kill $thr1_pid
    kill $thr2_pid
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

echo "run thr1"
./thr1 &
thr1_pid=$!
wait_for_start thr1
echo THR1 STARTED OK

echo "run the1"
./the1 &
the1_pid=$!
wait_for_start the1
echo THE1 STARTED OK

sleep 181

echo "run thr2"
./thr2 &
thr2_pid=$!
wait_for_start thr2
echo THR2 STARTED OK

echo Please wait for test complete...

sleep 5

echo 'shutdown...'
killch


#sh conv.sh $1

echo 'All done, verify traffic dump, please!'

