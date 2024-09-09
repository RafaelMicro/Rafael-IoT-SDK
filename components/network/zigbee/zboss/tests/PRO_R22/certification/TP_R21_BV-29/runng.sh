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
        s=`grep Device ${nm}*.log`
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

start_gzc() {
    echo "run gZC"
    ./gzc &
    gzc_pid=$!
    wait_for_start gzc
    echo gZC STARTED OK
}

killch() {
    kill $dutzr_pid
    kill $gzc_pid
    kill $ns_pid
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

sh clean
rm -f *log *dump *pcap *nvram
make -C ../../../../
make


wd=`pwd`
echo "run ns"
../../../../platform/devtools/nsng/nsng $wd/nodes_location.cfg &
ns_pid=$!
sleep 2


start_gzc

echo "run DUT ZR"
./dutzr &
dutzr_pid=$!
wait_for_start dutzr
echo DUT ZR STARTED OK

echo 'Please wait for test complete (20 seconds)...'
sleep 20


echo 'shutdown...'
killch

sh conv.sh

echo 'All done, verify traffic dump, please!'

