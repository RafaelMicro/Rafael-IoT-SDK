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
    ../../../devtools/dump_converter/dump_converter $1 ns.pcap
    rm -f $1
    set - `ls *zc*dump`
    ../../../devtools/dump_converter/dump_converter $1 zc.pcap
    rm -f $1
    set - `ls *zed1*dump`
    ../../../devtools/dump_converter/dump_converter $1 zed1.pcap
    rm -f $1
#    set - `ls *zed2*dump`
#    ../../../devtools/dump_converter/dump_converter $1 ze2.pcap
}

wait_for_start() {
    nm=$1
    s=''
    while [ A"$s" = A ]
    do
        sleep 1
        if [ -f core ]
        then
            echo 'Somebody has crashed?'
            killch
            exit 1
        fi
        s=`grep Device zb_${nm}*.log`
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
    kill $c_pid $ed1_pid $ed2_pid $ns_pid
    decode_dump
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

rm -f *.log *.pcap *.dump core ns.txt /tmp/ztt* *.nvram

echo "run network simulator"
#../../platform/devtools/nsng/nsng ../../devtools/nsng/nodes_location.cfg &
../../../platform/devtools/nsng/nsng &
ns_pid=$!
sleep 1

echo "run coordinator"
./zb_zc /tmp/znode0.write /tmp/znode0.read &
c_pid=$!
echo $c_pid
wait_for_start zc
echo ZC STARTED OK

echo "run dut zed"
./zb_zed /tmp/znode1.write /tmp/znode1.read &
ed1_pid=$!
echo $ed1_pid
wait_for_start zed
echo ED1 STARTED OK

sleep 100

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo shutdown...
killch

echo 'Now verify traffic dump, please!'
