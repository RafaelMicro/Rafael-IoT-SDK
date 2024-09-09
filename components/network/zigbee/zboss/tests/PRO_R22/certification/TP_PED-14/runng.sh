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
declare -a gzed_pids

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

start_gzr_c() {
    echo "run gZR"
    ./gzr_c &
    gzr_pid=$!
    wait_for_start 2_gzr
    echo gZR STARTED OK
}

start_gzr_d() {
    echo "run gZR"
    ./gzr_d &
    gzr_pid=$!
    wait_for_start 2_gzr
    echo gZR STARTED OK
}

killch() {
    if [ "$dut_role" = "zc" ]
    then
        kill $dutzc_pid
    elif [ "$dut_role" = "zr" ]
    then
        kill $dutzr_pid
        kill $gzr_pid
    fi

    kill ${gzed_pids[*]}

    kill $ns_pid
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
#../../../../platform/devtools/nsng/nsng $wd/nodes_location.cfg &
../../../../platform/devtools/nsng/nsng  &
ns_pid=$!
sleep 2


if [ "$dut_role" = "zc" ]
then
    echo "run DUT ZC"
    ./dutzc &
    dutzc_pid=$!
    wait_for_start 1_dutzc
    echo DUT ZC STARTED OK

    start_gzr_c

elif [ "$dut_role" = "zr" ]
then
    start_gzr_d

    echo "run DUT ZR"
    ./dutzr &
    dutzr_pid=$!
    wait_for_start 1_dutzr
    echo DUT ZR STARTED OK
else
    echo "Select dut role (zc or zr)"
    kill $ns_pid
    exit 1
fi


for i in `seq 0 24`
do
    echo run gZED$i
    ./gzed_var_ieee $i &
    gzed_pids[i]=$!
    wait_for_start 3_gzed_$i
    echo gZED$i STARTED OK
    sleep 2

    kill ${gzed_pids[$i]}
done

sleep 5
echo "Power off DUT"
if [ "$dut_role" = "zc" ]
then
    kill $dutzc_pid

elif [ "$dut_role" = "zr" ]
then
    kill $dutzr_pid
fi



sleep 5
echo "Power up DUT"
if [ "$dut_role" = "zc" ]
then
    ./dutzc &
    dutzc_pid=$!
    wait_for_start 1_dutzc

elif [ "$dut_role" = "zr" ]
then
    ./dutzr &
    dutzr_pid=$!
    wait_for_start 1_dutzr
fi


echo 'Wait 3 minutes'
sleep 180


echo 'shutdown...'
killch

sh conv.sh $1

echo 'All done, verify traffic dump, please!'

