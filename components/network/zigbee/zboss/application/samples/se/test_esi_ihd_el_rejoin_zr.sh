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
# PURPOSE: run ESI+IHD+EL_MET combination on nsng
#

$STACK_HOME=../../..

decode_dump() {
    set - `ls NS*dump`
    $STACK_HOME/devtools/dump_converter/dump_converter -n2 $1 ns.pcap
    set - `ls esi_*dump`
    $STACK_HOME/devtools/dump_converter/dump_converter $1 c.pcap
    set - `ls display_*dump`
    $STACK_HOME/devtools/dump_converter/dump_converter $1 display.pcap
    set - `ls el_*dump`
    $STACK_HOME/devtools/dump_converter/dump_converter $1 el.pcap
}


wait_for_start() {
    nm=$1
    s=''
    while [ A"$s" = A ]
    do
        sleep 1
        if [ -f core* ]
        then
            echo 'Somebody has crashed!'
            killch
            exit 1
        fi
        s=`egrep "Device.*STARTED|Device.*FAILED" ${nm}*.log`
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

do_clean=1
if [ "$1" = '-reboot' ] ; then
    do_clean=0
fi

if [ $do_clean -ne 0 ] ; then
    rm -f *.nvram *.log *.pcap *.dump core*
else
    echo Proceeding without nvram clear
    rm -f *.log *.pcap *.dump core*
fi

echo "run ns"

$STACK_HOME/platform/devtools/nsng/nsng &
PipePID=$!
sleep 1

echo "run coordinator"
energy_service_interface/esi_device&
wait_for_start esi_device
coordPID=$!
echo ZC STARTED OK

echo "run metering zed"
metering/metering_device &
endPID1=$!
wait_for_start metering_device
echo ZED STARTED OK

echo "run ihd zr"
in_home_display/display_device &
endPID2=$!
wait_for_start display_device
echo ZR STARTED OK

sleep 120

echo "Shutdown ZR..."
kill $endPID2

sleep 30

echo "Restarting ZR..."
in_home_display/display_device &
endPID2=$!
wait_for_start display_device
echo ZR STARTED OK

sleep 60

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch

echo 'Now verify traffic dump, please!'

