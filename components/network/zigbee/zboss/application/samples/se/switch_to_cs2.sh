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

HOME=../..

CRYPTOSUITE=2

pushd ${HOME}/application/common/
sed -i "s/#define SE_CRYPTOSUITE_1/#define SE_CRYPTOSUITE_${CRYPTOSUITE}/g" se_common.h
sed -i "s/#define SE_CRYPTOSUITE_2/#define SE_CRYPTOSUITE_${CRYPTOSUITE}/g" se_common.h
popd

ln -srf `pwd`/../pc_spec/24ghz/ihd_cs${CRYPTOSUITE}.prod display_device.prod
ln -srf `pwd`/../pc_spec/24ghz/esi_cs${CRYPTOSUITE}.prod esi_device.prod
ln -srf `pwd`/../pc_spec/24ghz/gasmet_cs${CRYPTOSUITE}.prod gas_device.prod
ln -srf `pwd`/../pc_spec/24ghz/elmet_cs${CRYPTOSUITE}.prod metering_device.prod
ln -srf `pwd`/../pc_spec/24ghz/pct_cs${CRYPTOSUITE}.prod thermostat_device.prod

rm -f *nvram *log *dump *pcap
