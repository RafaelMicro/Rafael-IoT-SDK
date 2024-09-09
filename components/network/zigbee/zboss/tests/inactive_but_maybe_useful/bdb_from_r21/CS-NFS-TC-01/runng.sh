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

dut_role="$1"

wait_for_start() {
    nm=$1
    s=''
    while [ A"$s" = A ]
    do
        sleep 1
        if [ -f core ]
        then
            echo 'Somebody has crashed'
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
    [ -z "$dut_pid" ] || kill $dut_pid
    [ -z "$thc1_pid" ] || kill $thc1_pid
    [ -z "$ns_pid" ] || kill $ns_pid

    sh conv.sh
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

#sh clean
#make

rm -f *log *dump *pcap *nvram

wd=`pwd`
echo "run ns"
../../../../platform/devtools/nsng/nsng $wd/nodes_location.cfg &
ns_pid=$!
sleep 2


run_dut_r() {
    echo "run DUT ZR"
   ./dut_r &
   dut_pid=$!
}

run_dut_ed() {
    echo "run DUT ZED"
   ./dut_ed &
   dut_pid=$!
}

run_thc1() {
    echo "run TH ZC1"
    ./thc1 &
    thc1_pid=$!
    wait_for_start thc1
}


reboot_devices() {
    [ -z "$thc1_pid" ] || kill $thc1_pid && unset thc_pid
    run_thc1
    sleep 1
    [ -z "$dut_pid" ] || kill $dut_pid &&  unset dut_pid
    $power_on_dut
}


if [ "$dut_role" = "zr" ]
then
    echo "DUT is ZR"
    power_on_dut="run_dut_r"
elif [ "$dut_role" = "zed" ]
then
    echo "DUT is ZED"
    power_on_dut="run_dut_ed"
else
    echo "Select dut role (zr or zed)"
    killch
    exit 1
fi


echo "Start test (wait about 10 minutes for test complete)"
run_thc1
sleep 2


echo -e "\\n\\nZC Drops Request Key"
run_dut_r
sleep 30


echo -e "\\n\\nZC send Transport Key with new TC LK unencrypted"
reboot_devices
sleep 30


echo -e "\\n\\nZC send Transport Key with new TC LK with Wrong protection level"
reboot_devices
sleep 30


echo -e "\\n\\nZC send Transport Key with new TC LK with wrong TC address"
reboot_devices
sleep 30


echo -e "\\n\\nZC send Transport Key with new TC LK not for DUT"
reboot_devices
sleep 30


echo -e "\\n\\nZC send Transport Key with new TC LK with wrong MIC"
reboot_devices
sleep 30


echo -e "\\n\\nZC send Transport Key with new TC LK with Data Key protection"
reboot_devices
sleep 30


echo -e "\\n\\nZC send Transport Key with new TC LK with KeyTransport Key protection"
reboot_devices
sleep 30


echo -e "\\n\\nZC send Transport Key with new TC LK with Network Key protection"
reboot_devices
sleep 30


echo -e "\\n\\nZC send Transport Key with new TC LK with old APS security frame counter"
reboot_devices
sleep 30


echo -e "\\n\\nZC send Transport Key with application link key"
reboot_devices
sleep 30


echo -e "\\n\\nZC send Transport Key with network key"
reboot_devices
sleep 30


echo -e "\\n\\nZC send Transport Key with dTCLK"
reboot_devices
sleep 60


echo -e "\\n\\nZC send Confirm Key protected with dTCLK"
reboot_devices
sleep 30


echo -e "\\n\\nZC drops Confirm Key"
reboot_devices
sleep 30


echo -e "\\n\\nSuccessful Commissioning"
reboot_devices
sleep 30


echo -e "\\n\\n\\n"
grep "TEST:" *thc1*log


echo -e "\\n\\nShutdown..."
killch

sh conv.sh

echo 'All done, verify traffic dump, please!'

