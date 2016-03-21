// copyright:
//            (C) SINA Inc.
//
//      file: ngx_http_adhandler_module.cc 
//      desc: nginx context for nginx moudle 
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date: 2012-10-10
//
//    change: 

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <ctype.h>
#include "ngx_adhandler_conf.h"
#include "ngx_handler_interface.h"

static void* s_request_handler;

static ngx_int_t init_process(ngx_cycle_t *cycle);

static ngx_int_t init_master(ngx_log_t *log);

static void exit_process(ngx_cycle_t *cycle);

static ngx_int_t ngx_http_response_result(ngx_http_request_t *r, char* result);

static char *ngx_http_adhandler(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static void *ngx_http_create_adhandler_loc_conf(ngx_conf_t *cf);

static char *ngx_http_merge_adhandler_loc_conf(ngx_conf_t *cf, void *prev, void *conf);

static void ngx_http_adhandler_post_handler(ngx_http_request_t *r);

static ngx_int_t ngx_http_get_handler(ngx_http_request_t *r);

// Set Redirction.
static ngx_int_t ngx_http_set_redirection(ngx_http_request_t* r, char* url);

// allocate memory for ngx_http_adhandler_loc_conf_t

static void* ngx_http_create_adhandler_loc_conf(ngx_conf_t *cf) {
	ngx_http_adhandler_loc_conf_t *srcf = ngx_palloc(cf->pool, sizeof(ngx_http_adhandler_loc_conf_t));
	if (NULL == srcf) {
		ngx_log_error(NGX_LOG_ERR, cf->log, 0, "palloc error!!!");
		return NGX_CONF_ERROR;
	}
	srcf->plugin_manager_config_file.len = NGX_CONF_UNSET_UINT;

	return srcf;
}

static char *ngx_http_merge_adhandler_loc_conf(ngx_conf_t *cf, void *prev, void *conf)
{
	ngx_http_adhandler_loc_conf_t *ch = (ngx_http_adhandler_loc_conf_t*)conf;

	if(NULL == ch) {
		ngx_log_error(NGX_LOG_DEBUG, cf->log, 0,"merge local conf, local conf pointer NULL");
		return NGX_CONF_OK;
	}

	if(ch->plugin_manager_config_file.len == NGX_CONF_UNSET_UINT) {
		return NGX_CONF_OK;
	}

	if(NULL == s_request_handler) {
		s_request_handler = ngx_create_handler(ch, cf);
		if(NULL == s_request_handler) {
			ngx_log_error(NGX_LOG_ERR, cf->log, 0,"create request handler error");
			return NGX_CONF_OK;
		}
	}
	return NGX_CONF_OK;
}

static ngx_command_t ngx_http_adhandler_commands[] = {
    { ngx_string(ADHANDLER),
      NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
      ngx_http_adhandler,
      0,
      0,
      NULL },

    { ngx_string(PLUGIN_MANAGER_CONF_FILE),
      NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
	    NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_adhandler_loc_conf_t,plugin_manager_config_file),
      NULL },
    ngx_null_command
};

static ngx_http_module_t ngx_http_adhandler_module_ctx = {
    NULL,                          /* preconfiguration */
    NULL,                          /* postconfiguration */

    NULL,                          /* create main configuration */
    NULL,                          /* init main configuration */

    NULL,                          /* create server configuration */
    NULL,                          /* merge server configuration */

    ngx_http_create_adhandler_loc_conf, /* create location configuration */
    ngx_http_merge_adhandler_loc_conf   /* merge location configuration */
};

ngx_module_t ngx_http_adhandler_module = {
    NGX_MODULE_V1,
    &ngx_http_adhandler_module_ctx,    /* module context */
    ngx_http_adhandler_commands,       /* module directives */
    NGX_HTTP_MODULE,                    /* module type */
    init_master,                        /* init master */
    NULL,                               /* init module */
    init_process,                       /* init process */
    NULL,                               /* init thread */
    NULL,                               /* exit thread */
    exit_process,                       /* exit process */
    NULL,                               /* exit master */
    NGX_MODULE_V1_PADDING
};

void exit_process(ngx_cycle_t *cycle) {
  return;
}

static ngx_int_t init_master(ngx_log_t *log) {
	return 0;
}

static ngx_int_t init_process(ngx_cycle_t *cycle) {

  /*
  ngx_http_adhandler_loc_conf_t* loc_conf = (ngx_http_adhandler_loc_conf_t*)
    ngx_get_conf(cycle->conf_ctx, ngx_http_adhandler_module);

	
  if(NULL == loc_conf) 
  {
		ngx_log_error(NGX_LOG_DEBUG, cycle->log, 0,"get location config is null");
    return NGX_ERROR;
  }
  else {
		ngx_log_error(NGX_LOG_DEBUG, cycle->log, 0,"init read handler  spio:%s", 
        loc_conf->plugin_manager_config_file.data);
  }*/
	if(NULL == s_request_handler) return NGX_ERROR;
	int ret = ngx_init_handler(s_request_handler, cycle);
	if(0 != ret) {
		ngx_log_error(NGX_LOG_ERR, cycle->log, 0,"init Request handle failed:%d", ret);
		return NGX_OK;
	}
	ngx_log_error(NGX_LOG_DEBUG, cycle->log, 0,"init process ok!!!");

	return NGX_OK;
}

static ngx_int_t ngx_http_adhandler_handler(ngx_http_request_t *r) {
  //
 //ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0, "request url =%s", r->args.data);
 //ngx_str_t value;
 struct timeval tpstart, tpend;
 long timeuse;
 gettimeofday(&tpstart,NULL);
 ngx_int_t ret;
 if ((r->method & (NGX_HTTP_GET|NGX_HTTP_HEAD))) {
   ret = ngx_http_get_handler(r);
   if (NGX_OK != ret) {
     ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, 
         "ngx_http_get handler is error, ret=%d", ret);
     return ret;
   }

   return NGX_OK;
 }
 
 /* if (ngx_http_arg(r, (u_char *) "ip", 2, &value) == NGX_OK) {
   ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0, "value =%s", value.data);
 }*/
 if (!(r->method & (NGX_HTTP_POST))) {
   ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,"NOT HTTP POST");
   return NGX_HTTP_NOT_ALLOWED;
 }
 
 ngx_int_t rc = ngx_http_read_client_request_body(r, ngx_http_adhandler_post_handler);
 if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
   ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,"rc = %d",  rc);
   return rc;
 }
 gettimeofday(&tpend,NULL);
 timeuse = 1000000*(tpend.tv_sec-tpstart.tv_sec)+(tpend.tv_usec-tpstart.tv_usec);
 ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "this request cost time = %ld", timeuse);
 return NGX_OK;
}
static ngx_int_t ngx_http_get_handler(ngx_http_request_t *r)
{
  char *result = NULL;
  ngx_int_t rc = ngx_handler_process(s_request_handler, r, &result);

  // Redirct url to destination url; NGX_URL_REDIRECTION is defined 
  // redirction flag type;
  if (NGX_URL_REDIRECTION == rc) {
    ngx_log_error(NGX_LOG_DEBUG, r->connection->log,
        0, "Nginx help redirction");
    rc = ngx_http_set_redirection(r, result);
  }
  else {

    if (NGX_OK != rc) {
      ngx_log_error(NGX_LOG_ERR, r->connection->log, 
          0,"ngx_handler_process , rc=%d", rc);
    } 
    rc = ngx_http_response_result(r, result);
    if (NGX_OK != rc) {
      ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
          "ngx_http_response_result, rc=%d", rc);
    } 
  }

  if (NGX_OK != rc) {
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 
        0,"ngx_response error, rc=%d", rc);
  } 
  return NGX_OK;
}

static ngx_int_t ngx_http_set_redirection(ngx_http_request_t* r, char* url) {
  if (NULL == url) {
    ngx_log_error(NGX_LOG_ERR, r->connection->log,
        0, "url is null,no need redirect");
    return NGX_ERROR;
  }
  ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0, "url=%s", url);
  r->headers_out.status = NGX_HTTP_MOVED_TEMPORARILY;
  r->header_only = 1; 
  r->headers_out.content_length_n = -1;
  r->headers_out.location = ngx_list_push(&r->headers_out.headers);

  if (r->headers_out.location == NULL) {
    ngx_log_error(NGX_LOG_ERR, r->connection->log,
        0, "location is null");
    return NGX_HTTP_INTERNAL_SERVER_ERROR;
  }
  r->headers_out.location->hash = 1;
  ngx_str_set(&r->headers_out.location->key, "Location"); 
  ngx_str_t val;
  val.len = strlen(url) + 1;
  val.data = ngx_pnalloc(r->pool, val.len);
  if (NULL == val.data) {
    return NGX_ERROR;
  }
  ngx_memcpy(val.data, url, val.len);
  r->headers_out.location->value = val;
  ngx_int_t rc = ngx_http_send_header(r);
  return rc;
}

static ngx_int_t ngx_http_response_result(ngx_http_request_t *r,
                                          char* result) {
  if (NULL == result) {
    return NGX_ERROR;
  }
  ngx_chain_t   out;
  /* attach this buffer to the buffer chain */
  ngx_buf_t    *b = NULL;
  ngx_uint_t len = strlen(result);
  b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
  /* adjust the pointers of the buffer */
  u_char *res = (u_char*)result;
  b->pos = res;
  b->last = res + len;
  b->memory = 1;    /* this buffer is in memory */
  b->last_buf = 1;  /* this is the last buffer in the buffer chain */
  /* set the status line */
  out.buf = b;
  out.next = NULL;
  r->headers_out.status = NGX_HTTP_OK;
  r->headers_out.content_length_n = len;
  ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0,"result=%s, size=%d\n", result, len);
  /* send the headers of your response */
  ngx_int_t rc = ngx_http_send_header(r);
  if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,"send the headers failed");
    return rc;
  }
  /* send the buffer chain of your response */
  rc = ngx_http_output_filter(r, &out);
  if(rc != NGX_OK)
  {
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,"ngx_http_output_filter failed");
    return rc;
  }
  return NGX_OK;
}

static void ngx_http_adhandler_post_handler(ngx_http_request_t *r)
{
  ngx_int_t rc;
  if (!(r->method & (NGX_HTTP_POST))) {
	  ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,"only HTTP POST request accepted");
	  return;
  }

  char *result = NULL;
  
  rc = ngx_handler_process(s_request_handler, r, &result);
	if(rc < 0) {
	  ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,"handleOneQ failed");
	}
  /* allocate a buffer for your response body */
  
	//ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0,"result=%s,len=%d\n", result,strlen(result));
  if (NGX_URL_REDIRECTION == rc) {
    rc = ngx_http_set_redirection(r, result);
  }
  else {
    rc = ngx_http_response_result(r, result);
  }
  ngx_http_finalize_request(r, rc);
}

static char *ngx_http_adhandler(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
  ngx_http_core_loc_conf_t *clcf;
  clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
  clcf->handler = ngx_http_adhandler_handler;
  
  return NGX_CONF_OK;
}

