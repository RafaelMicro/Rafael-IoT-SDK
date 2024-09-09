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

gzr_failed_start_allowed=0

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
	if [ $gzr_failed_start_allowed -eq 0 ]
	then
            echo $s
            killch
            exit 1
	fi
    fi
}

killch() {
    kill $ns_pid
    kill $gzc1_pid $gzc2_pid $gzc3_pid
    kill $gzed_pid
    kill $dutzr_pid
}

killch_ex() {
    kill $gzr_pid
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
../../../../platform/devtools/nsng/nsng &
ns_pid=$!
sleep 2


echo "run gZC1 - channel #11"
./gzc1 &
gzc1_pid=$!
wait_for_start 1_gzc1
echo gZC1 STARTED OK

echo "run gZC2 - channel #20"
./gzc2 &
gzc2_pid=$!
wait_for_start 2_gzc2
echo gZC2 STARTED OK

echo "run gZC3 - channel #25"
./gzc3 &
gzc3_pid=$!
wait_for_start 3_gzc3
echo gZC3 STARTED OK


echo "Starting DUT ZR: scanning channels"
./dutzr &
dutzr_pid=$!
wait_for_start 4_dutzr
echo DUT ZR STARTED OK

echo "Wait 30 seconds"
sleep 30


gzr_failed_start_allowed=1
echo "Optional step: starting gZR - send MAC-DATA without dest addr"
./gzr &
gzr_pid=$!
wait_for_start 5_gzr
gzr_failed_start_allowed=0

echo "Wait 40 seconds"
sleep 30
kill $gzr_pid
sleep 10


echo "Starting gZR - joining to DUT ZR"
./gzr &
gzr_pid=$!
sleep 1
wait_for_start 5_gzr
echo gZR STARTED OK

echo "Wait 30 seconds"
sleep 30


echo "Turn off gZR"
kill $gzr_pid
sleep 2


echo "Starting gZED - joining to DUT ZR"
./gzed &
gzed_pid=$!
wait_for_start 6_gzed
echo gZED STARTED OK


echo "Wait 10 seconds"
sleep 10


echo 'shutdown...'
killch

sh conv.sh

echo 'All done, verify traffic dump, please!'

