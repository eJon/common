// Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
// Author: luanzhaopeng@baidu.com

#include <smalltable2.hpp>

#include "das_lib_log.h"

using configio::DynamicRecord;

namespace das_lib {

template <class Table>
IBaseLoadStrategy<Table>::~IBaseLoadStrategy()
{}

//用于倒排的创建
template <class Table, class Connector, class ConnArg>
ConnectorLoadStrategy<Table, Connector, ConnArg>::ConnectorLoadStrategy(const std::string &desc,
            ConnectorMaker connector_maker,
            TableGroup *p_table_group,
            ConnArg connect_arg)
         : _conn_desc(desc)
         , _p_connector(NULL)
         , _connector_maker(connector_maker)
         , _connect_arg(connect_arg)
         , _p_table_group(p_table_group)
{

}

template <class Table, class Connector, class ConnArg>
ConnectorLoadStrategy<Table, Connector, ConnArg>::~ConnectorLoadStrategy()
{
    if (NULL != _p_connector) {
        delete _p_connector;
        _p_connector = NULL;
    }
}

//inverted tables use connectors to load themselves
template <class Table, class Connector, class ConnArg>
bool ConnectorLoadStrategy<Table, Connector, ConnArg>::load(Table&)
{
    return true;
}

template <class Table, class Connector, class ConnArg>
bool ConnectorLoadStrategy<Table, Connector, ConnArg>::post_load(Table &table)
{
    enable_connector(table);
    return true;
}

template <class Table, class Connector, class ConnArg>
bool ConnectorLoadStrategy<Table, Connector, ConnArg>::pre_load(Table &table)
{
    disable_connector(table);
    return true;
}

template <class Table, class Connector, class ConnArg>
bool ConnectorLoadStrategy<Table, Connector, ConnArg>::reload(Table&)
{
    return true;
}

template <class Table, class Connector, class ConnArg>
bool ConnectorLoadStrategy<Table, Connector, ConnArg>::init(Table &table)
{
    if (NULL == _connector_maker || NULL == _p_table_group) {
        DL_LOG_FATAL("something is NULL [%p/%p]", _connector_maker, _p_table_group);
        return false;
    }

    _p_connector = _connector_maker(table, *_p_table_group, _connect_arg);
    if (NULL == _p_connector) {
        DL_LOG_FATAL("fail to allocate [%s]", _conn_desc.c_str());
        return false;
    }

    show_connector();
    return true;
}

template <class Table, class Connector, class ConnArg>
void ConnectorLoadStrategy<Table, Connector, ConnArg>::show_connector() const 
{
    std::ostringstream oss;
    oss.str("");
    oss << c_show(Connector);
    DL_LOG_TRACE("%s: %s", _conn_desc.c_str(), oss.str().c_str());
}

template <class Table, class Connector, class ConnArg>
void ConnectorLoadStrategy<Table, Connector, ConnArg>::enable_connector(Table &table)
{
    if (NULL == _p_connector) {
        DL_LOG_FATAL("%s has not been initialized yet", _conn_desc.c_str());
        return;
    }

    st::Timer tm;
    
    tm.start();
    _p_connector->refresh();
    _p_connector->enable_observers();
    
    tm.stop();
    DL_LOG_TRACE("Enable %s connector", _conn_desc.c_str());
    DL_LOG_TRACE("Refreshed %s(%s): %lums (%luns per item)",
            _conn_desc.c_str(),
            "DESC to be filled",
            tm.m_elapsed(),
            tm.n_elapsed() / never_zero(table.size()));
}

template <class Table, class Connector, class ConnArg>
void ConnectorLoadStrategy<Table, Connector, ConnArg>::disable_connector(Table& )
{
    if (NULL == _p_connector) {
        DL_LOG_FATAL("%s has not been initialized yet", _conn_desc.c_str());
        return;
    }

    DL_LOG_TRACE("Disable %s connector(%s)", _conn_desc.c_str(), "DESC to be filled");
    _p_connector->disable_observers();
}

template <class Table, class Connector, class ConnArg>
ConnectorLoadStrategy<Table, Connector, ConnArg>::ConnectorLoadStrategy(const ConnectorLoadStrategy &rhs)
    : IBaseLoadStrategy<Table>(rhs)
    , _conn_desc(rhs._conn_desc)
    , _p_connector(NULL)
    , _connector_maker(rhs._connector_maker)
    , _connect_arg(rhs._connect_arg)
    , _p_table_group(NULL)
{
    //note that we should not copy _p_connector and _p_table_group here;
}

template <class Table, class Connector, class ConnArg>
ConnectorLoadStrategy<Table, Connector, ConnArg> *
ConnectorLoadStrategy<Table, Connector, ConnArg>::clone(TableGroup *p_table_group) const
{
    if (NULL == p_table_group) {
        DL_LOG_FATAL("p_table_group is NULL");
        return NULL;
    }

    ConnectorLoadStrategy<Table, Connector, ConnArg> *p_new_strategy = 
        new (std::nothrow) ConnectorLoadStrategy<Table, Connector, ConnArg>(*this);

    if (NULL == p_new_strategy) {
        DL_LOG_FATAL("fail to allocate %s", _conn_desc.c_str());
        return NULL;
    }

    p_new_strategy->_p_table_group = p_table_group;

    return p_new_strategy;
}

template <class Table>
FileLoadStrategy<Table>::FileLoadStrategy(const PartitionArg *p_part_arg,
                                                           PreLoader pre_loader,
                                                           Loader loader,
                                                           PostLoader post_loader,
                                                           cm_utility::FileWatcher &file_watcher,
                                                           TableGroup *p_table_group
                                                           ) 
    : _p_part_arg(p_part_arg)
    , _pre_load_handler(pre_loader)
    , _load_handler(loader)
    , _post_load_handler(post_loader)
    , _file_watcher(file_watcher)
    , _p_table_group(p_table_group)
{
}

template <class Table>
bool FileLoadStrategy<Table>::init(Table &)
{
    return true;
}

template <class Table>
bool FileLoadStrategy<Table>::pre_load(Table &)
{
    if (NULL == _pre_load_handler) {
        DL_LOG_DEBUG("no pre load handler");
        return true;
    }
    if (NULL == _p_table_group) {
        DL_LOG_FATAL("_p_table_group is NULL");
        return false;
    }

    bool ret = _pre_load_handler(*_p_table_group);
    if (!ret) {
        DL_LOG_FATAL("fail to pre load");
        return false;
    }
    
    return true;
}

template <class Table>
bool FileLoadStrategy<Table>::load(Table &table)
{
    if (NULL == _load_handler) {
        DL_LOG_FATAL("_load_handler is NULL");
        return false;
    }
    
    bool ret = _load_handler(table, _file_watcher.filepath(), _p_part_arg);
    if (!ret) {
        DL_LOG_FATAL("fail to load table from %s", _file_watcher.filepath());
        return false;
    }

    return true;
}

template <class Table>
bool FileLoadStrategy<Table>::post_load(Table &)
{
    if (NULL == _post_load_handler) {
        DL_LOG_DEBUG("no post load handler");
        return true;
    }
    if (NULL == _p_table_group) {
        DL_LOG_FATAL("_p_table_group is NULL");
        return false;
    }

    bool ret = _post_load_handler(*_p_table_group);
    if (!ret) {
        DL_LOG_FATAL("fail to post load");
        return false;
    }
    
    return true;
}

template <class Table>
bool FileLoadStrategy<Table>::reload(Table &table)
{
    if (_file_watcher.is_timestamp_updated() <= 0) {
        DL_LOG_TRACE("no update of file %s", _file_watcher.filepath());
        return true;
    }

    DL_LOG_WARNING("Detected that %s is updated, start to reload table",
                   _file_watcher.filepath());
    
    bool ret = pre_load(table);
    if (!ret) {
        DL_LOG_FATAL("fail to pre load");
        return false;
    }

    ret = load(table);
    if (!ret) {
        DL_LOG_FATAL("fail to load");
        return false;
    }

    ret = post_load(table);
    if (!ret) {
        DL_LOG_FATAL("fail to post load");
        return false;
    }

    _file_watcher.update_timestamp();
    
    return true;
}

template <class Table>
FileLoadStrategy<Table>::FileLoadStrategy(const FileLoadStrategy &rhs)
    : IBaseLoadStrategy<Table>(rhs)
    , _p_part_arg(rhs._p_part_arg)
    , _pre_load_handler(rhs._pre_load_handler)
    , _load_handler(rhs._load_handler)
    , _post_load_handler(rhs._post_load_handler)
    , _file_watcher(rhs._file_watcher)
    , _p_table_group(NULL)
{
    //note that we should not copy _p_table_group here and _is_first_load should be false;
}

template <class Table>
FileLoadStrategy<Table> *
FileLoadStrategy<Table>::clone(TableGroup *p_table_group) const
{
    FileLoadStrategy<Table> *p_new_strategy =
        new (std::nothrow) FileLoadStrategy<Table>(*this);
    if (NULL == p_new_strategy) {
        DL_LOG_FATAL("fail to allocate new FileLoadStrategy");
        return NULL;
    }

    p_new_strategy->_p_table_group = p_table_group;

    return p_new_strategy;
}

template <class Table>
IBaseUpdateStrategy<Table>::~IBaseUpdateStrategy()
{}

template <class Table>
bool IncUpdateStrategy<Table>::init(Table&)
{
    if (NULL == _update_handler) {
        DL_LOG_FATAL("_update_handler is NULL");
        return false;
    }

    return true;
}

template <class Table>
bool IncUpdateStrategy<Table>::update(Table &table, const DynamicRecord &record)
{
    if (NULL == _update_handler) {
        DL_LOG_FATAL("_update_handler is NULL");
        return false;
    }

    return _update_handler(table, record);
}

template <class Table>
IncUpdateStrategy<Table> *
IncUpdateStrategy<Table>::clone() const
{
    return new (std::nothrow) IncUpdateStrategy<Table>(*this);
}

} //namespace
