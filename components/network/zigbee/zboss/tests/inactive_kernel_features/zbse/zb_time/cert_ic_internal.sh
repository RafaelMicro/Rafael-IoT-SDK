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

CERT_FILE=./common/se_cert_internal.h
IC_FILE=./common/se_ic_internal.h

if [ ! -f $CERT_FILE ] ; then
  err_exit "No certificates file $CERT_FILE exists"
fi

if [ ! -f $IC_FILE ] ; then
  err_exit "No installcodes file $IC_FILE exists"
fi

ln -srf $CERT_FILE ./common/se_cert.h
ln -srf $IC_FILE ./common/se_ic.h
