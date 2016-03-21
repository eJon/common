// Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
// Author: lisen01@baidu.com
// Brief: lib 管理器的基类，根据基准和增量维护tables,并提供lib中所有table的更新，查找操作。
// 默认实现了load及handle inc方法,并通过MVCC提供更新时的读写保护

#ifndef DAS_LIB_LIB_MANAGER_BASE_H
#define DAS_LIB_LIB_MANAGER_BASE_H

// Brief: lib管理类的基类
// 各具体子类的主要使用方法:
// 假设TempLibManager是其一个子类, tmp_mgr是一个实例
// 初始化: tmp_mgr.init(const comcfg::ConfigUnit& conf)
//      配置的加载和子类成员的初始化在TempLibManager::load_conf()中完成
//      例如，TempLibManager::load_conf()中可能的操作有:
//          读取conf
//          初始化自己的成员变量
//          增加一个通过文本方式加载的基准: add_literal_base_conf()
//          增加一条das增量流: add_das_inc()
//      各个表在初始化中通过register_tables()加入到table group中
// 
// 初始化完成后通过start_service()加载基准，就可以提供服务了

#include <cm_utility/file_watcher.h>
#include <cm_utility/align_hash.h>

#include "table_group.h"
#include "das_inc_manager.h"

namespace comcfg {
class ConfigUnit;
}

namespace das_lib {

// forward declaration
class DasIncConf; 

class LibManagerBase {
public:
    LibManagerBase();
    virtual ~LibManagerBase();
    
    template<typename Table>
    Table* get_table(const std::string& name) {
        int pos = _vm_tg.find_latest_read_only();
        if (pos < 0) {
            DL_LOG_FATAL("Failed to find ready on vm");
            return NULL;
        }
        return _vm_tg[pos].get_table<Table>(name);
    }

    bool init(const comcfg::ConfigUnit&);

    bool start_service();

    virtual bool handle_inc();

    TableGroup* latest_table_group();

protected:
    virtual bool load_conf(const comcfg::ConfigUnit&) = 0;

    // 添加待监控的file watcher 到监控列表
    void add_file_watcher(cm_utility::FileWatcher* fw);

    // 增加一条das增量流
    // 注意: 增量处理是有优先级的，前面添加的流比后添加的流的优先级高
    bool add_das_inc(const DasIncConf &das_inc_conf);
    
    // 增加一份文本基准conf，在load阶段，按照加入的顺序依次load文本基准
    bool add_literal_base_conf(const DasBaseConf &base_conf);
    
    virtual bool register_tables(TableGroup* table_group) = 0;

private:
    // 记录每次处理的各个层级增量数目
    typedef cm_utility::align_hash_map<uint32_t,uint32_t> IncCountMap;
    
    //给table group创建新的version
    //返回值: 新version的指针
    //@param pos:新version的位置，用于后续的freeze或者drop操作
    TableGroup *create_version(int *pos);
    
    bool load_base_indexes();
    
    // 以文本方式加载一条das流的基准
    virtual bool load_literal_base(const DasBaseConf &conf,
                                        TableGroup* p_table_group);
    
    //@param record: 读取的第一个record。之所以有这个参数是因为现在configio不支持在read_next前
    //               检查是否有新的增量到达，是否有增量到达的判定必须通过read_next函数
    //               是否有增量到达的判定必须在hand_inc_impl外层做，因为涉及是否需要创建版本切换
    //@param p_table_group: 待更新的table group
    bool handle_inc_impl(configio::DynamicRecord& record, TableGroup* p_table_group);
    
    void dump_table_if_required();
    void log_handle_inc_info(TableGroup* p_table_group) const;

    st::VersionManager<TableGroup> _vm_tg;

    // 存放所有file watcher的地址，便于检查是否需要create version
    std::vector<cm_utility::FileWatcher *> _file_watcher_list;

    // 需要通过文本基准加载的基准conf
    std::vector<DasBaseConf> _literal_base_conf_list;

    // 管理多条DAS流
    DasIncManager _inc_manager;

    cm_utility::FileWatcher _dump_req_watcher;    

    IncCountMap _inc_count_map;
    
    static const int TABLE_VERSION_NUM = 2;

};

}  // namespace das_lib

#endif
