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
    ../../devtools/dump_converter/dump_converter -n0 $1 ns.pcap
    set - `ls pl_th_zc*dump`
    ../../devtools/dump_converter/dump_converter $1 c.pcap
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
    kill $coordPID $PipePID $routerPID $devicePID
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

#../../platform/devtools/nsng/nsng ../../devtools/nsng/nodes_location.cfg &
../../platform/devtools/nsng/nsng &
PipePID=$!
sleep 1


echo "run coordinator"
./pl_th_zc &
wait_for_start pl_th_zc
coordPID=$!
echo ZC STARTED OK

echo "run router"
./pl_th_zr &
wait_for_start pl_th_zr
routerPID=$!
echo ZR STARTED OK

echo "run zed"
./pl_dut_zed &
wait_for_start pl_dut_zed
devicePID=$!
echo ZED STARTED OK

sleep 600

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch


echo 'Now verify traffic dump, please!'
