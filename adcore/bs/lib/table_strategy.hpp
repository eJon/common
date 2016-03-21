// Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
// Author: luanzhaopeng@baidu.com

#ifndef DAS_LIB_TABLE_STRATEGY_H
#define DAS_LIB_TABLE_STRATEGY_H

#include <iostream>
#include <sstream>
#include <string>

#include <cm_utility/file_watcher.h>

#include "lib_util.h"

namespace configio {
class DynamicRecord;
}

namespace das_lib {

//管理类的前向声明    
class TableGroup; 

template <class Table>
class IBaseLoadStrategy {
public:
    virtual ~IBaseLoadStrategy() = 0;
    virtual bool init(Table &table) = 0;
    virtual bool pre_load(Table &table) = 0;
    virtual bool load(Table &table) = 0;
    virtual bool post_load(Table &table) = 0;
    virtual bool reload(Table &table) = 0;
    virtual IBaseLoadStrategy *clone(TableGroup *p_table_group) const = 0;
};

//用于倒排的创建
//ConnArg 是连接参数模板，默认支持一个连接参数，如果需要更多参数
//可以将使用结构体作为参数类型
template <class Table, class Connector, typename ConnArg = int>
class ConnectorLoadStrategy : public IBaseLoadStrategy<Table> {
public:
    //生成connector的方法，注意需要在返回前进行connect()操作
    typedef Connector *(*ConnectorMaker)(Table &table, TableGroup &table_group, ConnArg);

    ConnectorLoadStrategy(const std::string &desc,
                                ConnectorMaker connector_maker,
                                TableGroup *p_table_group,
                                ConnArg connect_arg);

    virtual ~ConnectorLoadStrategy();
    virtual bool init(Table &table);
    virtual bool pre_load(Table &table) ;
    virtual bool load(Table &table);
    virtual bool post_load(Table &table);
    virtual bool reload(Table &table);
    virtual ConnectorLoadStrategy *clone(TableGroup *p_table_group) const;

private:
    ConnectorLoadStrategy(const ConnectorLoadStrategy &rhs);
    void enable_connector(Table &table);
    void disable_connector(Table &table);
    void show_connector() const;

    const std::string _conn_desc;
    Connector *_p_connector;       //own this object
    ConnectorMaker _connector_maker;
    ConnArg _connect_arg;
    TableGroup *_p_table_group;    // not own this object
};

/*
brief:
通过读取整个文件来加载smalltable的策略类
用一个位于该策略类外部的file watcher来监控数据文件的变化，
如果发现文件更新，需要依次调用该类的pre_load()，load()，post_load()方法
来完成表的reload；

使用者可以提供pre_load和post_load两个handler，如果不需要，传入NULL即可；
一般的，pre_load的操作是将该策略管理的表涉及到的其他倒排的连接器断开，
以避免该表重新加载导致大量的连接操作；
post_load中进行恢复连接器的操作；

将file watcher放在类外部的原因是lib manager需要检查file watcher来判断是否需要
进行table group的creat version操作；
file watcher的更新在reload()方法中完成；
*/
template <class Table>
class FileLoadStrategy : public IBaseLoadStrategy<Table> {
public:
    typedef bool (*PreLoader)(TableGroup &);
    typedef bool (*Loader)(Table &, const char*, const PartitionArg *);
    typedef bool (*PostLoader)(TableGroup &);
    
    FileLoadStrategy(const PartitionArg *p_part_arg,
                        PreLoader pre_loader,
                        Loader loader,
                        PostLoader post_loader,
                        cm_utility::FileWatcher &_file_watcher,
                        TableGroup *p_table_group
                        );

    virtual bool init(Table &table);
    virtual bool pre_load(Table &table);
    virtual bool load(Table &table);
    virtual bool post_load(Table &table);
    virtual bool reload(Table &table);
    virtual FileLoadStrategy *clone(TableGroup *p_table_group) const;

private:
    FileLoadStrategy(const FileLoadStrategy &rhs);

    const PartitionArg *_p_part_arg;    //maybe NULL when there is no partition need

    const PreLoader _pre_load_handler;
    const Loader _load_handler;
    const PostLoader _post_load_handler;

    cm_utility::FileWatcher &_file_watcher; // not own this object
    TableGroup *_p_table_group;    // not own this object
};

template <class Table>
class IBaseUpdateStrategy {
public:
    virtual ~IBaseUpdateStrategy() = 0;
                
    virtual bool init(Table &table) = 0;
    virtual bool update(Table &table, const configio::DynamicRecord &) = 0;
    virtual IBaseUpdateStrategy *clone() const = 0;
};

template <class Table>
class IncUpdateStrategy : public IBaseUpdateStrategy<Table> {
public:
    typedef bool (*UpdateHandler)(Table &table, const configio::DynamicRecord &);
    
    explicit IncUpdateStrategy(UpdateHandler update_handler)
        : _update_handler(update_handler)
    {}

    virtual bool init(Table &table);
    virtual bool update(Table &table, const configio::DynamicRecord &);
    virtual IncUpdateStrategy *clone() const;
    
private:
    UpdateHandler _update_handler;
};

} //namespace das-lib

#include "table_strategy_inl.hpp"

#endif
