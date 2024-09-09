#!/bin/sh
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
# Purpose:
# */

wait_for_start() {
    nm=$1
    s=''
    while [ A"$s" = A ]
    do
        sleep 2
        echo 'wait '${nm}
        if [ -f core ]
        then
            echo 'Somebody has crashed'
            killch
            exit 1
        fi
        s=`grep Device ${nm}.log`
	if echo ${s} | grep OK
    	then
            return
    	else
	    #stil wait ok
            echo $s
            s=''
        fi
    done
}

killch() {
    [ -z "$combo_pid" ] || kill $combo_pid
    [ -z "$proxy_pid" ] || kill $proxy_pid
    [ -z "$zgpd_pid" ] || kill $zgpd_pid
    [ -z "$ns_pid" ] || kill $ns_pid

    #sh conv.sh
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

rm -f *log *dump *pcap *nvram

wd=`pwd`
echo "run ns"
../../platform/devtools/nsng/nsng  &
ns_pid=$!
sleep 1


echo "run ZC (sink)"
./zc_combo_skip &
combo_pid=$!
wait_for_start zc_combo

### Run proxy if you need

echo "run ZR (proxy)"
./zr_proxy &
proxy_pid=$!
wait_for_start zr_proxy

echo "run ZGPD"
./device &
zgpd_pid=$!
wait_for_start zgpd

echo "Wait 30 seconds"
sleep 30

echo 'shutdown...'
killch

#sh conv.sh

echo 'All done, verify traffic dump, please!'
