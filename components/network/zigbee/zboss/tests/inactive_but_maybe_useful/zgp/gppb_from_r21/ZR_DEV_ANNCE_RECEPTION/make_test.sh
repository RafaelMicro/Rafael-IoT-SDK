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

BUILD_DIRECTORY=`pwd`
ZBOSS_DIRECTORY=${BUILD_DIRECTORY}/../../../../..

echo Change vendor non-GP capable

echo ${ZBOSS_DIRECTORY}

pushd .
cd ${ZBOSS_DIRECTORY}/include
rm zb_vendor.h
ln -s zb_vendor_dsr_r21_dev.h zb_vendor.h
cd ..
make rebuild
popd
make zr

echo Change vendor to GP capable
pushd .
cd ${ZBOSS_DIRECTORY}/include
rm zb_vendor.h
ln -s zb_vendor_dsr_gppb.h zb_vendor.h
cd ..
make rebuild
popd
make

echo Complete
