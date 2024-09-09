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
    kill $end1PID 
    kill $end2PID 
    kill $coordPID 
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

rm -f *nvram *log *pcap *dump

echo "run ns-3"
../../../../platform/devtools/nsng/nsng &
PipePID=$!
sleep 5

echo "run coordinator"
./zc ${PIPE_NAME}0.write ${PIPE_NAME}0.read &
coordPID=$!
wait_for_start zc 
echo ZC STARTED OK $!

echo "run zed1"
./zed1 ${PIPE_NAME}1.write ${PIPE_NAME}1.read &
end1PID=$!
wait_for_start zed1
echo ZED1 STARTED OK $!

echo "run zed2"
./zed2 ${PIPE_NAME}2.write ${PIPE_NAME}2.read &
end2PID=$!
wait_for_start zed2
echo ZED2 STARTED OK $!

sleep 125

echo shutdown...
killch

sh conv.sh

echo "All done, verify traffic, please!"
