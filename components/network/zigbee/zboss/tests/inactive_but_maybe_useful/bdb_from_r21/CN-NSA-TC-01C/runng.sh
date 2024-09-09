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
    [ -z "$thr1_pid" ] || kill $thr1_pid
    [ -z "$the1_pid" ] || kill $the1_pid
    [ -z "$ns_pid" ] || kill $ns_pid

    sh conv.sh $dut_role
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


if [ "$dut_role" = "zc" ]
then
    echo "run DUT ZC (forming network)"
    ./dut_c &
    dut_pid=$!
    wait_for_start dut
    echo DUT ZC STARTED OK
elif [ "$dut_role" = "zr" ]
then
    echo "run DUT ZR (forming network)"
    ./dut_r &
    dut_pid=$!
    wait_for_start dut
    echo DUT ZR STARTED OK
else
    echo "Select dut role (zc zr)"
    kill $ns_pid
    exit 1
fi

sleep 1

echo "run TH ZED1 (joining network)"
./the1 &
the1_pid=$!
wait_for_start the1
echo TH ZED1 STARTED OK

sleep 1

echo "run TH ZR1 (joining network)"
./thr1 &
thr1_pid=$!
wait_for_start thr1
echo TH ZR1 STARTED OK

sleep 1

echo "Wait 6 minutes"
sleep 360


echo 'shutdown...'
killch

sh conv.sh $dut_role

echo 'All done, verify traffic dump, please!'

