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
# PURPOSE: Build multitest using only choosed groups.

# Pass here test groups as parameters (./build_groups.sh APS BDB HA)

BUILD_GROUPS="$@"

print_zboss_copyright_sh() {

    echo '#!/bin/sh
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
# Auto-generated file! Do not edit!
'

}

if [[ "`echo ${BUILD_GROUPS[*]}`" == "" ]] ; then
    make rebuild
    exit 0
fi

cp srcs_list srcs_list.tmp
cp zb_multi_test_main.c zb_multi_test_main.tmp

# Remove defines for unused groups
ALL_GROUPS=`grep "#define ZB_REG_TEST_GROUP_" zb_multi_test_main.c | sed 's:#define ZB_REG_TEST_GROUP_::g' | tr '\n' ' '`

echo "Selected groups: ${BUILD_GROUPS[*]}"
echo
echo "All available groups: ${ALL_GROUPS[@]}"
echo

set ${ALL_GROUPS}
GROUPS_NUM=$#

count=0
for group in ${ALL_GROUPS[@]}
do
    if [[ "`echo ${BUILD_GROUPS[*]} | grep -w ${group}`" == "" ]] ; then
        echo "Removed group: ZB_REG_TEST_GROUP_${group}"
        count=$((count+1))
        sed -i '/\#define ZB_REG_TEST_GROUP_'${group}'/d' zb_multi_test_main.c
    fi
done

if [[ ${count} -eq ${GROUPS_NUM} ]] ; then

    echo "No valid test groups!"

    rm -rf zb_multi_test_main.c srcs_list
    mv zb_multi_test_main.tmp zb_multi_test_main.c
    mv srcs_list.tmp srcs_list

    exit 1
fi

# Generate new src_list
(
    print_zboss_copyright_sh

    echo 'SRCS_ZCZR = \'
    cat tests_list |  sed -ne '/^ZC/s/[^ ]*[ ][^ ]*[ ]\(.*\)$/\1 \\/p' | sed -e 's/\.\.\/common\/zb_cert_test_globals.c[[:space:]]*//' | sed -e 's/\.\.\/common\/zb_test_step_control.c[[:space:]]*//' | sed -e 's/\.\.\/common\/zb_test_step_storage.c[[:space:]]*//' | grep -e `echo ${BUILD_GROUPS[@]} | sed 's: : -e :g'`
    echo 'zb_nvram_erase.c \'

    echo
    echo

    echo 'SRCS_ZED = \'
    cat tests_list |  sed -ne '/^ED/s/[^ ]*[ ][^ ]*[ ]\(.*\)$/\1 \\/p' | sed -e 's/\.\.\/common\/zb_cert_test_globals.c[[:space:]]*//' | sed -e 's/\.\.\/common\/zb_test_step_control.c[[:space:]]*//' | sed -e 's/\.\.\/common\/zb_test_step_storage.c[[:space:]]*//' | grep -e `echo ${BUILD_GROUPS[@]} | sed 's: : -e :g'`
    echo 'zb_nvram_erase.c \'

    echo
    echo
) > srcs_list

# Rebuild multitest
make rebuild

# Restore original files
rm -rf zb_multi_test_main.c srcs_list
mv zb_multi_test_main.tmp zb_multi_test_main.c
mv srcs_list.tmp srcs_list
