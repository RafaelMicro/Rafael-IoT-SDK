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

ulimit -c unlimited
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
    [ -z "$dut_pid" ] || kill $dut_pid
    [ -z "$thr1_pid" ] || kill $thr1_pid
    [ -z "$thr2_pid" ] || kill $thr2_pid
    [ -z "$ns_pid" ] || kill $ns_pid

    sh conv.sh
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

wd=`pwd`
echo "run ns"
../../../../platform/devtools/nsng/nsng  &
ns_pid=$!


sleep 3


start_thr1() {
    echo "run TH ZR1"
    ./thr1 &
    thr1_pid=$!
    wait_for_start thr1
    echo TH ZR1 STARTED OK $!
}

start_thr2() {
    echo "run TH ZR2 (target)"
    ./thr2 &
    thr2_pid=$!
    wait_for_start thr2
    echo TH ZR2 STARTED OK $!
}


if [ "$dut_role" = "zc" ]
then
    echo "run DUT ZC (initiator)"
    ./dut_c &
    dut_pid=$!
    wait_for_start dut
    echo DUT ZC STARTED OK $!

    start_thr2
    start_thr1

elif [ "$dut_role" = "zr" ]
then
    echo "run DUT ZR (initiator)"
    ./dut_r &
    dut_pid=$!
    wait_for_start dut
    echo DUT ZR STARTED OK $!

    start_thr2
    start_thr1

elif [ "$dut_role" = "zed" ]
then
    start_thr2
    
    echo "run DUT ZED (initiator)"
    ./dut_ed &  
    dut_pid=$!
    wait_for_start dut
    echo DUT ZED STARTED OK $!

    start_thr1

else
    echo "Select dut role (zc, zed or zr)"
    killch
    exit 1
fi


echo "wait for 7 minutes"
sleep 420

echo 'shutdown...'
killch



sh conv.sh

echo 'All done, verify traffic dump, please!'

