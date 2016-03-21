// Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
// Author: luanzhaopeng@baidu.com
// Brief: 使用configio做增量更新，smalltable作为底层存储的索引类
#ifndef DAS_LIB_TABLE_MANAGER_H
#define DAS_LIB_TABLE_MANAGER_H

#include "table_strategy.hpp"

namespace configio {
class DynamicRecord;
}
 
namespace das_lib {

class TableGroup;    

class IBaseTableManager {
public:
    virtual ~IBaseTableManager(){}
    virtual bool init() = 0;
    virtual bool pre_load() = 0;
    virtual bool load() = 0;
    virtual bool post_load() = 0;
    virtual bool update(const DynamicRecord &inc_record) = 0;
    virtual bool reload() = 0;

    //不同版本的TableGroup不同，因此需要在clone时传递
    virtual IBaseTableManager *clone(TableGroup *pTable_group) const = 0;
    virtual IBaseTableManager &operator=(const IBaseTableManager &) = 0;

    virtual size_t mem() const = 0;
    virtual const std::string &desc() const = 0;
    virtual size_t size() const = 0;
    virtual void serialize(const char *path) const = 0;
};

//smalltable管理类，可以定制不同的加载和更新策略
//例如文本基准加载的正排表使用增量更新策略，不需要指定加载策略
//connector生成的倒排表使用connector加载策略，不需要指定更新策略
//由于加载和更新策略都可以为空，如果没有加载策略，pre_load()/load()/post_load()直接返回true
//如果没有更新策略，update()直接返回true
template <class Table>
class TableManager : public IBaseTableManager {
public:
    TableManager(const std::string &desc,
                    IBaseLoadStrategy<Table> *p_load_strategy,
                    IBaseUpdateStrategy<Table> *p_update_strategy,
                    TableGroup *pTable_group);
    virtual ~TableManager();

    virtual bool init();
    virtual bool pre_load();
    virtual bool load();
    virtual bool post_load();
    virtual bool update(const DynamicRecord &inc_record);
    virtual bool reload();
    
    // brief: allocate a TableManager of the same type, 
    //        clone the load and update strategy and init them
    //        set its table group pointer using the parmater
    virtual TableManager *clone(TableGroup *pTable_group) const;

    // note: is meant to be used by version table manager
    // so, table name, load & update strategies will not be copied
    // only copy the small table
    virtual TableManager &operator=(const IBaseTableManager &rhs);

    virtual const std::string &desc() const;
    virtual size_t mem() const;
    virtual size_t size() const ;
    virtual void serialize(const char *path) const;
    virtual void dump_info() const;

    Table *mutable_table();
    const Table &get_table() const;

private:
    TableManager();
    TableManager(const TableManager &rhs);
    TableManager &operator=(const TableManager &rhs);
   
    Table _table;  
    const std::string _desc;
    IBaseLoadStrategy<Table> *_p_load_strategy;     //own this object
    IBaseUpdateStrategy<Table> *_p_update_strategy; //own this object
    TableGroup *_p_table_group;    //not own this object

    static const uint32_t RESERVE_SIZE = 100000;
};

} //namespace das lib

#include "table_manager_inl.hpp"

#endif
