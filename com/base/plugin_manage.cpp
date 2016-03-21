#include <dlfcn.h>
#include <mscom/base/plugin_manage.h>
#include <mscom/base/select_common.h>
#include "log.h"
typedef iplugin_t * (*create_plugin_fun)();

plugin_manage_t *
plugin_manage_create(const char *plugin_dir, const char *plugin_cfg_file_dir) {
    if(NULL == plugin_dir || NULL == plugin_cfg_file_dir) {
        ERROR(LOGROOT, "plugin_manage_create failed, detail: input param error");
        return NULL;
    }

    INFO(LOGROOT, "plugin_manage_create begin, plugin dir: %s, plugin cfg file dir: %s",
         plugin_dir, plugin_cfg_file_dir);

    plugin_manage_t *plugin_manage = NULL;
    plugin_manage = new(std::nothrow)plugin_manage_t;

    if(NULL == plugin_manage) {
        ERROR(LOGROOT, "plugin_manage_create failed, detail: alloc resource failed");
        return NULL;
    }

    plugin_manage->plugin_dir_ = plugin_dir;
    plugin_manage->plugin_cfg_file_dir_ = plugin_cfg_file_dir;
    INFO(LOGROOT, "plugin_manage_create end");
    return plugin_manage;
}

int
plugin_manage_destroy(plugin_manage_t *plugin_manage) {
    INFO(LOGROOT, "plugin_manage_destroy begin");

    if(NULL == plugin_manage) {
        ERROR(LOGROOT, "plugin_manage_destroy failed, detail: input param is null");
        return -1;
    }

    if(0 != plugin_manage_clear(plugin_manage)) {
        ERROR(LOGROOT, "plugin_manage_destroy failed, detail: plugin_manage_clear failed");
    }

    delete plugin_manage;
    INFO(LOGROOT, "plugin_manage_destroy end");
    return 0;
}

int
plugin_manage_insert(plugin_manage_t *plugin_manage, const string &query_name,
                     const string &so_name, const string &symbol_name) {
    INFO(LOGROOT, "plugin_manage_insert begin");

    if(NULL == plugin_manage || true == plugin_manage->plugin_dir_.empty() ||
            true == plugin_manage->plugin_cfg_file_dir_.empty() ||  true == query_name.empty() ||
            true == so_name.empty() || true == symbol_name.empty()) {
        ERROR(LOGROOT, "plugin_manage_insert failed, detail: input param error");
        return -1;
    }

    INFO(LOGROOT, "plugin_manage_insert begin, query name: %s, so name: %s, symbol name: %s",
         query_name.c_str(), so_name.c_str(), symbol_name.c_str());

    map<string, plugin_info_t *>::iterator iter = plugin_manage->plugins_.find(query_name);

    //time_t last_mode_time = -1;
    if(plugin_manage->plugins_.end() != iter) {
        if(NULL != iter->second) {
            ERROR(LOGROOT, "plugin_manage_insert failed, detail: plugin: %s is exist", query_name.c_str());
            return -1;
        }

        WARN(LOGROOT, "plugin_manage_insert failed, detail: plugin: %s is exist", query_name.c_str());
        plugin_manage->plugins_.erase(iter);
        /*
        if (0 != get_last_files_mod_time(iter->second->plugin_name_, last_mode_time)){
            ERROR(LOGROOT,"plugin_manage_insert failed, detail: get plugin: %s modefy time failed",
                iter->second->plugin_name_.c_str());
            return -1;
        }
        if (iter->second->plugin_last_mod_time_ == last_mode_time){
            if (0 != get_last_files_mod_time(iter->second->plugin_cfg_name_, last_mode_time)){
                ERROR(LOGROOT,"plugin_manage_insert failed, detail: get plugin: %s, conf:%s , modefy time failed",
                    iter->second->plugin_name_.c_str(), iter->second->plugin_cfg_name_.c_str());
                return -1;
            }
            if (iter->second->plugin_cfg_last_mod_time_ != last_mode_time){
                INFO(LOGROOT,"plugin_manage_insert plugin_manage_reload plugin: %s start", query_name.c_str());
                return plugin_manage_reload(plugin_manage, query_name);
            }

            return 0;
        }else{
            if (0 != plugin_manage_delete(plugin_manage, query_name)){
                ERROR(LOGROOT,"plugin_manage_insert failed, plugin_manage_delete plugin: %s failed", query_name.c_str());
                return -1;
            }
        }
        */
    }

    // 合成so的绝对路径
    int ret = -1;
    string so_full_path;
    string conf_full_path;
    string symbol_full_name;
    void *so_handle = NULL;
    iplugin_t *plugin = NULL;
    plugin_info_t *plugin_info = NULL;
    create_plugin_fun plugin_fun = NULL;

    so_full_path = plugin_manage->plugin_dir_;
    so_full_path += '/';
    so_full_path += so_name;

    conf_full_path = plugin_manage->plugin_cfg_file_dir_;
    conf_full_path += '/';
    conf_full_path += so_name;
    conf_full_path += ".conf";

    symbol_full_name = "create_";
    symbol_full_name += symbol_name;


    if(0 != plugin_manage->plugin_name_.count(so_full_path)
            || 0 != plugin_manage->symbol_name_.count(symbol_full_name)) {
        ERROR(LOGROOT, "plugin_manage_insert failed, plugin name: %s or symbol name: %s is duplicate",
              so_full_path.c_str(), symbol_full_name.c_str()
             );
        return -1;
    }

    // 加载动态库
    so_handle = dlopen(so_full_path.c_str(), RTLD_LAZY/*|RTLD_DEEPBIND*/);

    if(NULL == so_handle) {
        ERROR(LOGROOT, "%s load with error, errmsg: %s", so_full_path.c_str(), dlerror());
        ret = -1;
        goto Exit0;
    }

    INFO(LOGROOT, "plugin_manage_insert dlopen success, detail: plugin: %s", so_full_path.c_str());
    plugin_fun = (create_plugin_fun)dlsym(so_handle, symbol_full_name.c_str());

    if(NULL == plugin_fun || NULL == *plugin_fun) {
        ERROR(LOGROOT, "plugin_manage_insert failed, detail: dlsym error, errmsg: %s", dlerror());
        ret = -1;
        goto Exit0;
    }

    INFO(LOGROOT, "plugin_manage_insert dlsym success, detail: plugin: %s, plugin_func: %s",
         so_full_path.c_str(), symbol_full_name.c_str());
    // 获取插件实现
    plugin = (*plugin_fun)();
    INFO(LOGROOT, "plugin_manage_insert excute func %s success, detail: plugin: %s, plugin_func: %s",
         symbol_full_name.c_str(), so_full_path.c_str(), symbol_full_name.c_str());

    if(NULL == plugin) {
        ERROR(LOGROOT, "plugin_manage_insert failed, detail: dlsym error, errmsg: %s", dlerror());
        ret = -1;
        goto Exit0;
    }

    // 初始化插件
    if(0 != plugin->initialize(conf_full_path.c_str())) {
        ERROR(LOGROOT, "plugin_manage_insert failed, detail: plugin initialize error");
        ret = -1;
        goto Exit0;
    }

    INFO(LOGROOT, "plugin_manage_insert initialize success, detail: plugin: %s, plugin_func: %s",
         so_full_path.c_str(), symbol_full_name.c_str());
    // 保存动态库名称，动态库句柄和插件实现
    plugin_info = new(std::nothrow)plugin_info_t;

    if(NULL == plugin_info) {
        plugin->uninitialize();
        ERROR(LOGROOT, "plugin_manage_insert failed, detail: alloc resouce failed");
        ret = -1;
        goto Exit0;
    }

    plugin_info->is_copyed_ = 0;
    plugin_info->is_delete_ = 0;
    plugin_info->plugin_name_ = so_full_path;
    plugin_info->symbol_name_ = symbol_full_name;
    plugin_info->plugin_cfg_name_ = conf_full_path;
    /*
    if (0 != get_last_files_mod_time(so_full_path, plugin_info->plugin_last_mod_time_)){
        ERROR(LOGROOT,"plugin_manage_insert failed, detail: get file:%s modify time failed",
                so_full_path.c_str());
        ret = -1;
        goto Exit0;
    }
    if (0 != get_last_files_mod_time(conf_full_path, plugin_info->plugin_cfg_last_mod_time_)){
        ERROR(LOGROOT,"plugin_manage_insert failed, detail: get file:%s modify time failed",
            conf_full_path.c_str());
        ret = -1;
        goto Exit0;
    }
    */
    plugin_info->plugin_handle_ = so_handle;
    plugin_info->plugin_ = plugin;

    plugin_manage->plugins_[query_name] = plugin_info;
    plugin_manage->plugin_name_.insert(so_full_path);
    plugin_manage->symbol_name_.insert(symbol_full_name);
    ret = 0;
Exit0:

    if(0 != ret) {
        if(NULL != plugin) {
            delete plugin;
            plugin = NULL;
        }

        if(NULL != so_handle) {
            dlclose(so_handle);
            so_handle = NULL;
        }

        if(NULL != plugin_info) {
            delete plugin_info;
            plugin_info = NULL;
        }
    }

    INFO(LOGROOT, "plugin_manage_insert end");
    return ret;
}

/*
int
plugin_manage_reload(plugin_manage_t *plugin_manage, const string &query_name)
{
    INFO(LOGROOT,"plugin_manage_reload begin, detail: query name: %s", query_name.c_str());
    if (NULL == plugin_manage){
        ERROR(LOGROOT,"plugin_manage_reload failed, detail: plugin_manage is null");
        return -1;
    }
    map<string, plugin_info_t*>::iterator iter = plugin_manage->plugins_.find(query_name);
    if (plugin_manage->plugins_.end() == iter || NULL == iter->second){
        ERROR(LOGROOT,"plugin_manage_reload failed, detail: %s not found", query_name.c_str());
        return -1;
    }
    time_t last_mode_time = 0;
    if (0 != get_last_files_mod_time(iter->second->plugin_cfg_name_, last_mode_time)){
        ERROR(LOGROOT,"plugin_manage_reload failed, detail: get plugin:%s modefy time failed", query_name.c_str());
        return -1;
    }
    if (iter->second->plugin_cfg_last_mod_time_ != last_mode_time){
        iter->second->plugin_cfg_last_mod_time_ = last_mode_time;
        return iter->second->plugin_->reload_cfg();
    }
    INFO(LOGROOT,"plugin_manage_reload end");
    return 0;
}
*/

int
plugin_manage_delete(plugin_manage_t *plugin_manage, const string &query_name) {
    if(NULL == plugin_manage) {
        ERROR(LOGROOT, "plugin_manage_delete failed, detail: plugin_manage is null");
        return -1;
    }

    INFO(LOGROOT, "plugin_manage_delete begin, detail: query name: %s", query_name.c_str());
    map<string, plugin_info_t *>::iterator iter = plugin_manage->plugins_.find(query_name);

    if(plugin_manage->plugins_.end() == iter || NULL == iter->second || NULL == iter->second->plugin_) {
        ERROR(LOGROOT, "plugin_manage_delete failed, detail: %s not found", query_name.c_str());
        return -1;
    }

    if(0 != (iter->second->is_copyed_ & 0x01)) {
        return 0;
    }

    if(0 != iter->second->plugin_->uninitialize()) {
        ERROR(LOGROOT, "plugin_manage_delete failed, detail: %s uninitialize failed", query_name.c_str());
    }

    if(0 != dlclose(iter->second->plugin_handle_)) {
        ERROR(LOGROOT, "plugin_manage_delete failed, detail: dlclose failed, errmsg: %s", dlerror());
    }

    if(NULL != iter->second) {
        delete iter->second;
        iter->second = NULL;
    }

    plugin_manage->plugins_.erase(iter);
    plugin_manage->plugin_name_.erase(iter->second->plugin_name_);
    plugin_manage->symbol_name_.erase(iter->second->symbol_name_);
    INFO(LOGROOT, "plugin_manage_delete end");
    return 0;
}


int
plugin_manage_is_delete(plugin_manage_t *plugin_manage, const string &query_name, const string &so_name,
                        const string &symbol_name, int &is_delete) {
    INFO(LOGROOT, "plugin_manage_is_delete begin");

    if(NULL == plugin_manage) {
        ERROR(LOGROOT, "plugin_manage_find failed, detail: input param is null");
        return -1;
    }

    map<string, plugin_info_t *>::iterator iter = plugin_manage->plugins_.find(query_name);

    if(plugin_manage->plugins_.end() == iter || NULL == iter->second) {
        is_delete = 0;
        return 0;
    }

    string so_full_path;
    string symbol_full_name;

    so_full_path = plugin_manage->plugin_dir_;
    so_full_path += '/';
    so_full_path += so_name;

    symbol_full_name = "create_";
    symbol_full_name += symbol_name;

    if(iter->second->plugin_name_ != so_full_path || iter->second->symbol_name_ != symbol_full_name) {
        ERROR(LOGROOT, "plugin_manage_is_delete failed, plugin: %s found, but old plugin name: %s, olde symbol name: %s, "\
              "new plugin name: %s, new symbol name: %s, still use old configure", query_name.c_str(), iter->second->plugin_name_.c_str(),
              iter->second->symbol_name_.c_str(), so_full_path.c_str(), symbol_full_name.c_str()
             );
        return -1;
    }

    is_delete = ((0x01 == (iter->second->is_delete_ & 0x01)) ? 1 : 0);
    INFO(LOGROOT, "plugin_manage_is_delete end");
    return 0;
}

iplugin_t *
plugin_manage_find(plugin_manage_t *plugin_manage, const string &query_name) {
    INFO(LOGROOT, "plugin_manage_find begin");

    if(NULL == plugin_manage) {
        ERROR(LOGROOT, "plugin_manage_find failed, detail: input param is null");
        return NULL;
    }

    INFO(LOGROOT, "plugin_manage_find begin, detail: query name: %s", query_name.c_str());
    map<string, plugin_info_t *>::iterator iter = plugin_manage->plugins_.find(query_name);

    if(plugin_manage->plugins_.end() == iter || NULL == iter->second) {
        ERROR(LOGROOT, "plugin_manage_find failed, detail: %s not found", query_name.c_str());
        return NULL;
    }

    INFO(LOGROOT, "plugin_manage_find end");

    if(0 != (iter->second->is_delete_ & 0x01)) {
        return NULL;
    }

    INFO(LOGROOT, "plugin_manage_find end");
    return iter->second->plugin_;
}
int
plugin_manage_is_exist(plugin_manage_t *plugin_manage, const string &query_name,
                       const string &so_name, const string &symbol_name, int &is_exist) {
    INFO(LOGROOT, "plugin_manage_is_exist begin");

    if(NULL == plugin_manage) {
        ERROR(LOGROOT, "plugin_manage_is_exist failed, detail: plugin_manage is null");
        return -1;
    }

    map<string, plugin_info_t *>::iterator iter = plugin_manage->plugins_.find(query_name);

    if(plugin_manage->plugins_.end() == iter) {
        INFO(LOGROOT, "plugin_manage_is_exist failed, detail: %s not found", query_name.c_str());
        is_exist = 0;
        return 0;
    }

    if(NULL == iter->second) {
        INFO(LOGROOT, "plugin_manage_is_exist failed, detail: plugin %s not found", query_name.c_str());
        plugin_manage->plugins_.erase(iter);
        is_exist = 0;
        return 0;
    }

    string so_full_path;
    string symbol_full_name;

    so_full_path = plugin_manage->plugin_dir_;
    so_full_path += '/';
    so_full_path += so_name;

    symbol_full_name = "create_";
    symbol_full_name += symbol_name;

    if(iter->second->plugin_name_ != so_full_path || iter->second->symbol_name_ != symbol_full_name) {
        ERROR(LOGROOT, "plugin_manage_is_exist failed, plugin: %s found, but old plugin name: %s, olde symbol name: %s, "\
              "new plugin name: %s, new symbol name: %s, still use old configure", query_name.c_str(), iter->second->plugin_name_.c_str(),
              iter->second->symbol_name_.c_str(), so_full_path.c_str(), symbol_full_name.c_str()
             );
        return -1;
    }

    is_exist = 1;
    INFO(LOGROOT, "plugin_manage_is_exist end");
    return 0;
}

int
plugin_manage_clear(plugin_manage_t *plugin_manage) {
    if(NULL == plugin_manage) {
        ERROR(LOGROOT, "plugin_manage_clear failed, detail: plugin_manage is null");
        return -1;
    }

    INFO(LOGROOT, "plugin_manage_clear begin");

    for(map<string, plugin_info_t *>::iterator iter = plugin_manage->plugins_.begin();
            iter != plugin_manage->plugins_.end(); iter++) {
        INFO(LOGROOT, "plugin_manage_clear start clear plugin: %s", iter->first.c_str());

        if(NULL == iter->second || NULL == iter->second->plugin_ || NULL == iter->second->plugin_handle_) {
            ERROR(LOGROOT, "plugin_manage_clear failed, detail: plugin:%s not found", iter->first.c_str());
            return -1;
        }

        if(0 != (iter->second->is_copyed_ & 0x01)) {
            INFO(LOGROOT, "plugin_manage_clear plugin: %s is copyed", iter->first.c_str());
            iter->second->is_copyed_ = 0;
            continue;
        } else if(0 != (iter->second->is_delete_ & 0x01)) {
            WARN(LOGROOT, "plugin_manage_clear plugin: %s is delete", iter->first.c_str());
        } else {
            WARN(LOGROOT, "plugin_manage_clear plugin: %s is normal, it is not normal", iter->first.c_str());
        }

        if(0 != iter->second->plugin_->uninitialize()) {
            ERROR(LOGROOT, "plugin_manage_clear failed, detail: %s uninitialize failed", iter->first.c_str());
        }

        delete iter->second->plugin_;

        if(0 != dlclose(iter->second->plugin_handle_)) {
            ERROR(LOGROOT, "plugin_manage_delete failed, detail: dlclose failed, errmsg: %s", dlerror());
        }

        delete iter->second;
    }

    plugin_manage->plugins_.clear();
    plugin_manage->plugin_name_.clear();
    plugin_manage->symbol_name_.clear();
    INFO(LOGROOT, "plugin_manage_clear end");
    return 0;
}

/*
int plugin_manage_reload_all( plugin_manage_t *plugin_manage )
{
    if(NULL == plugin_manage){
        ERROR(LOGROOT,"plugin_manage_reload_all failed, detail: plugin_manage is null");
        return -1;
    }
    INFO(LOGROOT,"plugin_manage_reload_all begin");
    for (map<string, plugin_info_t*>::iterator iter = plugin_manage->plugins_.begin();
            iter != plugin_manage->plugins_.end(); iter++){
        if (0 != plugin_manage_reload(plugin_manage, iter->first)){
            ERROR(LOGROOT,"plugin_manage_reload_all failed, detail: plugin_manage_reload failed, plugin name: %s",
                iter->first.c_str());
        }
    }
    INFO(LOGROOT,"plugin_manage_reload_all end");
    return 0;
}
*/

int
plugin_manage_assign_all(plugin_manage_t *src, plugin_manage_t *dst) {
    INFO(LOGROOT, "plugin_manage_assign_all begin");

    if(NULL == src || NULL == dst) {
        ERROR(LOGROOT, "plugin_manage_assign_all failed, detail: NULL == src || NULL == dst");
        return -1;
    }

    if(src->plugin_cfg_file_dir_ != dst->plugin_cfg_file_dir_ || src->plugin_dir_ != dst->plugin_dir_) {
        return -1;
    }

    for(map<string, plugin_info_t *>::iterator iter = src->plugins_.begin(); iter != src->plugins_.end(); iter++) {
        if(NULL == iter->second) {
            WARN(LOGROOT, "plugin_manage_assign_all warn, detail: NULL == iter->second");
            continue;
        }

        if(0 != dst->plugins_.count(iter->first)) {
            ERROR(LOGROOT, "plugin_manage_assign_all failed, detail: plugin: %s exist is dst", iter->first.c_str());
            return -1;
        }

        iter->second->is_copyed_ = iter->second->is_copyed_ | 0x01;
        dst->plugins_.insert(*iter);
    }

    INFO(LOGROOT, "plugin_manage_assign_all end");
    return 0;
}

int
plugin_manage_mark_delete(plugin_manage_t *src, plugin_manage_t *dst) {
    INFO(LOGROOT, "plugin_manage_mark_delete begin");

    if(NULL == src || NULL == dst) {
        return -1;
    }

    for(map<string, plugin_info_t *>::iterator iter = src->plugins_.begin(); iter != src->plugins_.end(); iter++) {
        if(NULL == iter->second) {
            WARN(LOGROOT, "plugin_manage_assign_all warn, detail: NULL == iter->second");
            continue;
        }

        if(0 == dst->plugins_.count(iter->first)) {
            iter->second->is_copyed_ = 0;
            iter->second->is_delete_ = iter->second->is_delete_ | 0x01;
        }
    }

    INFO(LOGROOT, "plugin_manage_mark_delete end");
    return 0;
}

int
plugin_manage_assign(plugin_manage_t *src, plugin_manage_t *dst,
                     const string &query_name, const string &so_name, const string &symbol_name) {
    INFO(LOGROOT, "plugin_manage_assign begin");

    if(src == NULL || dst == NULL || true == query_name.empty()) {
        WARN(LOGROOT, "plugin_manage_assign warn, detail: src == NULL || dst == NULL || true == query_name.empty()");
        return -1;
    }

    map<string, plugin_info_t *>::iterator iter;
    iter = src->plugins_.find(query_name);

    if(src->plugins_.end() == iter || NULL == (*iter).second) {
        return 0;
    }

    if(0 != dst->plugins_.count(query_name)) {
        WARN(LOGROOT, "plugin_manage_assign warn, detail: plugin: %s is exist in dst", query_name.c_str());
        return -1;
    }

    string so_full_path;
    string symbol_full_name;

    so_full_path = src->plugin_dir_;
    so_full_path += '/';
    so_full_path += so_name;

    symbol_full_name = "create_";
    symbol_full_name += symbol_name;

    if(iter->second->plugin_name_ != so_full_path || iter->second->symbol_name_ != symbol_full_name) {
        ERROR(LOGROOT, "plugin_manage_assign failed, plugin: %s found, but old plugin name: %s, olde symbol name: %s, "\
              "new plugin name: %s, new symbol name: %s, still use old configure", query_name.c_str(), iter->second->plugin_name_.c_str(),
              iter->second->symbol_name_.c_str(), so_full_path.c_str(), symbol_full_name.c_str()
             );
        return -1;
    }

    (*iter).second->is_copyed_ = (*iter).second->is_copyed_ | 0x01;
    (*iter).second->is_delete_ = 0;
    dst->plugins_.insert(*iter);
    INFO(LOGROOT, "plugin_manage_assign end");
    return 0;
}

int
plugin_manage_statistics(plugin_manage_t *plugin_manage, char *buffer, size_t &len) {
    INFO(LOGROOT, "plugin_manage_statistics begin");

    if(NULL == plugin_manage || NULL == buffer || 0 >= len) {
        WARN(LOGROOT, "plugin_manage_statistics error, detail: NULL == plugin_manage || NULL == buffer || 0 >= len");
        return -1;
    }

    size_t used = 0;
    size_t un_used = 0;

    for(map<string, plugin_info_t *>::iterator iter = plugin_manage->plugins_.begin();
            iter != plugin_manage->plugins_.end(); iter++) {
        INFO(LOGROOT, "plugin_manage_statistics get plugin: %s statistics", iter->first.c_str());

        if(NULL == iter->second || NULL == iter->second->plugin_) {
            WARN(LOGROOT, "plugin_manage_statistics warn, detail: plugin: %s is empty", iter->first.c_str());
            continue;
        }

        if(0 != iter->second->is_delete_ & 0x01) {
            WARN(LOGROOT, "plugin_manage_statistics warn, detail: plugin: %s is delete", iter->first.c_str());
            continue;
        }

        if(len > used) {
            un_used = len - used;

            if(0 != iter->second->plugin_->statistics(buffer + used, un_used)) {
                WARN(LOGROOT, "plugin_manage_statistics warn, detail: plugin: %s statistics failed",
                     iter->first.c_str());
            }

            used += un_used;
        }

        INFO(LOGROOT, "plugin_manage_statistics get plugin: %s statistics buffer len: %lu", iter->first.c_str(), un_used);
    }

    len = used;
    INFO(LOGROOT, "plugin_manage_statistics end");
    return 0;
}


