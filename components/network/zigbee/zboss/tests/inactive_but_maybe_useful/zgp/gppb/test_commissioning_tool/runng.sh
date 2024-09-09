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

killct() {
    [ -z "$ct_pid" ] || kill $ct_pid
}

killch() {
    [ -z "$combo_pid" ] || kill $combo_pid
    [ -z "$proxy_pid" ] || kill $proxy_pid
    [ -z "$zgpd_pid" ] || kill $zgpd_pid
    [ -z "$ns_pid" ] || kill $ns_pid

    sh conv.sh
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

make clean
make rebuild

rm -f *log *dump *pcap *nvram

wd=`pwd`
echo "run ns"
../../../../platform/devtools/nsng/nsng  &
ns_pid=$!
sleep 1


echo "run ZC (combo)"
./zc_combo &
combo_pid=$!
wait_for_start zdo_zc

echo "run Proxy (combo)"
./proxy &
proxy_pid=$!
wait_for_start proxy

sleep 2

echo "run CT (commissioning tool)"
./ct &
ct_pid=$!
wait_for_start zdo_ct

sleep 2

killct

echo "run zgpd"
./zgpd &
zgpd_pid=$!

echo "Wait 30 seconds"
sleep 30

echo 'shutdown...'
killch

sh conv.sh

echo 'All done, verify traffic dump, please!'

