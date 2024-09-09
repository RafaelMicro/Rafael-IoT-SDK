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
        echo 'wait '${nm}
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
#    [ -z "$dut_pid" ] || kill $dut_pid
#    [ -z "$thr1_pid" ] || kill $thr1_pid
    [ -z "$combo_pid" ] || kill $combo_pid
    [ -z "$combo_dir_pid" ] || kill $combo_dir_pid
    [ -z "$zgpd1_pid" ] || kill $zgpd1_pid
    [ -z "$zgpd2_pid" ] || kill $zgpd2_pid    
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
../../../../platform/devtools/nsng/nsng  &
ns_pid=$!
sleep 1


echo "run ZC (combo)"
./zc_combo &
combo_pid=$!
wait_for_start zc

echo "run ZR (combo_dir)"
./zr_combo_dir &
combo_dir_pid=$!
wait_for_start zr

sleep 2

./zgpd1 &
zgpd1_pid=$!

./zgpd2 &
zgpd2_pid=$!

echo "Wait 40 seconds"
sleep 40

echo 'shutdown...'
killch

sh conv.sh

echo 'All done, verify traffic dump, please!'

