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

PIPE_NAME=&

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
    kill $dutzc_pid
    kill $gzed1_pid
    kill $gzed2_pid
    kill $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT


#sh clean
#make

echo "run ns"
../../../../platform/devtools/nsng/nsng &
PipePID=$!
sleep 3


echo "run DUT ZC"
./dutzc ${PIPE_NAME}0.write ${PIPE_NAME}0.read &
dutzc_pid=$!
wait_for_start 0_dutzc
echo ZC STARTED OK $!
sleep 1

echo "run gZED1"
./gzed1 ${PIPE_NAME}1.write ${PIPE_NAME}1.read &
gzed1_pid=$!
wait_for_start 1_gzed1
echo gZED1 STARTED OK $!
sleep 1

echo "run gZED2"
./gzed2 ${PIPE_NAME}2.write ${PIPE_NAME}2.read &
gzed2_pid=$!
wait_for_start 2_gzed2
echo gZED2 STARTED OK $!

sleep 120

echo shutdown...
killch

sh conv.sh

echo 'Now verify traffic dump, please!'


#if grep "Test status: OK" zdo_2_zed1*.log
#then
#  echo "DONE. TEST PASSED!!!"
#else
#  echo "ERROR. TEST FAILED!!!"
#fi


