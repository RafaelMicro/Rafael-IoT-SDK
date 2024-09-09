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
../../../../platform/devtools/nsng/nsng $wd/nodes_location.cfg &
ns_pid=$!
sleep 2


run_dut() {
    echo "run DUT ZC"
   ./dut &
   dut_pid=$!
   wait_for_start dut
}

run_thr1() {
    echo "run TH ZR1"
    ./thr1 &
    thr1_pid=$!
    wait_for_start thr1
}

run_thr2() {
    echo "run TH ZR2"
    ./thr2 &
    thr2_pid=$!
}


reboot_thr2() {
    [ -z "$thr2_pid" ] || kill $thr2_pid && unset thr2_pid
    run_thr2
}


echo "Start test (wait about 40 minutes for test complete)"
run_dut
sleep 1
run_thr1
sleep 1

echo -e "\\n\\nTHr2 not send Request Key"
run_thr2
sleep 241


echo -e "\\n\\nTHr2 send Request Key unprotected"
reboot_thr2
sleep 241


echo -e "\\n\\nTHr2 send Request Key with MIC error (Several steps)"
reboot_thr2
sleep 241


echo -e "\\n\\nTHr2 send Request Key for NWK Key"
reboot_thr2
sleep 241


echo -e "\\n\\nTHr2 send Request Key for Application Link Key with TC"
reboot_thr2
sleep 241


echo -e "\\n\\nTHr2 send Request Key for Application Link Key with THr1"
reboot_thr2
sleep 241


echo -e "\\n\\nTHr2 not send Verify Key"
reboot_thr2
sleep 241


echo -e "\\n\\nTHr2 fails verification and send out cmd protected with dTCLK"
reboot_thr2
sleep 241


echo -e "\\n\\nCorrect TCLK retention (for multiple devices) after updated TCLK confirmation)"
reboot_thr2
sleep 241


echo -e "\\n\\n\\n"
grep "TEST:" *thr1*log
echo -e "\\n\\n\\n"
grep "TEST:" *thr2*log


echo -e "\\n\\nShutdown..."
killch

sh conv.sh

echo 'All done, verify traffic dump, please!'

