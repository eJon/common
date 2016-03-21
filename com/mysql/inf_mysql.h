/***************************************************************************
 *
 * Copyright (c) 2012 staff.sina.com.cn, Inc. All Rights Reserved
 * $007$
 *
 **************************************************************************/

/**
 * @file reserve_plugin/src/reserve_plugin_db_assist.h
 * @author Andrew(pengdong@staff.sina.com.cn)
 * @date 2012/11/19 13:10:29
 * @version 1.0
 * @mysql assistant for reserve_plugin
 *
 **/
#ifndef RESERVE_PLUGIN_DB_ASSIST_H
#define RESERVE_PLUGIN_DB_ASSIST_H

#include <string>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sys/timeb.h>
#include <iostream>
#include <tr1/unordered_map>
#include "reserve_plugin_common.h"
namespace reserve_plugin {
class MysqlAssist {
    public:
        MysqlAssist(void);
        ~MysqlAssist(void);
    public:
        static int Select(MysqlConfig &mysql_config,
                          std::string sql,
                          int expect_field_num /*IGNORE*/,
                          std::vector<std::vector<std::string> > &result,
                          std::string *error = NULL);
        static int Update(MysqlConfig &mysql_config, std::string sql, std::string *error = NULL) {
            return Affect(mysql_config, sql, error);
        }
        static int Insert(MysqlConfig &mysql_config, std::string sql, std::string *error = NULL) {
            return Affect(mysql_config, sql, error);
        }
        static int Delete(MysqlConfig &mysql_config, std::string sql, std::string *error = NULL) {
            return Affect(mysql_config, sql, error);
        }
    private:
        static int Affect(MysqlConfig &mysql_config, std::string sql, std::string *error = NULL);
};//class MysqlAssist
}//namespace reserve_plugin
#endif //RESERVE_PLUGIN_DB_ASSIST_H
