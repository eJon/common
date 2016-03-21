#!/bin/sh
#########################
#@author hongbin2@staff.sina.com.cn
#@date
#@desc TODO
#########################
export PATH=/home/w/python27/bin:/usr/bin:$PATH
export SCRIPT_PATH=`dirname $(readlink -f $0)` # get the path of the script
pushd . > /dev/null
cd "$SCRIPT_PATH"

while [ 1 ]; do
python ../script/parse_indexserver.py
sleep 180
done


popd  > /dev/null # return the directory orignal


