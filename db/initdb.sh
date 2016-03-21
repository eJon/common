#/usr/bin

#change this
DB_HOST="localhost"
DB_USER="root"
DB_PWD="pengdong"
DB_PORT="3306"
DB_NAME="testdb"

#create database
mysql -h ${DB_HOST} -u ${DB_USER} -P ${DB_PORT} -p ${DB_PWD} < create_table.sql
