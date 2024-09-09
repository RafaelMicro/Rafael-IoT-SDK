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
    [ -z "$dut_pid" ] || kill $dut_pid
    [ -z "$thr1_pid" ] || kill $thr1_pid
    [ -z "$proxy_pid" ] || kill $proxy_pid
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


echo "run ZC (sink)"
./zc_combo &
dut_pid=$!
wait_for_start zc

echo "run ZR (proxy)"
./zr_proxy &
proxy_pid=$!
wait_for_start zr

sleep 2

#~/LW_ZGPD/zgpd_on_off/zgpd &
#../../custom/zgpd_secur_unidir_onoff/device &
../../custom/zgpd_secur_bidir_onoff/device &

thr1_pid=$!

echo "Wait 30 seconds"
sleep 30

echo 'shutdown...'
killch

sh conv.sh

echo 'All done, verify traffic dump, please!'

