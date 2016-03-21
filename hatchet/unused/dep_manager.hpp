// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
#pragma once

#ifndef _DEP_MANAGER_H_
#define _DEP_MANAGER_H_

#include <vector>
#include <bsl/string.h>
#include "graph.hpp"

namespace st {

/**
 * The base class for dependency action. It will handle inc data when ready
 * 
 */
class DepAction {
public:
    /**
     * Do action when inc event is ready
     */
    virtual int do_action() = 0; 
    /**
     * Do loading when reload event is ready
     */
    virtual int do_loading() = 0; 
    /**
     * Return the associated table name. 
     * @return string the name of the table that this action will modified
     */
    virtual bsl::string get_assoc_table_name() = 0;
    /**
     * Return the dependent table name. 
     * @return vector<string> the name array of the tables that this action will dependent on
     */
    virtual std::vector<bsl::string> get_dep_tables_name() const = 0;
};

/**
 * Dependent manager Class. It will handle the dependent relationship between tables
 * 
 */
class DepManager {
public:
    /**
     * ACTION result constant
     */
    enum ACTION_RESULT {SUCCESS = 0, HAS_CYCLE, MISS_ACTION};

    /**
     * Default constructor
     * 
     */
    DepManager();

    /**
     * Constructor
     * 
     * @param action_size the recommended count of tables that this object will managed. 
     *                    It will be used to construct a map to store all tables. 
     */
    DepManager(size_t action_size);

    /**
     * Register DepAction object in this manager
     * @param action The pointer to the action object
     * 
     * @see DepAction
     *
     * @return bool True when register is successful. False if there is something wrong happened. 
     * 
     */
    bool register_action(DepAction *action);

    /**
     * Build the relationship after registed all actions
     *
     * @return int SUCCESS when building succeed. HAS_CYCLE if the graph has cycle. 
     *               MISS_ACTION when a certain table can not find associated action
     * 
     */
    int build_relationship();
    /**
     * Do loading when reload event is ready
     * 
     * @see DepAction
     * @see DepManager::ACTION_RESULT
     *
     */
    int do_loadings();

    /**
     * Do actions when inc event is ready
     * 
     * @see DepAction
     * @see DepManager::ACTION_RESULT
     *
     * 
     */
    int do_actions();

    /**
     * Write content to string builder
     * 
     */
    void to_string(StringWriter& sb) const;

   /** 
    * A template method to custimize actions. 
    * @param Func a functor object to be called when each event is triggered
    * 
    * @see do_actions()
    *
    * @return int the result when call action in each action
    */
    template <typename Func>
    int do_actions(Func f);

private:
    bool make_dep(const bsl::string &from, const bsl::string &to);
private:
    graph_t<bsl::string> _graph;
    bsl::list<DepAction*> _sequence;
    bsl::hashmap<bsl::string, DepAction*> _action_map;
    size_t _action_size;
};
}
#endif /* _DEP_MANAGER_H_ */
