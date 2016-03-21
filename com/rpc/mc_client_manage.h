#ifndef _MC_CLIENT_MANAGE_H_
#define _MC_CLIENT_MANAGE_H_
#include <mccli/mccli.h>
#include <mscom/base/select_common.h>

struct mc_client_manage_t {
    size_t    mc_max_failed_times_;
    size_t    mc_max_reconnect_times_;
    size_t    mc_sock_timeout_;
    size_t    mc_connect_timeout_;
    string    mc_ip_port_;
    mcContext **mc_context_;
    int       node_num_;
};

mc_client_manage_t *mc_client_manage_create(const char *conf_file_name);
int mc_client_manage_get(mc_client_manage_t *mc_client, const char *key, size_t key_len, item_data **value);
int mc_client_manage_destroy(mc_client_manage_t *mc_client);


#endif
