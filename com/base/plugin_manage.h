#ifndef _PLUGIN_MANAGE_H_
#define _PLUGIN_MANAGE_H_
#include <string>
#include <map>
#include <set>
#include <mscom/iface/iplugin.h>

using namespace std;
struct plugin_info_t {
    int       is_copyed_;
    int       is_delete_;
    string    plugin_name_;    //插件名字
    string    plugin_cfg_name_;
    string    symbol_name_;    //动态库里变量的符号名
    void      *plugin_handle_; // 动态库句柄
    //time_t    plugin_last_mod_time_;
    //time_t    plugin_cfg_last_mod_time_;
    iplugin_t *plugin_;        // 插件实现
};

struct plugin_manage_t {
    string plugin_dir_;
    string plugin_cfg_file_dir_;
    map<string, plugin_info_t *> plugins_;
    set<string> plugin_name_;
    set<string> symbol_name_;
};

plugin_manage_t *plugin_manage_create(const char *plugin_dir, const char *plugin_cfg_file_dir);
int plugin_manage_destroy(plugin_manage_t *plugin_manage);
int plugin_manage_insert(plugin_manage_t *plugin_manage, const string &query_name,
                         const string &so_name, const string &symbol_name);
/*
int plugin_manage_reload(plugin_manage_t *plugin_manage, const string &query_name);
int plugin_manage_reload_all(plugin_manage_t *plugin_manage);
*/
int plugin_manage_delete(plugin_manage_t *plugin_manage, const string &query_name);
int plugin_manage_clear(plugin_manage_t *plugin_manage);
int plugin_manage_is_delete(plugin_manage_t *plugin_manage, const string &query_name, const string &so_name,
                            const string &symbol_name, int &is_delete);
iplugin_t *plugin_manage_find(plugin_manage_t *plugin_manage, const string &query_name);
int plugin_manage_assign_all(plugin_manage_t *src, plugin_manage_t *dst);
int plugin_manage_assign(plugin_manage_t *src, plugin_manage_t *dst,
                         const string &query_name, const string &so_name, const string &symbol_name);
int plugin_manage_is_exist(plugin_manage_t *plugin_manage, const string &query_name, const string &so_name,
                           const string &symbol_name, int &is_exist);
int plugin_manage_mark_delete(plugin_manage_t *src, plugin_manage_t *dst);
int plugin_manage_statistics(plugin_manage_t *plugin_manage, char *buffer, size_t &len);
#endif
