// Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
// Author: luanzhaopeng@baidu.com

#include <cm_utility/common.h>

#include "lib_util.h"
#include "lib_dump.hpp"

namespace das_lib {

template <class Table>
TableManager<Table>::TableManager(const std::string &desc,
                    IBaseLoadStrategy<Table> *p_load_strategy,
                    IBaseUpdateStrategy<Table> *p_update_strategy,
                    TableGroup *p_table_group)
        : _desc(desc)
        , _p_load_strategy(p_load_strategy)
        , _p_update_strategy(p_update_strategy)
        , _p_table_group(p_table_group)
{}

template<class Table>
TableManager<Table>::~TableManager()
{
    if (NULL != _p_load_strategy) {
        delete _p_load_strategy;
        _p_load_strategy = NULL;
    }

    if (NULL != _p_update_strategy) {
        delete _p_update_strategy;
        _p_update_strategy = NULL;
    }
}

template<class Table>
const std::string& TableManager<Table>::desc() const
{
    return _desc;
}

template<class Table>
size_t TableManager<Table>::mem() const
{
    return _table.mem(); 
}

template<class Table>
size_t TableManager<Table>::size() const 
{
    return _table.size();
}

template<class Table>
Table * TableManager<Table>::mutable_table() 
{ 
    return &_table; 
}

template<class Table>
const Table &TableManager<Table>::get_table() const
{
    return _table;
}

template <class Table>
void TableManager<Table>::serialize(const char *path) const
{
    if (NULL == path) {
        DL_LOG_FATAL("path is NULL");
        return;
    }

    std::ofstream ofs;
    ofs.open(path);
    if(!ofs.is_open()) {                                                
        DL_LOG_WARNING("Fail to open file to dump, file:%s", path); 
        return;
    }
    
    dump_table(&_table, ofs, _desc.c_str());

    ofs.close();
}

template <class Table>
TableManager<Table>::TableManager(const TableManager &rhs)
    : _table(rhs._table)
    , _desc(rhs._desc)
    , _p_load_strategy(NULL)
    , _p_update_strategy(NULL)
    , _p_table_group(NULL)
{
    // note that we should not copy any pointers here;
}

template <class Table>
TableManager<Table> &TableManager<Table>::operator=(const IBaseTableManager &rhs)
{
    if (this == &rhs) {
        return *this;
    }

    const TableManager<Table> *pTable_manager = down_cast<const TableManager<Table> *>(&rhs);
    if (NULL == pTable_manager) {
        DL_LOG_FATAL("Fail to cast table %s, maybe they are of different types", _desc.c_str());
        return *this;
    }

    // only small table needs to be copied
    _table = pTable_manager->_table;
    
    return *this;
}

template <class Table>
TableManager<Table> *
TableManager<Table>::clone(TableGroup *p_table_group) const
{
    TableManager<Table> *p_tm = new(std::nothrow) TableManager<Table>(*this);
    if (NULL == p_tm) {
        DL_LOG_FATAL("Fail to allocate TableManager for %s", _desc.c_str());
        return NULL;
    }

    p_tm->_p_table_group = p_table_group;

    if (NULL != _p_load_strategy) {
        p_tm->_p_load_strategy = _p_load_strategy->clone(p_table_group);
        if (NULL == p_tm->_p_load_strategy) {
            DL_LOG_FATAL("Fail to clone load strategy for %s", _desc.c_str());
            delete p_tm;
            return NULL;
        }

        if (!p_tm->_p_load_strategy->init(p_tm->_table)) {
            DL_LOG_FATAL("Fail to init load strategy for %s", _desc.c_str());
            delete p_tm;
            return NULL;
        }
    }
    if (NULL != _p_update_strategy) {
        p_tm->_p_update_strategy = _p_update_strategy->clone();
        if (NULL == p_tm->_p_update_strategy) {
            DL_LOG_FATAL("Fail to clone update strategy for %s", _desc.c_str());
            delete p_tm;
            return NULL;
        }
        if (!p_tm->_p_update_strategy->init(p_tm->_table)) {
            DL_LOG_FATAL("Fail to init update strategy for %s", _desc.c_str());
            delete p_tm;
            return NULL;
        }
        
    }

    DL_LOG_DEBUG("cloned a TableManager for %s", _desc.c_str());
    return p_tm;
}

template <class Table>
bool TableManager<Table>::init()
{
    int ret = _table.init();
    if (ret < 0) {
        DL_LOG_FATAL("Fail to init table %s", _desc.c_str());
        return false;
    }

    _table.resize(RESERVE_SIZE);

    if (NULL != _p_load_strategy) {
        if (!_p_load_strategy->init(_table)) {
            DL_LOG_FATAL("Fail to init load strategy");
            return false;
        }
    }

    if (NULL != _p_update_strategy) {
        if (!_p_update_strategy->init(_table)) {
            DL_LOG_FATAL("Fail to init update strategy");
            return false;
        }
    }

    return true;
}

template <class Table>
bool TableManager<Table>::load()
{
    if (NULL == _p_load_strategy) {
        DL_LOG_DEBUG("load strategy of %s is NULL", _desc.c_str());
        return true;
    }
    st::Timer tm;
    tm.start();
    bool ret = _p_load_strategy->load(_table);
    if (!ret) {
        DL_LOG_FATAL("Fail to load %s", _desc.c_str());
        return false;
    }
    tm.stop();
    DL_LOG_TRACE("Loaded %s: %lums (%luns per item)",
                _desc.c_str(),
                tm.m_elapsed(),
                tm.n_elapsed() / never_zero(_table.size()));
    return ret;
}

template <class Table>
bool TableManager<Table>::update(const DynamicRecord &inc_record)
{
    if (NULL == _p_update_strategy) {
        DL_LOG_DEBUG("update strategy of %s is NULL", _desc.c_str());
        return true;
    }
    return _p_update_strategy->update(_table, inc_record);
}

template <class Table>
bool TableManager<Table>::pre_load()
{
    if (NULL == _p_load_strategy) {
        DL_LOG_DEBUG("load strategy of %s is NULL", _desc.c_str());
        return true;
    }
    return _p_load_strategy->pre_load(_table);;
}

template <class Table>
bool TableManager<Table>::post_load()
{
    if (NULL == _p_load_strategy) {
        DL_LOG_DEBUG("load strategy of %s is NULL", _desc.c_str());
        return true;
    }
    return _p_load_strategy->post_load(_table);
}

template <class Table>
bool TableManager<Table>::reload()
{
    if (NULL == _p_load_strategy) {
        DL_LOG_DEBUG("load strategy of %s is NULL", _desc.c_str());
        return true;
    }
    return _p_load_strategy->reload(_table);
}

template <class Table>
void TableManager<Table>::dump_info() const
{
    DL_LOG_WARNING("Table: %s\tsize: %lu\tmem: %lu", 
        desc().c_str(), size(), mem());
}

} //namespace das lib

