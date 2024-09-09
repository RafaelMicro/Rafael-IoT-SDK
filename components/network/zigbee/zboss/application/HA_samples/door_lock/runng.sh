#!/bin/bash
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
# PURPOSE: Script for running HA samples in NSNG

HOME_DIR=`pwd`

decode_dump() {
    set - `ls NS*dump`
    ../../../devtools/dump_converter/dump_converter $1 ns.pcap
    set - `ls sample_zc.dump`
    ../../../devtools/dump_converter/dump_converter $1 sample_zc.pcap
}


wait_for_start() {
    nm=$1
    s=''
    while [ A"$s" = A ]
    do
        sleep 1
        if [ -f core ]
        then
            echo 'Somebody has crashed (ns?)'
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

killch() {
    kill $endPID1 $coordPID $PipePID
    decode_dump
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT


rm -f *.nvram *.log *.pcap *.dump core ns.txt /tmp/ztt*

echo "run ns"

#$HOME_DIR/nsng nodes_location.cfg &
../../../platform/devtools/nsng/nsng &
PipePID=$!
sleep 1

echo "run coordinator"
$HOME_DIR/sample_zc &
coordPID=$!
echo $coordPID
wait_for_start sample_zc

echo ZC STARTED OK

echo "run zed"
$HOME_DIR/sample_zed &
endPID1=$!
echo $endPID1
wait_for_start sample_zed

echo ZED STARTED OK

sleep 360

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch


echo 'Now verify traffic dump, please!'

