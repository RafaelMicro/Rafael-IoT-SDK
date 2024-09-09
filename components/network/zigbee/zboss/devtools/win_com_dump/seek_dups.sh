#!/bin/sh
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
# PURPOSE: Check for ZB_TRACE_FILE_ID duplicates

# Note: numbers should not be changed once created. If one has changed it,
# check and update numbers in zb_net_trace_is_net_blocked()
#

/usr/bin/find . -name \*.[ch] -exec grep 'define[ ]*ZB_TRACE_FILE_ID' {} \; | grep -v Binary | grep -v '\-1' | grep -v 'warning' | grep -v cortex | /usr/bin/sort -n -k 3 >ids
uniq ids > ids.u
ls -l ids*
diff -u ids ids.u | grep '\-' | grep -v '@' | grep -v ids |sed -r 's/^[-]//' | /usr/bin/sort | uniq | /usr/bin/sort -n -k 3 >dups

#note: now we have dups in osif for cortex. Seems it is ok

# search for holes in file id numbering
sed -e 's/[^0-9]*//g' ids | uniq | (c=1
while read a
do
    while [ $a -gt $c ]
    do
        echo $c
        c=`expr $c + 1`
    done
    c=`expr $c + 1`
done) > unused_ids

# create list of files to be updated
cat dups | while read a
do
    /usr/bin/find . -name \*.[ch] -exec grep -nH -e "$a"\$ {} \; | sed 1d
done > dup_files

#exit

sed -e 's/:.*$//' dup_files |
(
n=1
while read a
do
    id=`sed -n ${n}p unused_ids`
    n=`expr $n + 1`
    echo $a $id
    sed -e "/define[ ]*ZB_TRACE_FILE_ID/s/^.*$/#define ZB_TRACE_FILE_ID $id/" "$a" > tmp
    diff "$a" tmp >/dev/null 2>&1 || mv tmp "$a"
done
)
