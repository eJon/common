// copyright:
//            (C) SINA Inc.
//
//      file: ngx_handler_interface.h 
//      desc: implementation for API declarations to call Handler
//    author: kefeng
//     email: xdjackfeng@gmail.com
//      date: 2013-04-25
//
//    change: 
#include <sharelib/pluginmanager/http_request_define.h>
#include "ngx_handler_interface.h"
#include "ngx_handler.h"
#include <stdint.h>
#include <string>

#include "util.h" 

using namespace std;

#define PLUGIN_MANAGER_HOME_PATH "__plugin_manager_home_path__"

using namespace ngx_handler;

// This function is used when nginx merge location. Merge location
// is used by master process. Save these parameter conf for worker
// process, all worker process have copy of this object.
// param: [input] srv_conf: module location conf
//        [input] ngx_conf_t: nginx conf
// return: return handler object.

void *ngx_create_handler(ngx_http_adhandler_loc_conf_t* srv_conf, ngx_conf_t *cf) {
  STR_MAP config_map;
  if (NULL  == srv_conf || NULL == cf) {
    return NULL;
  }
  Handler *request_handler = new Handler();
  if (NULL == request_handler) {
    return NULL;
  }
  string plugin_conf_file = string((char*)srv_conf->plugin_manager_config_file.data,
      srv_conf->plugin_manager_config_file.len);
  
  config_map[PLUGIN_MANAGER_HOME_PATH] = plugin_conf_file;
  
  request_handler->Init(config_map);
  return request_handler;
}

// This function has been used by nginx for destroy handler.
// Free memory space and process end thing.
void ngx_destroy_handler(void *request_handler) {
    if (NULL != request_handler) {
        ((Handler *)request_handler)->Destroy();
    }
    delete ((Handler *)request_handler);
}

// Init some request handler, all workers use this function for
// request handler init.
int ngx_init_handler(void *request_handler, ngx_cycle_t *cyc) {
  if (NULL == request_handler) {
    return -1;
  }
  return ((Handler *)request_handler)->InitProcess();
}
// Get cookie string from nginx request struct.
static string ngx_http_get_cookie(ngx_http_request_t *r)
{
  ngx_table_elt_t ** cookies = NULL;
  ngx_log_error(NGX_LOG_DEBUG,  r->connection->log, 0, 
      "Cookie count: %d\n", r->headers_in.cookies.nelts);
  cookies = (ngx_table_elt_t**)r->headers_in.cookies.elts;
  ngx_uint_t i = 0;
  for (i = 0; i < r->headers_in.cookies.nelts; i++) { 
    ngx_log_error(NGX_LOG_DEBUG,  r->connection->log, 0,
        "Cookie line %d: %s\n", i, cookies[i]->value.data);
  }
  if (r->headers_in.cookies.nelts > 0) {
    return string((char*)cookies[0]->value.data, cookies[0]->value.len);
  }
  return "";

}

static string ngx_http_get_forward(ngx_http_request_t* r)
{
  if (NULL == r->headers_in.x_forwarded_for){
    return "";
  }
  if (r->headers_in.x_forwarded_for->value.len > 0) {
    return string((char*)r->headers_in.x_forwarded_for->value.data ,
        r->headers_in.x_forwarded_for->value.len);
  }
  return "";
}

static string ngx_http_get_client_ip(ngx_http_request_t* r) {
//ngx_http_get_indexed_variable("");
  string ipaddr = "";
  if (NULL != r->connection) {
    ipaddr= string((char*)r->connection->addr_text.data,
                   r->connection->addr_text.len);
  }
  return ipaddr;

}
static string ngx_http_get_referer(ngx_http_request_t* r)
{
  if (NULL == r->headers_in.referer){
    return "";
  }
  if (r->headers_in.referer->value.len > 0) {
    return string((char*)r->headers_in.referer->value.data ,
        r->headers_in.referer->value.len);
  }
  return "";
}


static string ngx_http_get_user_agent(ngx_http_request_t* r)
{
  if (NULL == r->headers_in.user_agent) {
    return "";
  }
  if (r->headers_in.user_agent->value.len > 0) {
    return string((char*)r->headers_in.user_agent->value.data ,
        r->headers_in.user_agent->value.len);
  }
  return "";
}
// This function process nginx request struct for post type.
static int ngx_do_get_post_body(ngx_http_request_t *r, STR_MAP& query_map)
{
  if (!(r->method & (NGX_HTTP_POST))) {
    ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0,"HTTP Get request accepted");
    return RET_OK;
  }
  size_t len = 0; //post data len   
  u_char *buf = NULL, *p = NULL, *last = NULL; //post data pointer   
  ngx_chain_t *cl;
  if (r->request_body != NULL && r->request_body->bufs != NULL) {
    for (cl = r->request_body->bufs; cl; cl = cl->next) {
      if(ngx_buf_in_memory(cl->buf)){
        len += cl->buf->last - cl->buf->pos;
      }else{
        len += cl->buf->file_last - cl->buf->file_pos;
        ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0, 
            "cl->buf->file_last - cl->buf->file_pos: %d", cl->buf->file_last - cl->buf->file_pos);
      }	        
    }
    ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0, "len: %d", len);
    if (len > 0) {
      buf = (u_char*)ngx_pcalloc(r->pool, len + 1);
      if (buf == NULL) {
        ngx_http_finalize_request(r, NGX_ERROR);
        return RET_OK;
      }
      p = buf;
      last = p + len; 

      for (cl = r->request_body->bufs; cl; cl = cl->next) {
        if(ngx_buf_in_memory(cl->buf)){
          p = ngx_copy(p, cl->buf->pos, cl->buf->last - cl->buf->pos);
        }else{
          if(NULL != cl->buf->file){
            if(0 > ngx_read_file(cl->buf->file, p, cl->buf->file_last - cl->buf->file_pos, cl->buf->file_pos)){
              ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ngx_read_file failed");
              return RET_OK;
            }						
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "len: %lu", len);
          }					
        }				
      }
    }
  }	else {
    return RET_OK;
  }
  string tmp_str;
  if (0 != UrlDecode(string((char*)buf), tmp_str)) {
    tmp_str = "";
  }
  query_map[HTTP_REQUEST_POST_BODY] = tmp_str; 
  return RET_OK;
}

static void ngx_query_map_print(ngx_http_request_t* r, const STR_MAP &query_map)
{
  STR_MAP::const_iterator iter = query_map.begin();
  while (iter != query_map.end()) {
    ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0, "key=%s,value=%s", 
        iter->first.c_str(), iter->second.c_str());
    iter++;
  }
}

// Parse http url string and save q_map, split by =
int ngx_url_parser(const char *in_buffer, STR_MAP& q_map, const char *const delims)
{
    char *key = NULL;
    size_t key_len =0;
    char *value = NULL;
    size_t value_len = 0;
    char *p = NULL;
    if(NULL != in_buffer){
        char *buffer = (char *)in_buffer;
        //char *buffer = (char*)malloc(strlen(in_buffer) + 1);
        //memcpy(buffer, in_buffer, strlen(in_buffer) + 1);
        for (p = strchr(buffer, '='); p; p = strchr(p, '=')) {
            if (p == buffer) {
                ++p;
                continue;
            }
            for (key = p-1; isspace(*key); --key);
            key_len = 0;
            while (isalnum(*key) || '_' == *key || '\\' == *key || '/' == *key || ':' == *key) {
                /* don't parse backwards off the start of the string */
                if (key == buffer) {
                    --key;
                    ++key_len;
                    break;
                }
                --key;
                ++key_len;
            }
            ++key;
            *(buffer + (key - buffer) + key_len) = '\0';
            for (value = p+1; isspace(*value); ++value);
            value_len = strcspn(value, delims);
            p = value + value_len;
            if ('\0' != *p){
                *(value + value_len) = '\0';
                p = value + value_len + 1;    
            }else{
                p = value + value_len;
            }
            q_map.insert(make_pair(key, value));
        }
    }
    return RET_OK;
}

// Process nginx request string  and save these value into map.
static int ngx_header_handler(ngx_http_request_t* r, STR_MAP &query_map)
{
  string raw_str = string((char*)r->args.data, r->args.len);
 
  string tmp_str;
  if (0 != UrlDecode(raw_str, tmp_str)) {
    return -1;
  }
  
  query_map[HTTP_REQUEST_URL] = tmp_str;
  if (RET_OK != ngx_url_parser(tmp_str.c_str(), query_map,"&")) {
    return -1;
  }
  tmp_str = string((char*)r->unparsed_uri.data, r->unparsed_uri.len);
  ngx_log_error(NGX_LOG_DEBUG,  r->connection->log, 0,
      "urllen=%lu,url=%s", r->unparsed_uri.len, tmp_str.c_str());
  size_t pos1 = tmp_str.find_last_of("/"), pos2 = tmp_str.find_first_of("?");
  ngx_log_error(NGX_LOG_DEBUG,  r->connection->log, 0, 
      "pos1=%lu, pos2=%lu", pos1,pos2);
  if (pos1 != string::npos && pos2 != string::npos && pos2 >pos1) {
    tmp_str = tmp_str.substr(pos1+1, pos2-pos1-1);
  }
  else {
    if (pos1 != string::npos) {
      tmp_str = tmp_str.substr(pos1+1);
    }
    else if(pos2 != string::npos){
      tmp_str = tmp_str.substr(0, pos2);
    }
  }
  query_map[HTTP_REQUEST_PLUGINNAME] = tmp_str;
  query_map[HTTP_REQUEST_COOKIE] = ngx_http_get_cookie(r);
  if (r->method & (NGX_HTTP_POST)) {
    query_map[HTTP_REQUEST_METHOD] = HTTP_REQUEST_POST_METHOD; 
  }
  else if(r->method & (NGX_HTTP_GET)){
    query_map[HTTP_REQUEST_METHOD] = HTTP_REQUEST_GET_METHOD;
  }
  query_map[HTTP_REQUEST_HEADER_FORWARD]= ngx_http_get_forward(r);

  query_map["__client_ip__"] = ngx_http_get_client_ip(r);

  query_map[HTTP_REQUEST_HEADER_USER_AGENT]= ngx_http_get_user_agent(r);

  query_map[HTTP_REQUEST_HEADER_REFERER] = ngx_http_get_referer(r);

  return RET_OK;
}

int ngx_handler_process(void *request_handler,ngx_http_request_t* r, char **result_str)
{
  STR_MAP query_map;
  if (NULL == request_handler) {
    return -1; 
  }
  int ret = ngx_header_handler(r, query_map);
  ret = ngx_do_get_post_body(r, query_map);
  if (RET_OK != ret) {
     ngx_log_error(NGX_LOG_DEBUG,  r->connection->log, 0, 
        "ngx_do_get_post_body is error, ret=%d", ret);
    return ret;
  }
  ngx_query_map_print(r, query_map);
  string result;
  ret = ((Handler *)request_handler)->Handle(query_map, result);
  /*if (RET_OK != ret) {
    ngx_log_error(NGX_LOG_DEBUG,  r->connection->log, 0, 
        "handle is error: %d\n", ret);
    return -1;
  }*/
 
  *result_str = (char*)ngx_pcalloc(r->pool, result.size()+1);
  if (NULL == *result_str) {
    ngx_log_error(NGX_LOG_DEBUG,  r->connection->log, 0, 
        "pcalloc result_str is null");
    return -1;
  }
  strcpy(*result_str, result.c_str());
  return  ret;
}
