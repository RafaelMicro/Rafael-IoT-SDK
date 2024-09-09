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

# Use Options-gen-tests-list to generate files list

cd ..

for i in T* ; do
    [ -d $i ] && {
        cd $i
        make
        cd ..
    }
done > testbin/tests_list


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

print_zboss_copyright_c() {

    echo '/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * This is unpublished proprietary source code of DSR Corporation
 * The copyright notice does not evidence any actual or intended
 * publication of such source code.
 *
 * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
 * Corporation
 *
 * Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and
 * DSR.
 */
/* PURPOSE:
 Auto-generated file! Do not edit!
 */
'
    
}

print_test_table() {
    
    echo 'static const zb_test_table_t s_tests_table[] = {'


    cat tests_list | grep $1 | while read role name src
    do
        test_name=`echo $name | tr '[a-z]' '[A-Z]' | tr '-' '_'`
        test_group=`echo $test_name | sed -E "s/^(TP\_)?([^\._-]+).*$/\2/"`

        if [ -z "$curr_group" ] || [ $test_group != $curr_group ]; then
            # We are starting a new ifdef group
            
            if [ -n "$curr_group" ]; then
                echo "#endif  /* ZB_TEST_GROUP_ZCP_R22_${curr_group} */"
            fi

            echo "#if defined ZB_TEST_GROUP_ZCP_R22_${test_group}"
        fi

        curr_group="${test_group}"
        echo "{ \"$test_name\", ${test_name}_main, ${test_name}_zb_zdo_startup_complete },"
    done

    echo "#endif "


    echo "{ \"NVRAM_ERASE\", NVRAM_ERASE_main, NULL},"

    echo '};'
}

########################################

cd testbin

(
    print_zboss_copyright_sh
    
    echo 'SRCS_ZCZR = \'
    cat tests_list |  sed -ne '/^ZC/s/[^ ]*[ ][^ ]*[ ]\(.*\)$/\1 \\/p' | sed -e 's/\.\.\/common\/zb_cert_test_globals.c[[:space:]]*//'
    echo 'zb_nvram_erase.c \'
    echo '../common/zb_cert_test_globals.c \'

    echo
    echo

    echo 'SRCS_ZED = \'
    cat tests_list |  sed -ne '/^ED/s/[^ ]*[ ][^ ]*[ ]\(.*\)$/\1 \\/p' | sed -e 's/\.\.\/common\/zb_cert_test_globals.c[[:space:]]*//'
    echo 'zb_nvram_erase.c \'
    echo '../common/zb_cert_test_globals.c \'

    echo
    echo
) > srcs_list

########################################

cat tests_list | while read role name src
do
    echo $src
    test_name=`echo $name | tr '[a-z]' '[A-Z]' | tr '\133\135-' '___'`
    grep ZB_ZDO_STARTUP_COMPLETE $src >/dev/null 2>&1 || {
    sed -i -e '/^void zb_zdo_startup_complete/s/^.*void.*zb_zdo_startup_complete/ZB_ZDO_STARTUP_COMPLETE/' -e '/^[a-z0-9_]*_t/s/^/static /' -e '/^void/s/^/static /' \
        -e "/#define ZB_TRACE_FILE_ID/i\\
#define ZB_TEST_NAME ${test_name}" $src
}
done


########################################

(
    print_zboss_copyright_c

    echo "#ifndef ZC_TESTS_TABLE_H"
    echo "#define ZC_TESTS_TABLE_H"
    echo

    cat tests_list | grep '^ZC' | sort -f | while read role name src
    do
        test_name=`echo $name | tr '[a-z]' '[A-Z]' | tr '-' '_'`
        echo "void ${test_name}_main();"
        echo "void ${test_name}_zb_zdo_startup_complete(zb_uint8_t param);"
    done

    echo "void NVRAM_ERASE_main();"

    echo

    print_test_table '^ZC'

    echo
    echo "#endif /* ZC_TESTS_TABLE_H */"

) > zc_tests_table.h

#########################################

(
    print_zboss_copyright_c

    echo "#ifndef ZED_TESTS_TABLE_H"
    echo "#define ZED_TESTS_TABLE_H"
    echo

    cat tests_list | grep '^ED' | while read role name src
    do
        test_name=`echo $name | tr '[a-z]' '[A-Z]' | tr '\133\135-' '___'`
        echo "void ${test_name}_main();"
        echo "void ${test_name}_zb_zdo_startup_complete(zb_uint8_t param);"
    done

    echo "void NVRAM_ERASE_main();"

    echo

    print_test_table '^ED'

    echo
    echo "#endif /* ZED_TESTS_TABLE_H */"

) > zed_tests_table.h
