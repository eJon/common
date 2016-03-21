#!/bin/bash
#################################################
# @desc: 数据校验                       #
#################################################

WORK_DIR=`pwd`

{
source ../conf/data_import_check.conf
echo 'SUCCESS_BACKUP_DIR  :' "$SUCCESS_BACKUP_DIR"
echo 'WORK_DIR            :' "$WORK_DIR"
echo 'Conf file for mem_db:' "$DB_CONF"

# 检查数据服务配置文件存在与否
if [ ! -f $DB_CONF ]; then
  echo "[ERRO] db conf file not exist"
  rm -f $RUN_TAG
  $ALARM_SCRIPT "db_conf_file_not_exist"
  exit 1;
fi

 #依次处理数据文件
for file in `find $SUCCESS_BACKUP_DIR -name "*.gz"`; do
  echo "[INFO] Checking $file"
  dir_name=$(echo `dirname $file` |tr './' '_-')
  file_name=`basename $file`
	echo $file_name
	echo $file
  zcat $file |../bin/DataImportCheck $DB_CONF 
done

} 

exit 0
