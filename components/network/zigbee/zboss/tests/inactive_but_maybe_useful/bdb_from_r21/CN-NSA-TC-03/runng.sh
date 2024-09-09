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
    [ -z "$thc2_pid" ] || kill $thc2_pid
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
../../../../platform/devtools/nsng/nsng $wd/nodes_location.cfg &
ns_pid=$!
sleep 2


run_thc1() {
    echo "TH ZC1 powered on"
    ./thc1 &
    thc1_pid=$!
    wait_for_start thc1
}

run_thc2() {
    echo "TH ZC2 powered on"
    ./thc2 &
    thc2_pid=$!
    wait_for_start thc2
}

run_thr1() {
    echo "TH ZR1 powered on"
    ./thr1 &
    thr1_pid=$!
    wait_for_start thr1
}


run_dut_r() {
    echo "DUT ZR powered on"
    ./dut_r &
    dut_pid=$!
}

run_dut_ed() {
    echo "DUT ZED powered on"
    ./dut_ed &
    dut_pid=$!
}

power_off_devices() {
    echo "Power off devices"
    [ -z "$dut_pid" ] || kill $dut_pid && unset dut_pid
    [ -z "$thc1_pid" ] || kill $thc1_pid && unset thc1_pid
    [ -z "$thc2_pid" ] || kill $thc2_pid && unset thc2_pid
    [ -z "$thr1_pid" ] || kill $thr1_pid && unset thr1_pid
    sleep 3
}


if [ "$dut_role" = "zr" ]
then
    echo "DUT is ZR"
    power_on_dut="run_dut_r"
elif [ "$dut_role" = "zed" ]
then
    echo "DUT is ZED"
    power_on_dut="run_dut_ed"
else
    echo "Select dut role (zr or zed)"
    killch
    exit 1
fi

echo "Start TEST"
echo "No child capacity at THc1"
run_thc1
sleep 1
run_thr1
run_thc2
sleep 7

echo "Power off THC2"
kill $thc2_pid
unset thc2_pid
sleep 1
$power_on_dut
sleep 10
power_off_devices

echo -e "\\n\\nIncomplete association at one node of network A"
run_thc1
sleep 1
run_thr1
sleep 7
$power_on_dut
sleep 10
power_off_devices

echo -e "\\n\\nIncomplete association in network A"
run_thc1
sleep 1
run_thr1
sleep 1
run_thc2
sleep 7
$power_on_dut
sleep 10
power_off_devices

echo -e "\\n\\nNetwork key not delivered"
run_thc1
run_thc2
sleep 2
$power_on_dut
sleep 30
power_off_devices

echo -e "\\n\\nUnique TCLK not delivered "
run_thc1
run_thc2
sleep 2
$power_on_dut
sleep 60
power_off_devices


killch
sh conv.sh


echo 'All done, verify traffic dump, please!'

