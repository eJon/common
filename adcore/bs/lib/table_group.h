// Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
// Author: lisen01@baidu.com
// Brief: table manager的管理类，维护所有表的加载、增量更新、dump、统计等工作
// 但不提供MVCC

#ifndef DAS_LIB_TABLE_GROUP_H
#define DAS_LIB_TABLE_GROUP_H

#include "table_manager.hpp"
#include <cm_utility/common.h>

namespace das_lib {

class TableGroup {
public:
    TableGroup();
    TableGroup(const TableGroup& rhs);
    ~TableGroup();
    TableGroup& operator=(const TableGroup& rhs);
    
    bool register_table(IBaseTableManager* table, int inc_level);
    
    bool init();
    bool pre_load();
    bool load();
    bool post_load();
    bool reload();
    bool handle_inc(DynamicRecord & inc, uint32_t level);

    const IBaseTableManager* get_table_manager(const std::string& name) {
        return mutable_table_manager(name);
    }

    template<typename To>
    const To* cast_const_table_manager(const std::string& name)
    {
        return down_cast<const To*>(get_table_manager(name));
    }

    template<typename TableType>
    TableType* get_table(const std::string& name)
    {
        TableManager<TableType> *p_tm =
            cast_mutable_table_manager< TableManager<TableType> >(name);
        if (NULL == p_tm) {
            DL_LOG_FATAL("fail to get table manager :%s", name.c_str());
            return NULL;
        }
        return p_tm->mutable_table();
    }

    bool is_init() const {
        return _init;
    }

    int table_num() const {
        return _inc_schedule_info.size();
    }

    bool serialize(const std::string& dump_dir, const std::string &table_name) const;
    void serialize(const std::string& dump_dir) const;
    void log_table_info() const;
    
private:
    struct DasTableInfo {
        IBaseTableManager* p_table_mgr;
        std::string table_name;   
        int inc_level;

        bool operator==(const DasTableInfo& rhs) const
        {
            return table_name == rhs.table_name;
        }
    };

    typedef std::vector<DasTableInfo> TableRegisteryType;
    typedef std::map<std::string, DasTableInfo> TableSeekType;
    typedef std::map<int, TableRegisteryType> IncScheduleInfoType;

    bool register_table_info(const DasTableInfo& table_info);

    void copy(const TableGroup& rhs);

    IBaseTableManager* mutable_table_manager(const std::string& name);

    template<typename To>
    To* cast_mutable_table_manager(const std::string& name)
    {
        return down_cast<To*>(mutable_table_manager(name));
    }
   
    //注册的顺序,对load,reload,update要按照这个顺序执行
    TableRegisteryType _table_manager_list;

    //用以更快的找到表, table_name -> DasTableInfo
    TableSeekType _table_info_map;
    
    //inc level->table_info，为方便按层级处理增量需求
    IncScheduleInfoType _inc_schedule_info;

    //是否已经初始化,这里是广义的初始化判断，当拷贝构造时也算作已经
    //被初始化了
    bool _init;
};

}  // namespace das_lib

#endif  // DAS_LIB_TABLE_GROUP_H
