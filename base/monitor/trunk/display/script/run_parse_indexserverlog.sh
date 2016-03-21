#!/bin/sh
#########################
#@author hongbin2@staff.sina.com.cn
#@date 
#@desc TODO
#########################
export SCRIPT_PATH=`dirname $(readlink -f $0)` # get the path of the script
export PATH=/sbin:/bin:/usr/sbin:/usr/bin:$PATH
pushd . > /dev/null 
cd "$SCRIPT_PATH"

while [ 1 ] ; do
    python2.6  ${SCRIPT_PATH}/parse_indexserverlog.py 
    sleep 200
done

popd  > /dev/null # return the directory orignal

