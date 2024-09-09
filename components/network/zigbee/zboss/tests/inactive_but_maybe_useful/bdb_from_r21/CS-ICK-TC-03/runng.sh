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


run_dut_r() {
    echo "run DUT ZR"
   ./dut_r &
   dut_pid=$!
}

run_dut_ed() {
    echo "run DUT ZED"
   ./dut_ed &
   dut_pid=$!
}

power_off_dut() {
    echo "Power off DUT"
    [ -z "$dut_pid" ] || kill $dut_pid && unset dut_pid
}

run_thc1() {
    echo "run TH ZC (bdbJoinUsesInstallCodeKey = TRUE)"
    ./thc1 &
    thc1_pid=$!
    wait_for_start thc1
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

echo "Start test"
echo "Joining without using installcodes (THC1 protects nwk key with dTCLK)"
run_thc1
sleep 1
$power_on_dut
sleep 10

grep "Device STARTED" *dut*log
# uncomment line below if DUT can not initiate join retry or DUT application never ends (bug with joining)"
power_off_dut

echo -e "\\nDUT rejoins"
# line below is not needed if line with power_off_dut was commented out
$power_on_dut
sleep 15
grep "Device STARTED OK" *dut*log
power_off_dut

sleep 7
echo -e "\\nDUT join and performs tclk update: negative tests - wait ~ 35 seconds"
$power_on_dut
sleep 35


echo -e "\\n\\nShutdown..."
killch

sh conv.sh

echo 'All done, verify traffic dump, please!'

