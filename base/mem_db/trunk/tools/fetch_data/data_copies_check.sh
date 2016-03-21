#!/bin/sh

# configuration
source ./servers.conf

table=""
key=""
column=""
file=""

if [ $# -ne 1 ]; then
  echo "usage: <shell_script> <file> "
	exit 0;
fi
file=$1
if [ ! -f $file ]; then
  echo "file: $file does not exist!!"
	exit 0;
fi



two_copies_and_the_same=0
two_copies_but_not_different=0
one_copy=0
no_exist=0

# for each line in the file
zcat $file |cut -f 1-3 | while read line
do
		for host in ${ip_address[@]}
		do
		{
			rm $host -rf
			rm ${host}.value -rf
		}
		done
  echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
	echo "processing: $line"
  table=`echo $line|cut -f1 -d" "`
  key=`echo $line|cut -f2 -d" "`
  column=`echo $line|cut -f3 -d" "`

	declare -a array
	declare -a servers
	index=0

	for host in ${ip_address[@]}
	do
	{
		echo "Fetching data from server: $host"

		{	
		 echo "get $table&$key&$column"  
		 usleep 50000
		} | telnet $host $port > $host 2>/etc/null

		sed -n '/VALUE/,/END/p' $host > ${host}.value
		if [ -s ${host}.value ]; then
			array[$index]=${host}.value 
			servers[$index]=${host}
			index=`expr $index + 1`
		fi
	}
	done

	echo
	if [ ${index} -lt 1 ];then
		no_exist=`expr $no_exist + 1`
		echo "Info: $table $key $column"
		echo "         Data does not exits"

		continue
	elif [ ${index} -eq 1 ];then
		one_copy=`expr $one_copy + 1`
		echo "Info: $table $key $column"
		echo "           Data on server: "${servers[0]}
		cat ${array[0]}
	else
		echo "Info: $table $key $column"
		echo "Data on server: " ${servers[0]}
		echo "Data on server: " ${servers[1]}
		rm -rf ".temp"
		touch ".temp"
		diff ${array[0]} ${array[1]} > ".temp"
		echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Two value items are found."
		if [ ! -s ".temp" ]; then
		  two_copies_and_the_same=`expr $two_copies_and_the_same + 1`
			cat ${array[0]}
			cat ${array[1]}
			echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>both values are the SAME."
		else
		  two_copies_but_not_different=`expr $two_copies_but_not_different + 1`
			echo ">>>>>>>>>>>>>>>>"
			cat ${array[0]}
			echo ">>>>>>>>>>>>>>>>"
			cat ${array[1]}
			echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>both values are NOT SAME."
		fi
	fi

	for host in ${ip_address[@]}
	do
	{
		rm $host -rf
		rm ${host}.value -rf
		rm -rf ".temp"
	}
	done
	if [ $# -gt 1 ]; then
		exit 0;
	fi
	###########################################################################
  echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Not existing items : $no_exist"
  echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Only one copy items: $one_copy"
  echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>two copy items and the same    : $two_copies_and_the_same"
  echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>two copy items but not the same: $two_copies_but_not_different"
done

echo "Not existing items : $no_exist"
echo "Only one copy items: $one_copy"
echo "two copy items and the same    : $two_copies_and_the_same"
echo "two copy items but not the same: $two_copies_but_not_different"



exit 0
