#!/bin/sh

# configuration
source ./servers.conf

table="ad_info"
key="5691"
column="ap:fadi:nm"

while true; do 
###########################################################################
echo
echo
echo "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"


if [ $# -gt 1 ]; then
  if [ $# -ne 4 ]; then
	  echo "usage: <shell_script> <table> <key> <column>"
		echo "       <shell_script>"
	fi
  table=$1
  key=$2
  column=$3
else
  echo -n "table name: "
  read table
  echo -n "row: "
  read key
  echo -n "column: "
  read column
fi

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
if [ ${#array[@]} -lt 1 ]; then
  echo "Info: $table $key $column"
  echo "Data does not exits"
  for host in ${ip_address[@]}
  do
  {
    rm $host -rf
    rm ${host}.value -rf
  }
  done

	continue
elif [ ${#array[@]} -eq 1 ]; then
  echo "Info: $table $key $column"
  echo "Data on server: "${servers[0]}
	cat ${array[0]}
  echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>ONLY one value item found."
else

	echo "Info: $table $key $column"
	echo "Data on server: "${servers[0]}
	echo "Data on server: "${servers[1]}
	`diff ${array[0]} ${array[1]}` > ".temp"
	echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Two value items are found."
	if [ ! -s ".temp" ]; then
		cat ${array[0]}
		echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>both values are the SAME."
	else
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
}
done
if [ $# -gt 1 ]; then
  exit 0;
fi
###########################################################################
done # end while

exit 0
