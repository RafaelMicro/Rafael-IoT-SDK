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
fb_type="$2"

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


if [ "$fb_type" = "initiator" ]
then
    echo "run TH ZR1 (target - form distributed network)"
    ./thr1_t &
    thr1_pid=$!
    wait_for_start thr1
    echo TH ZR1 STARTED OK $!
    
elif [ "$fb_type" = "target"  ]
then
    echo "run TH ZR1 (initiator - form distributed network)"
    ./thr1_i &
    thr1_pid=$!
    wait_for_start thr1
    echo TH ZR1 STARTED OK $!

else
    echo "Select DUT f&b role (target or initiator)"
    killch
    exit 1
fi

sleep 3

if [ "$dut_role" = "zr" -a "$fb_type" = "initiator" ]
then
    echo "run DUT ZR (initiator)"
    ./dut_ri &
    dut_pid=$!
    wait_for_start dut
    echo DUT ZR STARTED OK $!

elif [ "$dut_role" = "zr" -a "$fb_type" = "target" ]
then
    echo "run DUT ZR (target)"
    ./dut_rt &
    dut_pid=$!
    wait_for_start dut
    echo DUT ZR STARTED OK $!

elif [ "$dut_role" = "zed" -a  "$fb_type" = "initiator" ]
then
    echo "run DUT ZED (initiator)"
    ./dut_edi &
    dut_pid=$!
    wait_for_start dut
    echo DUT ZED STARTED OK $!

elif [ "$dut_role" = "zed" -a  "$fb_type" = "target" ]
then
    echo "run DUT ZED (target)"
    ./dut_edt &
    dut_pid=$!
    wait_for_start dut
    echo DUT ZED STARTED OK $!

else
    echo "Select dut role (zr or zed) and fb role (initiator or target)"
    killch
    exit 1
fi

sleep 3


echo "wait for 50 seconds"
sleep 50

echo 'shutdown...'
killch


if grep "Finding&binding done" *dut*.log
then
    echo "TEST PASSED: DUT start f&b procedure!"
else
    echo "TEST FAILED: DUT can not start finding&bindingr"
fi


sh conv.sh

echo 'All done, verify traffic dump, please!'

