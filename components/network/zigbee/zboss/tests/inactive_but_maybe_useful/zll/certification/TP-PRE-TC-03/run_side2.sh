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

rm -f *.log *.dump *.pcap *.nvram ns*.txt *.*~

#run ZR1
read -p "wait run ZR1" inputline

echo "run ZED "
echo "ZR1 must be started"
./zed &
endPID=$!

#work
read -p "wait 40 sec" inputline

echo 'power off zed... '
kill $endPID
mv zed*.log ze_old.log

#run ZR2
read -p "wait run ZR2" inputline

echo "run ZED "
echo "ZR2 must be started"
./zed &
endPID=$!

#work
read -p "wait 40 sec" inputline

echo 'power off zed... '
kill $endPID

echo 'Now verify traffic dump, please!'

if grep "Test finished. Status: OK" zed*.log
then
    echo "DONE. Test Passed OK\n"
else
    echo "Error. Test FAILED\n"
fi
