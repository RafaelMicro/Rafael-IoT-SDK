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
# PURPOSE: Compile multitest for gppb

ZGP_MULTITEST_BUILD_DIRECTORY=`pwd`
ZBOSS_DIRECTORY=${ZGP_MULTITEST_DIRECTORY}../../../../

echo Change options

echo ${ZBOSS_DIRECTORY}

pushd .
cd ${ZBOSS_DIRECTORY}
rm Options
ln -s build-configurations/Options-gen-tests-list Options
popd
pushd .
cd testbin
echo Generating lists
sh genlist.sh
popd

echo Change options to TestHarness
pushd .
cd ${ZBOSS_DIRECTORY}
rm Options
ln -s build-configurations/Options-linux-debug-th Options
echo Build STACK for TestHarness
make rebuild
popd
echo Make test harness
pushd .
cd testbin
make clean
make th
popd

echo Change options
pushd .
cd ${ZBOSS_DIRECTORY}
rm Options
ln -s build-configurations/Options-linux-debug Options
echo Build STACK
make rebuild
popd
echo Make test
pushd .
cd testbin
make clean
make
popd

echo Complete
