// Copyright(c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// A container hold multiple different version of the same table
// Author: yufan@baidu.com
// Date: May 4 16:01:44 CST 2011
#pragma once
#ifndef _TABLE_STACK_HPP_
#define _TABLE_STACK_HPP_
#include "cow_table.hpp"
#include "c_connector.hpp"
#include <map>

namespace st {

template <typename _TABLE> 
class TableStack
{
private:
    typedef TableStack<_TABLE> Self;
public:
    struct table_desc
    {
        _TABLE * table;
        int sid;
    };
    typedef unsigned int table_id;
    typedef _TABLE table_type;
    _TABLE * add_table(table_id n)
    {
        if(tables_.find(n) ==  tables_.end()) {
            tables_[n].table = new _TABLE();
            tables_[n].table->init();
            tables_[n].sid = random();
            return tables_[n].table;
        }
        else {
            return tables_[n].table;
        }
    }
    TableStack() 
    {
    }
    TableStack(const Self & ref)
    {
        t2t_ = ref.t2t_;
        default_id =  ref.default_id;
        for(typename std::map<table_id, table_desc>::const_iterator i = ref.tables_.begin();
                i != ref.tables_.end(); 
                i++){
            tables_[i->first].table = new _TABLE(*i->second.table);
            tables_[i->first].sid = i->second.sid;
        }
    }
    Self & operator= (const Self & ref)
    {
        t2t_ = ref.t2t_;
        default_id =  ref.default_id;
        for(typename std::map<table_id, table_desc>::const_iterator i = tables_.begin(); 
                i != tables_.end(); 
                i++){
            delete i->second.table;
        }
        for(typename std::map<table_id, table_desc>::const_iterator i = ref.tables_.begin();
                i != ref.tables_.end(); 
                i++){
            tables_[i->first].table = new _TABLE(*i->second.table);
            tables_[i->first].sid = i->second.sid;
        }
        return *this;
    }
    ~TableStack()
    {
        for(typename std::map<table_id, table_desc>::iterator i = tables_.begin(); i != tables_.end(); i++){
            delete i->second.table;
        }
    }
    bool is_t2t() const
    {
        return t2t_;
    }
    void set_t2t() 
    {
        t2t_ = true;
    }
    template<typename T>
    void make_t2t(T & that)
    {
        t2t_ = true;
        for(typename T::iterator i = that.begin(); i != that.end(); i++){
            table_desc * t = get_table_desc(i->first);
            t->sid = i->second.sid;
        }
        that.set_t2t();
    }
    int set_default(table_id n)
    {
        if(tables_.find(n) != tables_.end()) {
            default_id = n;
            return 0;
        }
        else {
            return ENOTEXIST;
        }
    }
    _TABLE * get_default()
    {
        if(tables_.size() == 1) {
            return tables_.begin()->second.table;
        }
        else {
            return get_table(default_id);
        }
    }
    const _TABLE * get_default() const
    {
        if(tables_.size() == 1) {
            return tables_.begin()->second.table;
        }
        else {
            return get_table(default_id);
        }
    }
    _TABLE * get_table(table_id n)
    {
        iterator i = tables_.find(n);
        if(i != tables_.end()) {
            return i->second.table;
        }
        else {
            return NULL;
        }
    }
    const _TABLE * get_table(table_id n) const
    {
        const_iterator i = tables_.find(n);
        if(i != tables_.end()) {
            return i->second.table;
        }
        else {
            return NULL;
        }
    }
    _TABLE * get_table_sid(int n)
    {
        for(iterator i = tables_.begin(); 
                i != tables_.end();
                i++) {
            if(i->second.sid == n)
            {
                return i->second.table;
            }
        }
        return NULL;
    }
    const _TABLE * get_table_sid(int n) const
    {
        for(const_iterator i = tables_.begin(); 
                i != tables_.end();
                i++) {
            if(i->second.sid == n)
            {
                return i->second.table;
            }
        }
        return NULL;
    }
    table_desc * get_table_desc(table_id n)
    {
        if(tables_.find(n) != tables_.end()) {
            return &tables_[n];
        }
        else {
            return NULL;
        }
    }
    int init()
    {
        /* init within add_table
        for(iterator i = tables_.begin(); 
                i != tables_.end();
                i++) {
            if(i->second.table->init() != 0) {
                return E_GENERAL;
            }
        }
        */
        return 0;
    }
    size_t mem() const
    {
        unsigned int m = 0;
        for(const_iterator i = tables_.begin(); 
                i != tables_.end();
                i++) {
            m += i->second.table->mem();
        }
        return m;
    }
    typedef typename std::map<table_id, table_desc>::iterator iterator;
    typedef typename std::map<table_id, table_desc>::const_iterator const_iterator;
    iterator begin()
    {
        return tables_.begin();
    }
    iterator end()
    {
        return tables_.end();
    }
    const_iterator begin() const
    {
        return tables_.begin();
    }
    const_iterator end() const
    {
        return tables_.end();
    }
    // Return the number of tables in the stack
    unsigned int count() const
    {
        return tables_.size();
    }
    void print()
    {
        for(iterator i = tables_.begin(); 
                i != tables_.end();
                i++) {
            std::cout << "id:" << i->first << " sid:" << i->second.sid << std::endl;
        }
    }
    void clear()
    {
        for(iterator i = tables_.begin(); 
                i != tables_.end();
                i++) {
            i->second.table->clear();
        }
    }
    void resize (size_t n_bucket2)
    {
        for(iterator i = tables_.begin(); 
                i != tables_.end();
                i++) {
            i->second.table->resize(n_bucket2);
        }
    }
    // Return the size count of all tables in the stack
    size_t size () const
    {
        size_t s = 0;
        for(const_iterator i = tables_.begin(); 
                i != tables_.end();
                i++) {
            s += i->second.table->size();
        }
        return s;
    }
private:
    bool t2t_;
    table_id default_id;
    std::map<table_id, table_desc> tables_;
};


template <typename CONNECTOR>
class ConnectorStack
{
    typedef CONNECTOR ConnectorType;
    typedef typename ConnectorType::DstTableType DstTableType;
    TableStack<DstTableType> * dst_stack;
    std::vector<ConnectorType *> connector_stack_;
    typename ConnectorType::TablePtrTup tmptup;
    std::vector<int> tmpsid;
    template <typename Table, typename Tup, int N>
    struct insert
    {
        static void call(ConnectorStack<CONNECTOR> & that, Tup & tup, int sid)
        {                                                              
            that.tmptup.template at_n<N - 1>() = tup.template at_n<N - 1>();
            typedef TCAP(content_type_of, typename Tup::template type_at_n<N - 2>::R) Tb;
            insert<Tb, Tup, N - 1>::call(that, tup, sid);
        }
    };
    template <typename Table, typename Tup>
    struct insert<Table, Tup, 1>
    {
        static void call(ConnectorStack<CONNECTOR> & that, Tup & tup, int sid)
        {
            that.tmptup.template at_n<0>() = tup.template at_n<0>();
            DstTableType * dst = that.dst_stack->add_table(sid);   
            if(dst->init()) {
                ST_FATAL("init table failed");
            }
            ConnectorType * con =  new ConnectorType(dst, &that.tmptup);
            that.connector_stack_.push_back(con);
        }
    };
    template <typename T, typename Tup>
    struct insert<TableStack<T>, Tup, 1>
    {
        static void call(ConnectorStack<CONNECTOR> & that, Tup & tup, int sid)
        {
            const TableStack<T> * t = tup.template at_n<0>();
            if(t->is_t2t()) {
                for(unsigned int i = 0; i < that.tmpsid.size(); i++)
                {
                    const T * c = t->get_table_sid(that.tmpsid[i]);
                    if(c) {
                        that.tmptup.template at_n<0>() = c;
                        DstTableType * dst = that.dst_stack->add_table(sid); 
                        if(dst->init()) {
                            ST_FATAL("init table failed");
                        }
                        ConnectorType * con =  new ConnectorType(dst, &that.tmptup);
                        that.connector_stack_.push_back(con);
                        return;
                    }
                }
            }
            for(typename TableStack<T>::const_iterator i =  tup.template at_n<0>()->begin();
                    i != tup.template at_n<0>()->end();
                    i++)
            {
                that.tmptup.template at_n<0>() = i->second.table;
                DstTableType * dst = that.dst_stack->add_table(sid ^ i->second.sid);   
                if(dst->init()) {
                    ST_FATAL("init table failed");
                }
                ConnectorType * con =  new ConnectorType(dst, &that.tmptup);
                that.connector_stack_.push_back(con);
            }
        }
    };
    template <typename T, typename Tup, int N>
    struct insert<TableStack<T>, Tup, N>
    {
        static void call(ConnectorStack<CONNECTOR> & that, Tup & tup, int sid)
        {
            typedef TCAP(content_type_of, typename Tup::template type_at_n<N - 2>::R) T1;
            const TableStack<T> * t = tup.template at_n<N - 1>();
            if(t->is_t2t()) {
                for(unsigned int i = 0; i < that.tmpsid.size(); i++)
                {
                    const T * c = t->get_table_sid(that.tmpsid[i]);
                    if(c) {
                        that.tmptup.template at_n<N - 1>() = c;
                        insert<T1, Tup, N - 1>::call(that, tup, sid);
                        return;
                    }
                }
            }
            for(typename TableStack<T>::const_iterator i =  t->begin();
                i != t->end();
                i++)
            {
                that.tmpsid.push_back(i->second.sid);
                that.tmptup.template at_n<N - 1>() = i->second.table;
                insert<T1, Tup, N - 1>::call(that, tup, sid ^ i->second.sid);
                that.tmpsid.pop_back();
            }
        }
    };
public:
#define DEFINE_CONNECTORSTACK_VARIADIC_CONSTRUCTOR(_n_)                 \
    template <ST_SYMBOLL##_n_(typename _T)>                             \
    ConnectorStack (TableStack<DstTableType>* p_dst_table, ST_PARAML##_n_(const _T, *a))    \
    {\
        dst_stack = p_dst_table; \
        typedef BasicTuple<TCAP(make_list, ST_SYMBOLL##_n_##P(const _T))> Tup;\
        Tup tup;\
        init_tuple (&tup, ST_SYMBOLL##_n_(a)); \
        typedef TCAP(content_type_of, typename Tup::template type_at_n<_n_ - 1>::R) T; \
        insert<T, Tup, _n_>::call(*this, tup, 0); \
    }
    typedef ConnectorStack<CONNECTOR> Self;
public:
    DEFINE_CONNECTORSTACK_VARIADIC_CONSTRUCTOR(1)
    DEFINE_CONNECTORSTACK_VARIADIC_CONSTRUCTOR(2)
    DEFINE_CONNECTORSTACK_VARIADIC_CONSTRUCTOR(3)
    DEFINE_CONNECTORSTACK_VARIADIC_CONSTRUCTOR(4)
    DEFINE_CONNECTORSTACK_VARIADIC_CONSTRUCTOR(5)
    DEFINE_CONNECTORSTACK_VARIADIC_CONSTRUCTOR(6)
    DEFINE_CONNECTORSTACK_VARIADIC_CONSTRUCTOR(7)
    DEFINE_CONNECTORSTACK_VARIADIC_CONSTRUCTOR(8)
    ConnectorStack(const Self & ref)
    {
        dst_stack = ref.dst_stack;
        connector_stack_ = ref.connector_stack_;
        for(int i=0; i< connector_stack_; i++){
            connector_stack_[i] = new ConnectorType(*connector_stack_[i]);
        }
    }
    void disable_observers ()
    {
        for(unsigned int i=0; i< connector_stack_.size(); i++){
            connector_stack_[i]->disable_observers();
        }
    }
    void enable_observers ()
    {
        for(unsigned int i=0; i< connector_stack_.size(); i++){
            connector_stack_[i]->enable_observers();
        }
    }
    
    void refresh ()
    {
        for(unsigned int i=0; i< connector_stack_.size(); i++){
            connector_stack_[i]->refresh();
        }
    }

    void connect ()
    {
        for(unsigned int i=0; i< connector_stack_.size(); i++){
            connector_stack_[i]->connector();
        }
    }
#define DEFINE_CONNECTORSTACK_VARIADIC_CONNECT(_n_)                   \
    template <ST_SYMBOLL##_n_(typename _T)>                           \
    void connect (ST_PARAML##_n_(const _T, & a))                      \
    {                                                                 \
        for(unsigned int i=0; i< connector_stack_.size(); i++){       \
            connector_stack_[i]->connect(ST_SYMBOLL##_n_(a));        \
        }                                                             \
    }                                                                 \

    DEFINE_CONNECTORSTACK_VARIADIC_CONNECT(1)
    DEFINE_CONNECTORSTACK_VARIADIC_CONNECT(2)
    DEFINE_CONNECTORSTACK_VARIADIC_CONNECT(3)
    DEFINE_CONNECTORSTACK_VARIADIC_CONNECT(4)

};


}

#endif // _TABLE_STACK_HPP_
