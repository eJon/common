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

num=$(ps aux |grep run_common_parse.sh |grep -v "grep" | wc -l)

if [ $num -ge 1 ]; then
    exit 0
fi  

sh run_common_parse.sh
popd  > /dev/null # return the directory orignal

