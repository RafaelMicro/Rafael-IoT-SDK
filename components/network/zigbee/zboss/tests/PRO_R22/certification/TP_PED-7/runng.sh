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
    kill $dutzc_pid
    kill $gzed1_pid
    kill $gzed2_pid
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
../../../../platform/devtools/nsng/nsng $wd/nodes_location.cfg &
ns_pid=$!
sleep 2


echo "run DUT ZC"
./dutzc &
dutzc_pid=$!
wait_for_start 1_dutzc
echo DUT ZC STARTED OK



echo "run gZED1"
./gzed1 &
gzed1_pid=$!
wait_for_start 2_gzed1
echo gZED1 STARTED OK


echo "run gZED2"
./gzed2 &
gzed2_pid=$!
wait_for_start 3_gzed2
echo gZED2 STARTED OK

echo 'Please wait for test complete (210 seconds)...'
sleep 20

kill $gzed2_pid
echo "gZED2 turned off"
sleep 200


echo 'shutdown...'
killch

sh conv.sh

echo 'All done, verify traffic dump, please!'

