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

# Retrieve DUT's role
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
    kill $ns_pid

    if [ "$dut_role" = "zc" ]
    then
        kill $dutzc_pid
	kill $gzed_pid
    elif [ "$dut_role" = "zr" ]
    then
        kill $dutzr_pid
	kill $gzc_pid
	kill $gzed_pid
    elif [ "$dut_role" = "zed" ]
    then
        kill $dutzed_pid
	kill $gzc_pid
    fi

    kill $gzr1_pid
    kill $gzr2_pid
    kill $gzr3_pid
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
    echo "run DUT ZC"
    ./dutzc &
    dutzc_pid=$!
    wait_for_start 1_dutzc
    echo DUT ZC STARTED OK

elif [ "$dut_role" = "zr" ]
then
    echo "run gZC"
    ./gzc &
    gzc_pid=$!
    wait_for_start 2_gzc
    echo gZC STARTED OK


    echo "run DUT ZR"
    ./dutzr &
    dutzr_pid=$!
    wait_for_start 1_dutzr
    echo DUT ZR STARTED OK
elif [ "$dut_role" = "zed" ]
then
    echo "run gZC"
    ./gzc &
    gzc_pid=$!
    wait_for_start 2_gzc
    echo gZC STARTED OK


    echo "run DUT ZED"
    ./dutzed &
    dutzed_pid=$!
    wait_for_start 1_dutzed
    echo DUT ZED STARTED OK
else
    echo "Select dut role (zc, zr or zed)"
    kill $ns_pid
    exit 1
fi


if [ "$dut_role" != "zed" ]
then
    echo "run gZED"
    ./gzed &
    gzed_pid=$!
    wait_for_start 6_gzed
    echo gZED STARTED OK
fi


echo "run gZR1"
./gzr1 &
gzr1_pid=$!
wait_for_start 3_gzr1
echo "gZR1 STARTED OK"


echo "run gZR2"
./gzr2 &
gzr2_pid=$!
wait_for_start 4_gzr2
echo "gZR2 STARTED OK"


echo "run gZR3"
./gzr3 &
gzr3_pid=$!
wait_for_start 5_gzr3
echo "gZR3 STARTED OK"


echo 'Please wait for test complete (60 seconds)...'
sleep 160


echo 'shutdown...'
killch

sh conv.sh $1

echo 'All done, verify traffic dump, please!'

