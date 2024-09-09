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
    ../../../devtools/dump_converter/dump_converter -n2 $1 ns.pcap
    set - `ls esi_*dump`
    ../../../devtools/dump_converter/dump_converter $1 c.pcap
    set - `ls gas_*dump`
    ../../../devtools/dump_converter/dump_converter $1 r.pcap
    set - `ls esi_zr*dump`
    ../../../devtools/dump_converter/dump_converter $1 esi_r.pcap
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

function print_test_results_awk()
{
    # Check execution of following events:
    # Time synchronization with esi_zc
    # Invoke user's callback to sychronize time
    #
    # gawk -Wposix \
    gawk \
    'BEGIN {
        total = 5;
        passed = 0;
        failed = total;
        passed_mask = 0;
    };
    / Sync time with .* ep 1/ {
        passed += 1;
        failed -= 1;
        passed_mask = or(failed_mask, lshift(1, 0));
        next;
    };
    /Get network time\: / {
        passed += 1;
        failed -= 1;
        passed_mask = or(failed_mask, lshift(1, 1));
        next;
    }
    END {
        print "=======Test Report=======";
        print "Gas device:"
        print "Passed: " passed " from " total;
        print "Failed: " failed;
        print "Passed mask: " passed_mask;
    }' gas_device.log
}


function print_test_results()
{
    passed=0;
    total=11;
    failed=$total;

    echo ""  # print a blank link. It makes pretty output when user interrupts a script

    if grep -e "Sync time with 0x0 ep 1" gas_device.log >/dev/null
    then
        let "passed += 1"
        let "failed -= 1"
    else
        echo "Failed: Gas device: doesn't sync time with esi coordinator(ep 1)"
    fi

    if grep -e "Get network time\: " gas_device.log >/dev/null
    then
        let "passed += 1"
        let "failed -= 1"
    else
        echo "Failed: Gas device: doesn't inform user's application about time synchronization"
    fi

    if grep "The second authoritative time server: ep=5" gas_device.log >/dev/null
    then
        let "passed += 1"
        let "failed -= 1"
    else
        echo "Failed: Gas device: the second most authoritative Time server is chosen incorrectly"
    fi

    if grep "Real Time Clock callback not assigned (use ZB_ZCL_TIME_SET_REAL_TIME_CLOCK_CB) or invalid time value received" gas_device.log >/dev/null
    then
        let "passed += 1"
        let "failed -= 1"
    else
        echo "Failed: Gas device: Can set real time clock at Time attribute writing."
    fi

    if grep "Master bit is set. Time synchronization is not allowed." gas_device.log >/dev/null \
       || grep "Real Time Clock callback not assigned (use ZB_ZCL_TIME_SET_REAL_TIME_CLOCK_CB) or invalid time value received" gas_device.log >/dev/null
    then
        let "passed += 1"
        let "failed -= 1"
    else
        echo "Failed: Gas device: Time attribute shouldn't be written"
    fi

    if grep "Don't write Time Status attribute: Master Bit is set" gas_device.log >/dev/null
    then
        echo "Failed: Gas device should write Time Status attribute"
    else
        let "passed += 1"
        let "failed -= 1"
    fi

    if grep "Sync time with 0x0 ep 1" esi_zr.log >/dev/null
    then
        let "passed += 1"
        let "failed -= 1"
    else
        echo "Failed: ESI router: ESI ZR doesn't sync time with ESI coordinator(ep 1)"
    fi

    if grep -e "Get network time\: " esi_zr.log >/dev/null
    then
        let "passed += 1"
        let "failed -= 1"
    else
        echo "Failed: ESI router: ESI router doesn't inform user's application about time synchronization"
    fi

    if grep "The second authoritative time server: ep=5" esi_zr.log >/dev/null
    then
        let "passed += 1"
        let "failed -= 1"
    else
        echo "Failed: ESI router: the second most authoritative Time server is chosen incorrectly"
    fi

    if grep "Master bit is set. Time synchronization is not allowed." esi_device.log >/dev/null \
       || grep "Real Time Clock callback not assigned (use ZB_ZCL_TIME_SET_REAL_TIME_CLOCK_CB) or invalid time value received" esi_device.log >/dev/null
    then
        let "passed += 1"
        let "failed -= 1"
    else
        echo "Failed: ESI coordinator device: Time attribute shouldn't be written"
    fi

    if grep "Don't write Time Status attribute: Master Bit is set" esi_device.log >/dev/null
    then
        let "passed += 1"
        let "failed -= 1"
    else
        echo "Failed: ESI coordinator device shouldn't write Time Status attribute"
    fi

    echo "========Test Report========"
    echo "Passed $passed from $total"
    echo "Failed $failed"
}


killch() {
    # If pid exists then kill it.

    print_test_results

    # ps -p $watchPID &>/dev/null && kill -s 15 $watchPID
    sleep 1
    kill $endPID $coordPID $PipePID $esi_zrPID
    decode_dump
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}


function watch_test_result()
{
    function print_results()
    {
        echo ""
        echo "======Test Report======="
        echo "Passed: $passed from $total"
        echo "Failed: $failed"
        exit 0
    }

    trap print_results TERM INT

    echo "Watch test results"
    log_name=$1
    s=''
    passed=0
    failed=0
    total=1
    while  (( $total > $passed ))
    do
        sleep 1

        s=$(grep -ir "sync time with" ${log_name}*.log)
        if [ A"$s" != A ] ;
        then
            let "passed += 1"
            echo "Test OK"
        fi

    done

    print_results
}


trap killch_ex TERM INT


rm -f *.nvram *.log *.pcap *.dump core ns.txt /tmp/ztt*

echo "run ns"

#../../platform/devtools/nsng/nsng ../../devtools/nsng/nodes_location.cfg &
../../../platform/devtools/nsng/nsng &
PipePID=$!
sleep 1

echo "run coordinator"
../../../samples/se/energy_service_interface/esi_device &
coordPID=$!
echo $coordPID
wait_for_start esi_device

echo ZC STARTED OK

echo "run zed"
./md/gas_device &
endPID=$!
echo $endPID
wait_for_start gas_device

echo ZR STARTED OK

echo "run esi_zr"
esi_zr/esi_zr &
esi_zrPID=$!
echo $endPID
wait_for_start esi_zr

echo ZR STARTED OK

# watch_test_result gas_device &
# watchPID=$!

# sleep 10

# kill -s 15 $watchPID

#sleep 200
# sleep 350
sleep 80

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch


echo 'Now verify traffic dump, please!'

