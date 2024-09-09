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
wait_for_start() {
    nm=$1
    start_pattern="${2// /\\s}"

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

        s=`grep "$start_pattern" ${nm}*.log`
    done
    if echo $s | grep $start_pattern
    then
        return
    else
        echo $s
        killch
        exit 1
    fi
}

killch() {
    kill $nsng_pid
    kill $ncp_fw_pid
    kill $host_pid
    kill $th1_pid
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

STACK_ROOT=../../../../..

cd ${STACK_ROOT}/ncp

echo "=========ZDO_STARTUP_ZC HOST========="
echo -e "Working directory: " `pwd` "\n"

rm *.log *.pcap 2>/dev/null

echo -e "Start NS"
../platform/devtools/nsng/nsng &
nsng_pid=$!
echo -e "NS started\n"

sleep 1

echo -e "Start NCP FW"
./ncp_fw &
ncp_fw_pid=$!
wait_for_start "ncp_fw" "zb_nlme_reset_confirm"
echo -e "NCP FW started\n"

sleep 1

echo -e "Start ZC host"
../tests/ncp/host/ctests/zdo_startup/zdo_start_zc &
host_pid=$!
wait_for_start "zdo_zc" "Device started with status 0"
echo -e "ZC host started\n"

sleep 1

echo -e "Start ZR"
../tests/zdo/zdo_startup/zdo_start_zr &
th1_pid=$!
wait_for_start "zdo_zr" "Device STARTED"
echo -e "ZR started\n"

sleep 1

echo -e "Wait for the test completion\n"

sleep 30

echo -e "Test is completed, stop processes"
killch
echo -e "Processes are stopped"
