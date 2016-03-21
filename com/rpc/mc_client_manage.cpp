#include "mc_client_manage.h"
#include <mscom/base/crc32.h>
#include <mscom/base/xml_cfg.h>
#include <mscom/base/log.h>

#define MAX_KEY_LEN 256

using namespace ms::log;

mc_client_manage_t *
mc_client_manage_create(const char *conf_file_name) {
    if(NULL == conf_file_name) {
        ERROR(LOGROOT, "mc_client_manage_create failed, detail: conf file name is null");
        return NULL;
    }
    INFO(LOGROOT, "mc_client_manage_create begin");
    int ret = -1;
    xml_cfg_t mc_conf;
    list<string> ip_port_pair;
    vector<string> buffer;
    size_t index = 0;
    struct timeval connect_timeout;
    struct timeval sock_timeout;
    mc_client_manage_t *mc_client_manage = NULL;
    mc_client_manage = new(std::nothrow)mc_client_manage_t;
    if(NULL == mc_client_manage) {
        ERROR(LOGROOT, "mc_client_manage_create failed, detail: alloc mc_client_manage_t resource failed");
        ret = -1;
        goto Exit0;
    }
    mc_client_manage->mc_context_ = NULL;
    if(0 != mc_conf.parse_config(conf_file_name)) {
        ERROR(LOGROOT, "mc_client_manage_create failed, detail: parse_config failed");
        ret = -1;
        goto Exit0;
    }
    if(0 != mc_conf.get_unsigned("mc_sock_timeout", mc_client_manage->mc_sock_timeout_, 5)) {
        ERROR(LOGROOT, "mc_client_manage_create failed, detail: get mc_sock_timeout failed");
        ret = -1;
        goto Exit0;
    }
    if(0 != mc_conf.get_unsigned("mc_connect_timeout", mc_client_manage->mc_connect_timeout_, 5)) {
        ERROR(LOGROOT, "mc_client_manage_create failed, detail: get mc_connect_timeout failed");
        ret = -1;
        goto Exit0;
    }
    if(0 != mc_conf.get_unsigned("mc_max_failed_times", mc_client_manage->mc_max_failed_times_, 5)) {
        ERROR(LOGROOT, "mc_client_manage_create failed, detail: get mc_max_failed_times failed");
        ret = -1;
        goto Exit0;
    }
    if(0 != mc_conf.get_unsigned("mc_max_reconnect_times", mc_client_manage->mc_max_reconnect_times_, 3)) {
        ERROR(LOGROOT, "mc_client_manage_create failed, detail: get mc_max_reconnect_times failed");
        ret = -1;
        goto Exit0;
    }
    if(0 != mc_conf.get_string("mc_ip_port", mc_client_manage->mc_ip_port_)) {
        ERROR(LOGROOT, "mc_client_manage_create failed, detail: mc_ip_port not found");
        ret = -1;
        goto Exit0;
    }

    connect_timeout.tv_sec = 0;
    connect_timeout.tv_usec = mc_client_manage->mc_connect_timeout_ *  1000;
    sock_timeout.tv_sec = 0;
    sock_timeout.tv_usec = mc_client_manage->mc_sock_timeout_ * 1000;

    if(0 != parse_filter(mc_client_manage->mc_ip_port_, ip_port_pair, "|")) {
        ERROR(LOGROOT, "mc_client_manage_create failed, detail: parse_filter failed");
        ret = -1;
        goto Exit0;
    }
    mc_client_manage->mc_context_ = new(std::nothrow)mcContext*[ip_port_pair.size()];
    if(NULL == mc_client_manage->mc_context_) {
        ERROR(LOGROOT, "mc_client_manage_create failed, detail: alloc mcContext resource failed");
        ret = -1;
        goto Exit0;
    }
    mc_client_manage->node_num_ = ip_port_pair.size();
    for(list<string>::iterator iter = ip_port_pair.begin(); iter != ip_port_pair.end(); iter++, index++) {
        buffer.clear();
        if(0 != parse_filter(*iter, buffer, ":")) {
            ERROR(LOGROOT, "mc_client_manage_create failed, detail: parse_filter failed, buffer: %s",
                         (*iter).c_str());
            ret = -1;
            goto Exit0;
        }
        if(4 != buffer.size()) {
            ERROR(LOGROOT, "mc_client_manage_create failed, detail: ip_port_pair is not right");
            ret = -1;
            goto Exit0;
        }

        //connect_timeout:connect timeout time, sock_timeout: sock timeout time,
        //mc_maxsrtimes: reconnect times, mc_maxrctimes:primary max failed times, choose secondary
        mc_client_manage->mc_context_[index] = mcConnectWithTimeout(buffer[0].c_str(), atoi(buffer[1].c_str()),
                                               buffer[2].c_str(), atoi(buffer[3].c_str()), connect_timeout, sock_timeout,
                                               mc_client_manage->mc_max_reconnect_times_, mc_client_manage->mc_max_failed_times_);
        if(NULL == mc_client_manage->mc_context_[index]) {
            ERROR(LOGROOT, "mc_client_manage_create failed, detail: mcConnectWithTimeout failed, buffer: %s",
                         (*iter).c_str());
            ret = -1;
            goto Exit0;
        }
    }
    ret = 0;
Exit0:
    if(0 != ret) {
        if(NULL != mc_client_manage && NULL != mc_client_manage->mc_context_) {
            delete mc_client_manage->mc_context_;
            mc_client_manage->mc_context_ = NULL;
        }
        if(NULL != mc_client_manage) {
            delete mc_client_manage;
            mc_client_manage = NULL;
        }
    }
    INFO(LOGROOT, "mc_client_manage_create end");
    return mc_client_manage;
}

int
mc_client_manage_get(mc_client_manage_t *mc_client, const char *key, size_t key_len, item_data **value) {
    if(NULL == mc_client || NULL == key || 0 == key_len || 0 == mc_client->node_num_ ||
            NULL == mc_client->mc_context_) {
        ERROR(LOGROOT, "mc_client_manage_get failed, detail: input param failed");
        return -1;
    }
    INFO(LOGROOT, "mc_client_manage_get begin");
    int index = crc32((char *)key, key_len) % mc_client->node_num_;
    *value = (item_data*)mc_get(mc_client->mc_context_[index], key);
    INFO(LOGROOT, "mc_client_manage_get end");
    return 0;
}

int
mc_client_manage_destroy(mc_client_manage_t *mc_client) {
    if(NULL == mc_client) {
        ERROR(LOGROOT, "mc_client_manage_destroy failed, detail: input param failed");
        return -1;
    }
    INFO(LOGROOT, "mc_client_manage_destroy begin");
    if(NULL != mc_client->mc_context_) {
        for(int count = 0; count < mc_client->node_num_; count++) {
            if(NULL != mc_client->mc_context_[count]) {
                mcFree(mc_client->mc_context_[count]);
                mc_client->mc_context_[count] = NULL;
            }
        }
        delete []mc_client->mc_context_;
        mc_client->mc_context_ = NULL;
    }
    delete mc_client;
    INFO(LOGROOT, "mc_client_manage_destroy end");
    return 0;
}
