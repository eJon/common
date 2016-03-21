// copyright:
//            (C) SINA Inc.
//
//      file: crmservice.h 
//      desc: definition for structure of ngx_http_crmservice_loc_conf_t 
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date: 2012-10-10
//
//    change: 

#ifndef MOD_ADFRONT_NGX_HANDLER_CONF_H_
#define MOD_ADFRONT_NGX_HANDLER_CONF_H_

#define ADFRONT_VERSION 1.0
#define ADFRONT_VER "adfront/" ADFRONT_VERSION
#define PLUGIN_MANAGER_CONF_FILE "plugin_manager_config_file"
#define ADHANDLER "adhandler"
//#define PLUGIN_NAME "__plugin_name__"


#ifndef UNITTEST_
#include <ngx_config.h>
#include <ngx_core.h>

struct ngx_adhandler_loc_conf_s {
    ngx_str_t plugin_manager_config_file;
};
typedef struct ngx_adhandler_loc_conf_s ngx_http_adhandler_loc_conf_t;

#endif //end UNITTEST_

#endif  // MOD_ADFRONT_NGX_HANDLER_CONF_H_
