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
    kill $zr_pid
    kill $ed_pid
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

rm -f *.log *.pcap *.dump *.nvram

echo "run ns"
../../../../platform/devtools/nsng/nsng &
ns_pid=$!
sleep 2

echo "run DUTZC"
./dutzc &
dutzc_pid=$!
wait_for_start 1_dutzc
echo DUTZC STARTED OK $!

echo "run zr"
./zr &
zr_pid=$!
wait_for_start 3_zr
echo zr STARTED OK $!

echo "run ZED"
./zed &
ed_pid=$!
wait_for_start 4_zed
echo ZED STARTED OK $!

echo "Wait 4 minutes"

sleep 20
kill $zr_pid
echo "zr turned off"
echo "Wait 120 seconds before ZR2 power on"

sleep 120
echo "run zr"
./zr &
zr_pid=$!
wait_for_start 3_zr
echo zr STARTED OK $!

sleep 80

echo 'shutdown...'
killch

sh conv.sh

echo 'All done, verify traffic dump, please!'
