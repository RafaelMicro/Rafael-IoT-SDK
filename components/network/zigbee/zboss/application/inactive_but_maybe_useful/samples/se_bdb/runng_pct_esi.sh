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

decode_dump() {
    set - `ls NS*dump`
    ../../devtools/dump_converter/dump_converter -n2 $1 ns.pcap
    set - `ls esi_*dump`
    ../../devtools/dump_converter/dump_converter $1 c.pcap
    set - `ls display_*dump`
    ../../devtools/dump_converter/dump_converter $1 r.pcap
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
    kill $endPID $coordPID $PipePID
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

#../../devtools/nsng/nsng ../../devtools/nsng/nodes_location.cfg &
../../devtools/nsng/nsng &
PipePID=$!
sleep 1

echo "run coordinator"
energy_service_interface/esi_device &
coordPID=$!
wait_for_start esi_device

echo ZC STARTED OK

echo "run zed"
thermostat/thermostat_device &
endPID=$!
wait_for_start thermostat_device

echo ZED STARTED OK

sleep 600

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch


echo 'Now verify traffic dump, please!'

