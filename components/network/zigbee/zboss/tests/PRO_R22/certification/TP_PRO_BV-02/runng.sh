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

echo "run coordinator"
./zc &
c_pid=$!
wait_for_start 1_zc
echo ZC STARTED OK

echo "run dut zed1"
./zed1 &
ed1_pid=$!
wait_for_start 2_zed1
echo ED1 STARTED OK

echo "run dut zed2"
./zed2 &
ed2_pid=$!
wait_for_start 3_zed2
echo ED2 STARTED OK

sleep 70

killch

if grep "Device STARTED OK" zdo_2_zed1*.log
then
  if grep "Device STARTED OK" zdo_3_zed2*.log
  then
    echo "DONE. TEST PASSED!!!"
  else
    echo "ERROR. TEST FAILED!!!"
  fi
else
  echo "ERROR. TEST FAILED!!!"
fi

sh conv.sh

echo 'Now verify traffic dump, please!'
