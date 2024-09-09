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
        kill $dutzc_pid $gzr1_c_pid
    elif [ "$dut_role" = "zr" ]
    then
        kill $dutzr_pid $gzr1_d_pid
    elif [ "$dut_role" = "zed" ]
    then
        kill $dutzed_pid $gzr1_d_pid
    fi

    #sh conv.sh $dut_role
}

killch_ex() {
    killch
    kill $gzr2_pid
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

#sh clean
#make

rm -f *log *dump *pcap *nvram

wd=`pwd`
echo "run ns"
#../../../../platform/devtools/nsng/nsng $wd/nodes_location.cfg &
../../../../platform/devtools/nsng/nsng &
ns_pid=$!
sleep 2


start_gzr1_c() {
    echo "run gZR1 (For Centralized Network)"
    ./gzr1_c &
    gzr1_c_pid=$!
    wait_for_start 2_gzr1_c
    echo gZR1 STARTED OK $!
}

start_gzr1_d() {
    echo "run gZR1 (For Distributed Network)"
    ./gzr1_d &
    gzr1_d_pid=$!
    wait_for_start 2_gzr1_d
    echo gZR1 STARTED OK $!
}


start_gzr2() {
    echo "run gZR2"
    ./gzr2 &
    gzr2_pid=$!
    wait_for_start 3_gzr2
    echo gZR2 STARTED OK $!
}



if [ "$dut_role" = "zc" ]
then
    echo "run DUT ZC"
    ./dutzc &
    dutzc_pid=$!
    wait_for_start 1_dutzc
    echo DUT ZC STARTED OK

    start_gzr1_c
    start_gzr2

    echo "Wait for 35 seconds"
    sleep 35
    kill $gzr2_pid
    echo "gZR2 turned off"
    echo "Wait for 40 seconds"
    sleep 40

elif [ "$dut_role" = "zr" ]
then
    start_gzr1_d

    echo "run DUT ZR"
    ./dutzr &
    dutzr_pid=$!
    wait_for_start 1_dutzr
    echo DUT ZR STARTED OK

    start_gzr2

    echo "Wait for 60 seconds"
    sleep 60
    kill $gzr2_pid
    echo "gZR2 turned off"
    echo "Wait for 40 seconds"
    sleep 40

elif [ "$dut_role" = "zed" ]
then
    start_gzr1_d

    echo "run DUT ZED"
    ./dutzed &
    dutzed_pid=$!
    wait_for_start 1_dutzed
    echo DUT ZED STARTED OK

    start_gzr2
    echo "Wait for 60 seconds"
    sleep 60
else
    echo "Select dut role (zc, zr or zed)"
    kill $ns_pid
    exit 1
fi


echo 'shutdown...'
killch

#sh conv.sh $dut_role


echo 'All done, verify traffic dump, please!'

