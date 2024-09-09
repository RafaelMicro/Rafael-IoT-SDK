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
# PURPOSE: Script for running light samples in NSNG (on-place! not in SDK!)

HOME_DIR=`pwd`

decode_dump() {
    set - `ls NS*dump`
    $HOME_DIR/../../devtools/dump_converter/dump_converter $1 ns.pcap
    set - `ls light_zc.dump`
    $HOME_DIR/../../devtools/dump_converter/dump_converter $1 light_zc.pcap
    set - `ls light_control.dump`
    $HOME_DIR/../../devtools/dump_converter/dump_converter $1 light_control.pcap
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
    kill $endPID1 $endPID2 $coordPID $PipePID
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
$HOME_DIR/../../platform/devtools/nsng/nsng &
PipePID=$!
sleep 1

echo "run coordinator"
$HOME_DIR/light_coordinator/light_zc &
coordPID=$!
echo $coordPID
wait_for_start light_zc

echo ZC STARTED OK

sleep 5

echo "run zr"
$HOME_DIR/dimmable_light/bulb &
endPID1=$!
echo $endPID1
wait_for_start bulb

echo ZR STARTED OK

sleep 5

echo "run zed"
$HOME_DIR/light_control/light_control &
endPID2=$!
echo $endPID2
wait_for_start light_control

echo ZED STARTED OK

sleep 90

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch


echo 'Now verify traffic dump, please!'

