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
        if [ -f core ]
        then
            echo 'Somebody has crashed?'
            killch
            exit 1
        fi
        s=`grep Device zdo_${nm}*.log`
    done
    if echo $s | grep "status 0"
    then
        return
    else
        echo $s
        killch
        exit 1
    fi
}

killch() {
    kill $r_pid $c_pid $ns_pid
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

#sh clean
#make

echo "run network simulator"
../../../../platform/devtools/nsng/nsng &
ns_pid=$!
sleep 1

echo "run coordinator thc1"
./thc1 &
c_pid=$!
wait_for_start thc1
echo ZC STARTED OK

echo "run thr1 "
./thr1 &
r_pid=$!
wait_for_start thr1
echo thr1 STARTED OK

echo "run dut_r "
./dut_r &
r_pid=$!
wait_for_start dut
echo dur_t STARTED OK

echo "wait for 40s"
sleep 40

killch


echo 'Now verify traffic dump, please!'
