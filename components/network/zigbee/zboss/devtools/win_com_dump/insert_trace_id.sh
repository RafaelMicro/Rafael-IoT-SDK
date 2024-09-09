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
# Insert unique ZB_TRACE_FILE_ID definition into all .c files
#

# Note: numbers should not be changed once created. If one has changed it,
# check and update numbers in zb_net_trace_is_net_blocked()
#

#FIND=gfind
FIND=/usr/bin/find

set -x

DIRS_LIST='application 
aps
bootloader 
common 
gpmac 
mac 
nwk 
samples 
osif 
secur 
tests 
zcl 
zdo 
zgp 
zll
zse
devtools/nsng'

DIRS_LIST0='.'

maxid=0
for i in `$FIND $DIRS_LIST -name \*.c` 
do
    id=`sed -ne '/ZB_TRACE_FILE_ID/s/[^0-9]*//p' $i 2>/dev/null`
    [ -n "$id" ] && {
        [ "$id" -gt $maxid ] && {
            maxid=$id
        }
    }
done

maxid=`expr $maxid + 1`

#exit

for i in `$FIND $DIRS_LIST -name \*.c` 
do
    grep ZB_TRACE_FILE_ID $i 2>&1 1>/dev/null || {
# use ex, not ed to get Windows line endians with cygwin
        ex $i <<EOF 
/#include/i
#define ZB_TRACE_FILE_ID $maxid
.

w!
EOF
        maxid=`expr $maxid + 1`
    }
done
