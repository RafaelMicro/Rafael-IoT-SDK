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

#Save user Options before coping Options-gen-tests-list
echo "Save Options"
cp -d ../../../../Options ../../../../Options_tmp
# Make a symbolic link to the Options-gen-tests-list
ln -fs `pwd`/../../../../build-configurations/Options-gen-tests-list ../../../../Options


cd ..

#for i in C* F* D* ; do
#for i in CN-CNF* CN-NFS* ; do
for i in FS-PRE* DN-DNF* DN-DNS*; do
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
    
echo 'SRCS_ZCZR = \'
cat tests_list |  sed -ne '/^ZC/s/[^ ]*[ ][^ ]*[ ]\(.*\)$/\1 \\/p'

echo
echo

echo 'SRCS_ZED = \'
cat tests_list |  sed -ne '/^ED/s/[^ ]*[ ][^ ]*[ ]\(.*\)$/\1 \\/p'

echo
echo
) > srcs_list

cat tests_list | while read role name src
do
    test_name=`echo $name | tr '[a-z]' '[A-Z]' | tr '\133\135-' '___'`
    grep ZB_ZDO_STARTUP_COMPLETE $src >/dev/null 2>&1 || {
    sed -i -e '/^void zb_zdo_startup_complete/s/^.*void.*zb_zdo_startup_complete/ZB_ZDO_STARTUP_COMPLETE/' -e '/^[a-z0-9_]*_t/s/^/static /' -e '/^void/s/^/static /' \
        -e "/#define ZB_TRACE_FILE_ID/i\\
#define ZB_TEST_NAME ${test_name}" $src
}
done

(
echo '\
/*  Copyright (c)  DSR Corporation, Denver CO, USA.
 PURPOSE:
 Auto-generated file! Do not edit!
*/
'

echo "#ifndef ZC_TESTS_TABLE_H"
echo "#define ZC_TESTS_TABLE_H"
echo

cat tests_list | grep '^ZC' | while read role name src
do
    test_name=`echo $name | tr '[a-z]' '[A-Z]' | tr '-' '_'`
    echo "void ${test_name}_main();"
    echo "void ${test_name}_zb_zdo_startup_complete(zb_uint8_t param);"
done

echo
echo 'static const zb_test_table_t s_tests_table[] = {'
cat tests_list | grep '^ZC' | while read role name src
do
    test_name=`echo $name | tr '[a-z]' '[A-Z]' | tr '-' '_'`
    echo "{ \"$test_name\", ${test_name}_main, ${test_name}_zb_zdo_startup_complete },"
done
echo '};'
echo
echo "#endif /* ZC_TESTS_TABLE_H */"

) > zc_tests_table.h

(
echo '\
/*  Copyright (c)  DSR Corporation, Denver CO, USA.
 PURPOSE:
 Auto-generated file! Do not edit!
*/
'

echo "#ifndef ZED_TESTS_TABLE_H"
echo "#define ZED_TESTS_TABLE_H"
echo

cat tests_list | grep '^ED' | while read role name src
do
    test_name=`echo $name | tr '[a-z]' '[A-Z]' | tr '\133\135-' '___'`
    echo "void ${test_name}_main();"
    echo "void ${test_name}_zb_zdo_startup_complete(zb_uint8_t param);"
done

echo
echo 'static const zb_test_table_t s_tests_table[] = {'
cat tests_list | grep '^ED' | while read role name src
do
    test_name=`echo $name | tr '[a-z]' '[A-Z]' | tr '\133\135-' '___'`
    echo "{ \"$test_name\", ${test_name}_main, ${test_name}_zb_zdo_startup_complete },"
done
echo '};'
echo
echo "#endif /* ZED_TESTS_TABLE_H */"

) > zed_tests_table.h

#Restore user Options
echo "Restore Options"
rm ../../../../Options
cp -d ../../../../Options_tmp ../../../../Options
rm ../../../../Options_tmp
