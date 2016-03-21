/***************************************************************************
 *
 * Copyright (c) 2012 staff.sina.com.cn, Inc. All Rights Reserved
 * $007$
 *
 **************************************************************************/

/**
 * @file reserve_plugin/src/reserve_plugin_db_assist.cpp
 * @author Andrew(pengdong@staff.sina.com.cn)
 * @date 2012/11/19 13:10:29
 * @version 1.0
 * @mysql assistant for reserve_plugin
 *
 **/
#include <mysql/mysql.h>
#include <sys/timeb.h>
#include "inf_mysql.h"

namespace reserve_plugin {

MysqlAssist::MysqlAssist(void) {
}
MysqlAssist::~MysqlAssist(void) {
}
int MysqlAssist::Select(MysqlConfig &mysql_config,
                        std::string sql,
                        int expect_field_num /*IGNORE*/,
                        std::vector<std::vector<std::string> > &result,
                        std::string *error) {
    Log_r::Debug("select sql(%s)", sql.c_str());
    MYSQL *mysql_data =  mysql_init(NULL);

    if (NULL == mysql_data) {
        return 1;
    }

    if (NULL == mysql_real_connect(mysql_data, mysql_config.mysql_host.c_str(),
                                   mysql_config.mysql_user.c_str(), mysql_config.mysql_pwd.c_str(),
                                   mysql_config.mysql_db.c_str(), mysql_config.mysql_port, NULL, 0)) {
        if (error) {
            *error = mysql_error(mysql_data);
        }

        return RESERVE_PLUGIN_RET_WORK_ERR_LOAD_TASK_FROMDB;
    }

    result.clear();

    if (0 != mysql_real_query(mysql_data, sql.c_str(), strlen(sql.c_str()))) {
        if (error) {
            *error = mysql_error(mysql_data);
        }

        mysql_close(mysql_data);
        return 1;
    }

    MYSQL_RES *res = mysql_store_result(mysql_data);
    MYSQL_ROW row;
    int num_fields =  mysql_num_fields(res);

    if (expect_field_num != num_fields ) {
        char tmp[100] = {0};
        snprintf(tmp, 100, "expect field(%d) is not equal to query field(%d)",
                 expect_field_num,
                 num_fields);
        *error = tmp;
        mysql_free_result(res);
        mysql_close(mysql_data);
        return 1;
    }

    while ((row = mysql_fetch_row(res)) != NULL) { //foreach order
        std::vector<std::string> vrow;

        for (int i = 0; i < num_fields; ++i) {
            vrow.push_back(row[i]);
        }

        result.push_back(vrow);
    }

    if (result.size() == 0) {
        if (error) {
            *error = "query mysql return 0 row";
        }

        return 1;
    }

    if (error) {
        *error = "successful";
    }

    mysql_free_result(res);
    mysql_close(mysql_data);
    return 0;
}
//private method
int MysqlAssist::Affect(MysqlConfig &mysql_config, std::string sql, std::string *error) {
   
    MYSQL *mysql_data =  mysql_init(NULL);

    if (NULL == mysql_data) {
        return 1;
    }

    if (NULL == mysql_real_connect(mysql_data, mysql_config.mysql_host.c_str(),
                                   mysql_config.mysql_user.c_str(), mysql_config.mysql_pwd.c_str(),
                                   mysql_config.mysql_db.c_str(), mysql_config.mysql_port, NULL, 0)) {
        if (error) {
            *error = mysql_error(mysql_data);
        }

        return 1;
    }

    if (0 != mysql_real_query(mysql_data, sql.c_str(), strlen(sql.c_str()))) {
        if (error) {
            *error = mysql_error(mysql_data);
        }

        mysql_close(mysql_data);
        return 1;
    }

    if (error) {
        *error = "successful";
    }

    mysql_close(mysql_data);
    return 0;
}
}
