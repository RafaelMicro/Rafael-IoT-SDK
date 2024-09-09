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
    [ -z "$dut1_pid" ] || kill $dut1_pid
    [ -z "$dut2_pid" ] || kill $dut2_pid
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
../../../../platform/devtools/nsng/nsng $wd/nodes_location.cfg &
ns_pid=$!
sleep 2


run_dut1() {
    echo "run DUT ZC (bdbJoinUsesInstallCodeKey = FALSE)"
   ./dut_wo_ic &
   dut1_pid=$!
   wait_for_start dut1
}

run_dut2() {
    echo "run DUT ZC (bdbJoinUsesInstallCodeKey = TRUE)"
   ./dut_using_ic &
   dut2_pid=$!
   wait_for_start dut2
}

run_thr1() {
    echo "run TH ZR1 (bdbJoinUsesInstallCodeKey = TRUE)"
    ./thr1 &
    thr1_pid=$!
}

run_thr2() {
    echo "run TH ZR2 (bdbJoinUsesInstallCodeKey = FALSE)"
    ./thr2 &
    thr2_pid=$!
}


#run_thr1
#sleep 1
#exit 1
#run_dut2
#sleep 2
#grep "DUT:" *dut2.log
#exit 1


echo "Start test"
echo "Joining without using installcodes"
run_dut1
sleep 1
run_thr1
sleep 10

if grep "Device STARTED OK" *thr1.log
then
    mv *thr1.log zdo_thr1_backup1.log
    kill $thr1_pid
    unset thr1_pid
    echo "Reboot DUT"
    kill $dut1_pid
    unset dut1_pid
    run_dut1
    sleep 1
fi

run_thr2
sleep 10
if grep "Device STARTED OK" *thr2.log
then
    echo "THr2 successfully joins to DUT"
    kill $thr2_pid
    unset thr2_pid
else
    echo "THr2 failed to join DUT's network"
fi

kill $dut1_pid
unset dut1_pid

sleep 3
echo -e "\\n\\nJoining using installcodes"
run_dut2
sleep 1
run_thr1
sleep 10

if grep "Device STARTED OK" *thr1.log
then
    mv *thr1.log zdo_thr1_backup2.log
    kill $thr1_pid
    unset thr1_pid
    echo "Reboot DUT"
    kill $dut2_pid
    unset dut2_pid
    run_dut1
    sleep 16
else
# uncomment line below if thr1 does not ends after startup timeout
    [ -z "thr1_pid" ] || kill $thr1_pid && unset thr1_pid
    sleep 6
fi

echo "Entering wrong install code into DUT"
sleep 12

grep "DUT:" *dut2*log

echo "Starting THr1 again: wait 20 seconds"
run_thr1
sleep 20


echo -e "\\n\\nShutdown..."
killch

sh conv.sh

echo 'All done, verify traffic dump, please!'

