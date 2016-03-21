// copyright:
//            (C) SINA Inc.
//
//      file: handler.h 
//      desc: API declarations to call RequestHandler
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date: 2012-10-10
//
//    change: 

#ifndef NGINX_CRMSERVICE_MOD_HANDLER_H_
#define NGINX_CRMSERVICE_MOD_HANDLER_H_

#if __cplusplus
extern "C" {
#endif

#include "ngx_core.h"
#include "ngx_http.h"
#include "ngx_http_request.h"
#include "ngx_adhandler_conf.h"
#define NGX_URL_REDIRECTION 1000
    
    void *ngx_create_handler(ngx_http_adhandler_loc_conf_t*srv_conf, ngx_conf_t *cf);
    
    int ngx_init_handler(void *request_handler, ngx_cycle_t *cyc);
    
    int ngx_handler_process(void *request_handler, ngx_http_request_t* r, 
          char **result_string);
    void ngx_destroy_handler(void *request_handler);

#if __cplusplus
}
#endif

enum {
  RET_OK = 0
};

#endif  // NGINX_CRMSERVICE_MOD_HANDLER_H_
