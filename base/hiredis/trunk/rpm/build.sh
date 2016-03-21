#!/bin/bash

##for check
if [ $# -ne 3 ] && [ $# -ne 4 ];then
	echo "Usage: ./build.sh specfilename version buildno [prefix]"
	exit
fi
if [ $# -eq 4 ];then
	/usr/local/bin/rpm_create -p $4 -v $2 -r $3 $1 -k
else
	/usr/local/bin/rpm_create -v $2 -r $3 $1 -k
fi
