// Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
// Author: lisen01@baidu.com
// Brief: table manager的管理类，维护所有表的加载、增量更新、dump、统计等工作
// 但不提供MVCC

#include "table_group.h"
#include "table_defs.h"
#include "table_manager.hpp"
#include "das_lib_log.h"

namespace das_lib {

TableGroup::TableGroup()
    : _init(false)
{
}

bool TableGroup::register_table_info(const DasTableInfo& table_info)
{
    TableSeekType::iterator iter = _table_info_map.find(table_info.table_name);
    if (iter != _table_info_map.end()) {
        DL_LOG_FATAL("Table[%s] is already registered", table_info.p_table_mgr->desc().c_str());
        return false;
    }

    _table_info_map[table_info.table_name] = table_info;

    _table_manager_list.push_back(table_info);

    TableRegisteryType& inc_table_group = _inc_schedule_info[table_info.inc_level]; 
    inc_table_group.push_back(table_info);

    DL_LOG_DEBUG("registered table[%s] into level %u", 
                  table_info.table_name.c_str(), 
                  table_info.inc_level);
 
    return true;
}

TableGroup::TableGroup(const TableGroup& rhs)
{
    copy(rhs);
}

void TableGroup::copy(const TableGroup& rhs)
{
    TableRegisteryType::const_iterator iter = 
        rhs._table_manager_list.begin();

    for (;iter != rhs._table_manager_list.end(); ++ iter) {
        DasTableInfo table_info;
        table_info.p_table_mgr = iter->p_table_mgr->clone(this);
        if (NULL == table_info.p_table_mgr) {
            DL_LOG_FATAL("Fail to clone %s", iter->p_table_mgr->desc().c_str());
            return;
        }
        table_info.table_name = iter->p_table_mgr->desc();
        table_info.inc_level = iter->inc_level;

        if(!register_table_info(table_info)) {
            DL_LOG_FATAL("Fail to register %s", iter->p_table_mgr->desc().c_str());
            return;
        }
    }
    _init = true;
}

TableGroup::~TableGroup()
{
    TableRegisteryType::iterator iter = 
        _table_manager_list.begin();

    for (; iter != _table_manager_list.end(); ++iter) {
        delete iter->p_table_mgr;
        iter->p_table_mgr = NULL;
    }
}

TableGroup& TableGroup::operator= (const TableGroup& rhs)
{
    if (this == &rhs) {
        return *this;
    }

    if (!_init) {
        copy(rhs);
        return *this;
    }
    
    if (_table_manager_list.size() != rhs._table_manager_list.size()) {
        DL_LOG_FATAL("in operator =, table size not equal,[%lu]!=[%lu]",
                _table_manager_list.size(),rhs._table_manager_list.size());
        return *this;
    }
    
    TableRegisteryType::iterator iter = 
       _table_manager_list.begin();
    TableRegisteryType::const_iterator rhs_iter = 
        rhs._table_manager_list.begin();

    while (iter != _table_manager_list.end() && rhs_iter != rhs._table_manager_list.end()) {
        iter->table_name = rhs_iter->table_name;

        if (iter->p_table_mgr == NULL) {
            DL_LOG_FATAL("Null pointer of iter table mgr,name[%s]",
                    iter->table_name.c_str());
            return *this;
        }

        (*(iter->p_table_mgr)) = *(rhs_iter->p_table_mgr);

        ++iter;
        ++rhs_iter;
    }

    return *this;
}

bool TableGroup::register_table(IBaseTableManager* table, int inc_level)
{
    if (table == NULL) {
        DL_LOG_FATAL("Null pointer of table");
        return false;
    }
    
    DasTableInfo table_info;
    table_info.p_table_mgr = table;
    table_info.table_name = table->desc();
    table_info.inc_level = inc_level;

    return register_table_info(table_info); 
}

bool TableGroup::init()
{
    TableRegisteryType::iterator iter = 
        _table_manager_list.begin();
    for (; iter != _table_manager_list.end(); ++iter) {
        if (!iter->p_table_mgr->init()) {
            DL_LOG_FATAL("Fail to init table[%s]", iter->table_name.c_str());
            return false;
        }
    }

    _init = true;
    
    return true;
}

bool TableGroup::pre_load()
{
    bool ret = true;
    
    TableRegisteryType::iterator iter;
    for (iter = _table_manager_list.begin(); iter != _table_manager_list.end(); ++iter) {
        ret = iter->p_table_mgr->pre_load();
        if (!ret) {
            DL_LOG_FATAL("Before load table[%s] failed", iter->table_name.c_str());
            break;
        }
    }
 
    return ret;
}

bool TableGroup::post_load()
{
    bool ret = true;
    
    TableRegisteryType::iterator iter;
    for (iter = _table_manager_list.begin(); iter != _table_manager_list.end(); ++iter) {
        ret = iter->p_table_mgr->post_load();
        if (!ret) {
            DL_LOG_FATAL("Before load table[%s] failed", iter->table_name.c_str());
            break;
        }
    }
 
    return ret;
}

bool TableGroup::load()
{
    bool ret = true;
    
    TableRegisteryType::iterator iter;
    for (iter = _table_manager_list.begin(); iter != _table_manager_list.end(); ++iter) {
        ret = iter->p_table_mgr->load();
        if (!ret) {
            DL_LOG_FATAL("Load table[%s] failed", iter->table_name.c_str());
            break;
        }
    }
 
    return ret;
}

bool TableGroup::reload()
{
    bool ret = true;
    
    TableRegisteryType::iterator iter;
    for (iter = _table_manager_list.begin(); iter != _table_manager_list.end(); ++iter) {
        ret = iter->p_table_mgr->reload();
        if (!ret) {
            DL_LOG_FATAL("Fail to reload table[%s]",iter->table_name.c_str());
            break;
        }
    }
 
    return ret;
}

bool TableGroup::serialize(const std::string& dump_dir, const std::string &table_name) const
{
    if (table_name == "all") {
        serialize(dump_dir);
        return true;
    }
    
    std::string file_path = dump_dir + "/" + table_name;

    TableSeekType::const_iterator iter = _table_info_map.find(table_name);
    if (iter == _table_info_map.end()) {
        DL_LOG_WARNING("Failed to serialize table[%s], can't find name", table_name.c_str());
        return false;
    }

    iter->second.p_table_mgr->serialize(file_path.c_str());

    return true;
}

void TableGroup::serialize(const std::string& dump_dir) const
{
    TableRegisteryType::const_iterator iter = 
        _table_manager_list.begin();
    for (; iter != _table_manager_list.end(); ++iter) {
        std::string file_path = dump_dir + "/" + iter->table_name;
        iter->p_table_mgr->serialize(file_path.c_str());
    }
}

bool TableGroup::handle_inc(DynamicRecord &inc, uint32_t level)
{
    bool ret = true;

    IncScheduleInfoType::iterator inc_iter = 
        _inc_schedule_info.find(level);
    if (inc_iter == _inc_schedule_info.end()) {
        //找不到对应层级增量处理的类
        DL_LOG_TRACE("Fail to find inc level %u", level);
        return true;
    }

    TableRegisteryType::iterator table_iter = inc_iter->second.begin();
    for (; table_iter != inc_iter->second.end(); ++ table_iter) {
        if (NULL == table_iter->p_table_mgr) {
            DL_LOG_FATAL("table[%s] is NULL [%p]", table_iter->table_name.c_str(), 
                table_iter->p_table_mgr);
            return false;
        }
        if (!table_iter->p_table_mgr->update(inc)) {
            ret = false;
            DL_LOG_FATAL("Fail to handle inc for table[%s]", table_iter->table_name.c_str());
        }
    }

    return ret;
}

IBaseTableManager* TableGroup::mutable_table_manager(const std::string& name)
{
    TableSeekType::iterator iter = _table_info_map.find(name);
    if (iter == _table_info_map.end()) {
        DL_LOG_FATAL("Failed to find table[%s]", name.c_str());
        return NULL;
    }

    return iter->second.p_table_mgr;
}

void TableGroup::log_table_info() const 
{
    size_t sum_mem = 0;
    uint64_t sum_size = 0;
    long proc_mem = resident_mem_of_process();

    DL_LOG_MON("+-------------------------------------------------------+");
    DL_LOG_MON("| Entity          | Memory(MB) | Size                   |");
    DL_LOG_MON("|-----------------+------------+------------------------|");

    TableRegisteryType::const_iterator iter = 
        _table_manager_list.begin();
    for (; iter != _table_manager_list.end(); ++iter) {
        if (iter->p_table_mgr == NULL) {
            DL_LOG_FATAL("Null table[%s] pointer", iter->table_name.c_str());
        }
        sum_mem += iter->p_table_mgr->mem();
        sum_size += iter->p_table_mgr->size();
        DL_LOG_MON("| %-15s | %-10.2f | %-8.2d |",
                iter->table_name.c_str(), MB(iter->p_table_mgr->mem()), iter->p_table_mgr->size());
    }

    DL_LOG_MON("|-----------------+------------+------------------------|");
    DL_LOG_MON("| %-15s | %-10.2f | %-8.2f |",
                 "sum up", MB(sum_mem), sum_size);
    DL_LOG_MON("| %-15s | %-10.2f | ", "other resident", MB(proc_mem - sum_mem));
    DL_LOG_MON("+-------------------------------------------------------+");

    return;
}

}//namespace das_lib

