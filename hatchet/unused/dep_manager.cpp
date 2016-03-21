// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
/***************************************************************************
 * 
 * Copyright (c) 2010 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/

// The implementation of dependency manager class
// Author: yanlin@baidu.com
// Date: Thu Aug  5 10:26:40 2010

#include "dep_manager.hpp"

namespace st 
{

#define ACTION_SIZE 43

struct do_action_func_t {
    int operator () (DepAction *da)
    {
        if (da) {
            return da->do_action();
        }
        else {
            ST_WARN("NULL DepAction");
            return -1;
        }
    }
};

struct do_loading_func_t {
    int operator () (DepAction *da)
    {
        if (da) {
            return da->do_loading();
        }
        else {
            ST_WARN("NULL DepAction");
            return -1;
        }
    }
};

DepManager::DepManager(): _action_size(ACTION_SIZE)
{
    _sequence.create();
    _action_map.create(_action_size);
}

DepManager::DepManager(size_t action_size): 
    _action_size(action_size)
{
    _sequence.create();
    _action_map.create(_action_size);
}

bool DepManager::register_action(DepAction *action)
{
    if (NULL == action) {
        ST_WARN("NULL action or NULL table");
        return false;
    }
    if (action->get_assoc_table_name().empty()) {
        ST_WARN("The associated table name is empty");
        return false;
    }

    _action_map.set(action->get_assoc_table_name(), action, 1);
    ST_DEBUG("Map action %p with table %s", action, action->get_assoc_table_name().c_str());
    
    bool succ = true;
    std::vector<bsl::string> names = action->get_dep_tables_name();
    if (!names.empty()) {
        std::vector<bsl::string>::iterator iter = names.begin();
        for (; iter != names.end(); ++iter) {
            if (!make_dep(action->get_assoc_table_name(), *iter)) {
                break;
            }
        }
    }
    else {
       succ = _graph.add_vertex(action->get_assoc_table_name());
    }
    return succ;
}

bool DepManager::make_dep(const bsl::string &from, const bsl::string &to)
{
    if (from.empty() || to.empty()) {
        ST_WARN("The table name is empty, can not build dependency relationship.");
        return false;
    }
    bool result = _graph.add_edge(from, to);
    ST_DEBUG("Table %s depend on table %s", from.c_str(), to.c_str());
    return result;
}

int DepManager::do_loadings()
{
    return do_actions(do_loading_func_t());
}

int DepManager::do_actions()
{
    return do_actions(do_action_func_t());
}

int DepManager::build_relationship()
{
    _sequence.clear();
    bsl::list<bsl::string> result;
    if (!topological_sort(_graph, std::back_inserter(result)))
        return HAS_CYCLE;
    
    bsl::list<bsl::string>::iterator iter = result.begin();
    for (; iter != result.end(); ++iter) {
        DepAction* a;
        if (_action_map.get(*iter, &a) == bsl::HASH_NOEXIST) {
            ST_WARN("Can not find associated action for table %s", iter->c_str());
            return MISS_ACTION;
        }
        _sequence.push_back(a);
    }
    return SUCCESS;
}

template <typename Func>
int DepManager::do_actions(Func f)
{
    bsl::list<DepAction *>::iterator iter;
    int ret = 0;
    for (iter = _sequence.begin(); iter != _sequence.end(); ++iter) {
        ST_DEBUG("Action on table:%s begins", (*iter)->get_assoc_table_name().c_str());
        ret = f(*iter);
        ST_DEBUG("Action on table:%s ends", (*iter)->get_assoc_table_name().c_str());
        if (ret < 0) {
            ST_FATAL("do action return error:%d", ret);
            break;
        }
    }
    return ret;
}

void DepManager::to_string(StringWriter& sb) const
{
    sb << "graphics:\n" << _graph;
}

}
