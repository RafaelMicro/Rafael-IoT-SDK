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
# PURPOSE: Generate files for single-fw test build scripts.

# Use  Options-gen-tests-list to generate files list

#Save user Options before coping Options-gen-tests-list
echo "Save Options"
cp -d ../../../Options ../../../Options_tmp
# Make a symbolic link to the Options-gen-tests-list
ln -fs `pwd`/../../../build-configurations/Options-gen-tests-list ../../../Options

MAC_PRIMITIVES_LIST="zb_mlme_associate_indication zb_mlme_associate_confirm zb_mlme_beacon_notify_indication zb_mlme_comm_status_indication zb_mlme_orphan_indication zb_mlme_reset_confirm zb_mlme_scan_confirm zb_mlme_start_confirm zb_mlme_poll_confirm zb_mlme_purge_confirm zb_mcps_data_indication zb_mcps_data_confirm zb_mlme_duty_cycle_mode_indication"

touch tests_list

cd ..

for i in tp_154_* ; do
[ -d $i ] && {
cd $i
make
cd ..
}
done > testbin/tests_list

cd testbin

(
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

echo 'SRCS_MAC_TESTS = \'
cat tests_list |  sed -ne '/^MAC/s/[^ ]*[ ][^ ]*[ ]\(.*\)$/\1 \\/p'

echo
echo

) > srcs_list

cat tests_list | while read role name src
do
    test_name=`echo $name | tr '[a-z]' '[A-Z]' | tr '\133\135-' '___'`
    change_to_static=0

    for mac_primitive_low_case in ${MAC_PRIMITIVES_LIST}
    do
        mac_primitive_up_case=`echo $mac_primitive_low_case | tr '[a-z]' '[A-Z]'`

        if grep "USE_${mac_primitive_up_case}" $src >/dev/null 2>&1; then

            (grep -v "USE_${mac_primitive_up_case}" $src | grep $mac_primitive_up_case >/dev/null 2>&1) || {
                sed -i -e "/^void ${mac_primitive_low_case}/s/^.*void.*${mac_primitive_low_case}/${mac_primitive_up_case}/" $src \
                    && sed -i -e "/^void ${mac_primitive_low_case}/s/^.*void.*${mac_primitive_low_case}/${mac_primitive_up_case}/" $src \
                    && change_to_static=1
            }
        fi
    done

    (grep "ZB_TEST_NAME" $src >/dev/null 2>&1) || {
        sed -i -e "/#define ZB_TRACE_FILE_ID/i\#define ZB_TEST_NAME ${test_name}" $src
    }

    if [ $change_to_static -ne 0 ]; then
        sed -i -e '/^[a-z0-9_]*_t/s/^/static /' -e '/^void/s/^/static /' $src
    fi
done

(
echo '\
/*  Copyright (c)  DSR Corporation, Denver CO, USA.
 PURPOSE:
 Auto-generated file! Do not edit!
*/
'

echo "#ifndef MAC_TESTS_TABLE_H"
echo "#define MAC_TESTS_TABLE_H"
echo

cat tests_list | grep '^MAC' | while read role name src
do
    test_name=`echo $name | tr '[a-z]' '[A-Z]' | tr '-' '_'`
    echo "void ${test_name}_main();"

    for mac_primitive_low_case in ${MAC_PRIMITIVES_LIST}
    do
        mac_primitive_up_case=`echo $mac_primitive_low_case | tr '[a-z]' '[A-Z]'`

        if grep "USE_${mac_primitive_up_case}" $src >/dev/null 2>&1; then
            echo "void ${test_name}_${mac_primitive_low_case}(zb_uint8_t param);"
        fi
    done
done

echo
echo 'static const zb_mac_test_table_t s_mac_tests_table[] ='
echo '{'
cat tests_list | grep '^MAC' | while read role name src
do
     test_name=`echo $name | tr '[a-z]' '[A-Z]' | tr '-' '_'`
     #echo "{ \"$test_name\", ${test_name}_main, ${test_name}_zb_zdo_startup_complete },"
     echo "  {"
     echo "    \"$test_name\","
     echo -n "    ${test_name}_main"

     for mac_primitive_low_case in ${MAC_PRIMITIVES_LIST}
     do
         mac_primitive_up_case=`echo $mac_primitive_low_case | tr '[a-z]' '[A-Z]'`

         echo ","
         if grep "USE_${mac_primitive_up_case}" $src >/dev/null 2>&1; then
             echo -n "    ${test_name}_${mac_primitive_low_case}"
         else
             echo -n "    NULL"
         fi
     done
     echo
     echo "  },"
done
echo '};'
echo
echo "#endif /* MAC_TESTS_TABLE_H */"

) > mac_tests_table.h


#Restore user Options
echo "Restore Options"
rm ../../../Options
cp -d ../../../Options_tmp ../../../Options
rm ../../../Options_tmp
