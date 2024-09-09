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
    kill $dut_pid
    kill $ns_pid

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


if [ "$dut_role" = "zc" ]
then
    echo "run DUT ZC"
    ./dut_c &
    dut_pid=$!
    wait_for_start dut
    echo DUT ZC STARTED OK $!

elif [ "$dut_role" = "zr" ]
then
    echo "run DUT ZR"
    ./dut_r &
    dut_pid=$!
    wait_for_start dut
    echo DUT ZR STARTED OK $!

elif [ "$dut_role" = "zed" ]
then
    echo "run DUT ZED"
    ./dut_ed &
    dut_pid=$!
    wait_for_start dut
    echo DUT ZED STARTED OK $!

else
    echo "Select dut role (zc, zr or zed)"
    kill $ns_pid
    exit 1
fi


echo "wait for 30 seconds"
sleep 30

echo 'shutdown...'
killch

if grep "TEST ERROR" *dut*.log
then
  echo "ERROR. TEST FAILED!!!"
fi

sh conv.sh

echo 'All done, verify traffic dump, please!'

