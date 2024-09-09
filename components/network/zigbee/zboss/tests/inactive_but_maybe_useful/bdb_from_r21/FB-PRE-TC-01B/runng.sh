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
    [ -z "$thc1_pid" ] || kill $thc1_pid
    [ -z "$thr1_pid" ] || kill $thr1_pid
    [ -z "$the1_pid" ] || kill $the1_pid
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


start_thc1() {
    echo "run TH ZC (additional device)"
    ./thc1 &
    thc1_pid=$!
    wait_for_start thc1
}

start_thr1() {
    echo "run TH ZR1 (target)"
    ./thr1 &
    thr1_pid=$!
    wait_for_start thr1
}

start_the1() {
    echo "run TH ZED1 (target)"
    ./the1 &
    the1_pid=$!
    wait_for_start the1
}

start_dut_r() {
    echo "run DUT ZR (initiator)"
    ./dut_r &
    dut_pid=$!
    wait_for_start dut
}

start_dut_ed() {
    echo "run DUT ZED (initiator)"
    ./dut_ed &
    dut_pid=$!
    wait_for_start dut
}

power_off_dut() {
    [ -z "$dut_pid" ] || kill $dut_pid && unset dut_pid
}

power_off_thr1() {
    [ -z "$thr1_pid" ] || kill $thr1_pid && unset thr1_pid
}


if [ "$dut_role" = "zr" ]
then
    power_on_dut="start_dut_r"

elif [ "$dut_role" = "zed" ]
then
    power_on_dut="start_dut_ed"
    
else
    echo "Select dut role (zed or zr)"
    killch
    exit 1
fi


start_thc1
sleep 1
start_thr1
sleep 1
power_off_thr1
$power_on_dut
sleep 1
power_off_dut
start_thr1
sleep 30
start_the1
sleep 1
$power_on_dut


echo "Wait 90 seconds"
sleep 90

echo 'shutdown...'
killch



sh conv.sh

echo 'All done, verify traffic dump, please!'

