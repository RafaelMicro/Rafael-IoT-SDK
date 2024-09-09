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
# PURPOSE: Create links to tests


cat tests_list | grep '_dut_' | while read role name src
do
  d=`dirname $src`
  t=`basename $d`
  n=`echo $name | sed -e "s/${t}_//"`
  if [ $role = ZC ]
  then
     ln -f r21_zczr ../${t}/$n
  else
     ln -f r21_zgpd ../${t}/$n
  fi
done

cat tests_list | grep '_th_' | while read role name src
do
  d=`dirname $src`
  t=`basename $d`
  n=`echo $name | sed -e "s/${t}_//"`
  if [ $role = ZC ]
  then
     ln -f r21_zczr_th ../${t}/$n
  else
     ln -f r21_zgpd_th ../${t}/$n
  fi
done
