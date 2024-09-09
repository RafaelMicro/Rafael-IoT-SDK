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

TESTBIN_DIR=`pwd`
ZBOSS_DIR=${TESTBIN_DIR}/../../..

cd ${ZBOSS_DIR}

mv Options tmp_Options
ln -fs tests/multitest_common/Options-gen-tests-list Options

cd ${TESTBIN_DIR}/..

for i in R* ; do
    [ -d $i ] && {
        cd $i
        make
        cd ..
    }
done > testbin/tests_list

print_zboss_copyright_sh() {

    echo '#/* ZBOSS Zigbee software protocol stack
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
#
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
        test_group=`echo $test_name | sed -E "s/^(RTP\_)?([^\._-]+).*$/\2/"`

        if [ -z "$curr_group" ] || [ $test_group != $curr_group ]; then
            # We are starting a new ifdef group

            if [ -n "$curr_group" ]; then
                echo "#endif  /* ZB_REG_TEST_GROUP_${curr_group} */"
            fi

            echo "#if defined ZB_REG_TEST_GROUP_${test_group}"
        fi

        test_activation_conditions="   "
        test_deactivation_conditions="   "

        test_src_read_command="$(cat $src | grep 'ZB_TEST_TAG_')"

        while read define_decl tag_name tag_value
        do
        if [[ ! -z "${tag_name}" ]]; then
            test_activation_conditions+="defined(ZB_TEST_ENABLE_TAG_${tag_value}) && "
            test_deactivation_conditions+="!defined(ZB_TEST_DISABLE_TAG_${tag_value}) && "
        fi
        done <<< $test_src_read_command

        test_activation_conditions+="1"
        test_deactivation_conditions+="1"

        if [[ ! -z "${test_activation_conditions}" ]]; then
            echo "#if (defined(ZB_TEST_ENABLE_ALL_TAGS) && (${test_deactivation_conditions})) || (${test_activation_conditions})"
        fi

        curr_group="${test_group}"
        echo "{ \"$test_name\", ${test_name}_main, ${test_name}_zb_zdo_startup_complete },"

        if [[ ! -z "${test_activation_conditions}" ]]; then
            echo "#endif"
        fi
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
    cat tests_list |  sed -ne '/^ZC/s/[^ ]*[ ][^ ]*[ ]\(.*\)$/\1 \\/p' | sed -e 's/\.\.\/common\/zb_reg_test_globals.c[[:space:]]*//'

    echo
    echo

    echo 'SRCS_ZED = \'
    cat tests_list |  sed -ne '/^ED/s/[^ ]*[ ][^ ]*[ ]\(.*\)$/\1 \\/p' | sed -e 's/\.\.\/common\/zb_reg_test_globals.c[[:space:]]*//'

    echo
    echo
) > srcs_list

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

cd ${ZBOSS_DIR}

rm Options
mv tmp_Options Options

cd ${TESTBIN_DIR}
