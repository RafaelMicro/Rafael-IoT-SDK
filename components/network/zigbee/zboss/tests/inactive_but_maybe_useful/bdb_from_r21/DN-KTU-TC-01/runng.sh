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
        return
#        killch
#        exit 1
    fi
}

killch() {
    if [ "$dut_role" = "zed" ]
    then
        [ -z "$dut_zed_pid" ] || kill $dut_zed_pid
    else
        [ -z "$dut_zr_pid" ] || kill $dut_zr_pid
    fi
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
../../../../platform/devtools/nsng/nsng &
ns_pid=$!
sleep 2


echo "run THr1"
./thr1 &
thr1_pid=$!
wait_for_start 1_thr1

sleep 5

if [ "$dut_role" = "zed" ]
then
    echo "run DUT zed (should not start)"
    ./dut_zed &
    dut_zed_pid=$!
elif [ "$dut_role" = "zr" ]
then
    echo "run DUT zr (should not start)"
    ./dut_zr &
    dut_zr_pid=$!
else
    echo "Select dut role (zed or zr)"
    kill $thr1_pid
    kill $ns_pid
    exit 1
fi

echo "Wait 60 seconds"
sleep 60


echo 'shutdown...'
killch

sh conv.sh

echo 'All done, verify traffic dump, please!'

