// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Inline implementations of c_connector.hpp
// Author: gejun@baidu.com
// Date: Dec.7 14:10:24 CST 2010

namespace st {

template <int _TABLE_POS, class _Table, class _TableM, class _MaybeM,
          class _DomG, class _FPredL, class _VarTup, class _TablePtrTup,
          class _VPredL>
void TrigSelector<_TABLE_POS, _Table, _TableM, _MaybeM, _DomG, _FPredL,
                  _VarTup, _TablePtrTup, _VPredL>::
c_to_string (std::ostream& os)
{ os << "T" << _TABLE_POS << "=>" << c_show(TSL); }

template <class _DstTable, class _Pick, class _From, class _Where>
void Connector<_DstTable, _Pick, _From, _Where>::c_to_string (std::ostream& os)
{
    os << "Connector{" << std::endl
       << "TSL: " << c_show(TSL) << std::endl
       << "TrigSels: " << c_show(TrigSelL) << std::endl
       << "From: " << c_show(SrcTableL) << std::endl
       << "Where: " << c_show(class _Where::PredL) << std::endl
       << '}';
}

template <class _DstTable, class _Pick, class _From, class _Where>
class Connector<_DstTable, _Pick, _From, _Where>::DefaultDstTupMaker {
public:
    template <class _Receiver, class _IterTup> void call
    (const _Receiver& recv, const _IterTup& iter_tup, const VarTup& var_tup)
    {
        DstTup tup;
        instantiate_tuple<class DstTup::AttrS, DstTup,
            TCAP(list_map, attr_to_pos, class _Pick::R),
            _IterTup, VarTup>::call(&tup, iter_tup, var_tup);
        recv(tup);
    }
};

template <class _DstTable, class _Pick, class _From, class _Where>
class Connector<_DstTable, _Pick, _From, _Where>::UserDstTupMaker {
public:
    template <class _Receiver, class _IterTup> void call
    (const _Receiver& recv, const _IterTup& iter_tup, const VarTup& var_tup)
    { user_pick_(recv, SelectResult<_IterTup, VarTup>(&iter_tup, &var_tup)); }

    _Pick user_pick_;  // user's function object
};

template <class _DstTable, class _Pick, class _From, class _Where>
template <class _TrigSel>
class Connector<_DstTable, _Pick, _From, _Where>::InsertObserver
    : public BaseObserver<const class _TrigSel::Table::Tup&> {
    typedef class _TrigSel::Table::Tup Tup_;
public:
    void on_event (const Tup_& tup)
    {
        class _TrigSel::Iterator it;

        it.template set_sub_iterator<_TrigSel::TABLE_POS>(&tup);
        it.init(p_trig_->var_tup_, &p_trig_->table_tup_);
        for ( ; it; ++it) {
            p_trig_->tup_maker_.call(
                CowTableInserter<_DstTable>(p_trig_->p_dst_table_),
                it.tuple_of_sub_iterators(), p_trig_->var_tup_);
        }
    }
    
    Connector* p_trig_;
};

template <class _DstTable, class _Pick, class _From, class _Where>
template <class _TrigSel>
class Connector<_DstTable, _Pick, _From, _Where>::InsertMaybeObserver
    : public BaseObserver<const class _TrigSel::Table::Tup&> {
    typedef class _TrigSel::Table::Tup Tup_;
    enum { MP0 = _TrigSel::MaybePos::R,
           MP = (MP0 < 0 ? -MP0-1 : MP0) };
    typedef TCAP(list_at, MP, class _TrigSel::Table::IndexL) Index;
public:
    InsertMaybeObserver () { dft_tup_ = ST_DEFAULT_VALUE_OF(Tup_); }
        
    void on_event (const Tup_& tup)
    {
        class _TrigSel::Iterator it;

        // Erase dst tuples associated with default value
        if (p_trig_->table_tup_.template
            at_n<_TrigSel::TABLE_POS>()->template index<MP>().copy_key_part(
                &dft_tup_,
                tup)) {
            it.template set_sub_iterator<_TrigSel::TABLE_POS>(&dft_tup_);
            it.init(p_trig_->var_tup_, &p_trig_->table_tup_);
            for ( ; it; ++it) {
                p_trig_->tup_maker_.call(
                    CowTableEraser<_DstTable>(p_trig_->p_dst_table_),
                    it.tuple_of_sub_iterators(), p_trig_->var_tup_);
            }
        }

        if (MP0 >= 0) {
            // Insert dst tuples associated with the new tuple
            // only for Maybe, Exclude does not need this
            it.template set_sub_iterator<_TrigSel::TABLE_POS>(&tup);
            it.init(p_trig_->var_tup_, &p_trig_->table_tup_);
            for ( ; it; ++it) {
                p_trig_->tup_maker_.call(
                    CowTableInserter<_DstTable>(p_trig_->p_dst_table_),
                    it.tuple_of_sub_iterators(), p_trig_->var_tup_);
            }
        }
    }

    Tup_ dft_tup_;
    Connector* p_trig_;
};


template <class _DstTable, class _Pick, class _From, class _Where>
template <class _TrigSel>
class Connector<_DstTable, _Pick, _From, _Where>::EraseObserver
    : public BaseObserver<const class _TrigSel::Table::Tup&> {
public:
    void on_event (class _TrigSel::Table::Tup const& tup)
    {
        class _TrigSel::Iterator it;

        it.template set_sub_iterator<_TrigSel::TABLE_POS>(&tup);
        it.init(p_trig_->var_tup_, &p_trig_->table_tup_);
        for ( ; it; ++it) {
            p_trig_->tup_maker_.call(
                CowTableEraser<_DstTable>(p_trig_->p_dst_table_),
                it.tuple_of_sub_iterators(), p_trig_->var_tup_);
        }
    }

    Connector* p_trig_;
};

template <class _DstTable, class _Pick, class _From, class _Where>
template <class _TrigSel>
class Connector<_DstTable, _Pick, _From, _Where>::EraseMaybeObserver
    : public BaseObserver<const class _TrigSel::Table::Tup&> {
    typedef class _TrigSel::Table::Tup Tup_;
    enum { MP0 = _TrigSel::MaybePos::R,
           MP = (MP0 < 0 ? -MP0-1 : MP0) };
    typedef TCAP(list_at, MP, class _TrigSel::Table::IndexL) Index;
public:
    EraseMaybeObserver ()
    {
        dft_tup_ = ST_DEFAULT_VALUE_OF(Tup_);
    }

    void on_event (const Tup_& tup)
    {
        class _TrigSel::Iterator it;

        if (MP0 >= 0) {
            // only for Maybe, Exclude does not need this
            it.template set_sub_iterator<_TrigSel::TABLE_POS>(&tup);
            it.init(p_trig_->var_tup_, &p_trig_->table_tup_);
            for ( ; it; ++it) {
                p_trig_->tup_maker_.call(
                    CowTableEraser<_DstTable>(p_trig_->p_dst_table_),
                    it.tuple_of_sub_iterators(), p_trig_->var_tup_);
            }
        }

        // Insert with default value
        if (p_trig_->table_tup_.template
            at_n<_TrigSel::TABLE_POS>()->template index<MP>().copy_key_part(
                &dft_tup_,
                tup)) {
            it.template set_sub_iterator<_TrigSel::TABLE_POS>(&dft_tup_);
            it.init(p_trig_->var_tup_, &p_trig_->table_tup_);
            for ( ; it; ++it) {
                p_trig_->tup_maker_.call(
                    CowTableInserter<_DstTable>(p_trig_->p_dst_table_),
                    it.tuple_of_sub_iterators(), p_trig_->var_tup_);
            }
        }
    }

    Tup_ dft_tup_;
    Connector* p_trig_;
};

template <class _DstTable, class _Pick, class _From, class _Where>
class Connector<_DstTable, _Pick, _From, _Where>::ClearObserver
    : public BaseObserver<> {
public:
    void on_event () { p_trig_->p_dst_table_->clear(); }
    Connector* p_trig_;
};


template <class _DstTable, class _Pick, class _From, class _Where>
void Connector<_DstTable, _Pick, _From, _Where>::refresh ()
{
    // A refresh without clearing is counter-intuitive, although a table
    // conceptually could be targeted by multiple connectors, but in
    // practice, there're still a couple of problems to solve. In short,
    // We do clear in refresh currently
    p_dst_table_->clear();
    
    for (Iterator it(var_tup_, &table_tup_); it; ++it) {
        tup_maker_.call(
            CowTableInserter<_DstTable>(p_dst_table_),
            it.tuple_of_sub_iterators(), var_tup_);
    }
}

// Helper function object to call init() of an observer group
template <class _Connector> struct init_observer_group {
    template <class _ObserverGroup>
    void operator() (_ObserverGroup& og) const { og.init(p_trig_); }
    _Connector* p_trig_;
};

struct enable_observer_group {
    template <class _ObserverGroup>
    void operator() (_ObserverGroup& og) const { og.enable(); }
};

struct disable_observer_group {
    template <class _ObserverGroup>
    void operator() (_ObserverGroup& og) const { og.disable(); }
};

template <class _DstTable, class _Pick, class _From, class _Where>
void Connector<_DstTable, _Pick, _From, _Where>::init_observers ()
{
    init_observer_group<Connector> init_og;                            
    init_og.p_trig_ = this;                                         
    og_tup_.do_map(init_og);
}

template <class _DstTable, class _Pick, class _From, class _Where>
void Connector<_DstTable, _Pick, _From, _Where>::enable_observers ()
{
    og_tup_.do_map(enable_observer_group());
}

template <class _DstTable, class _Pick, class _From, class _Where>
void Connector<_DstTable, _Pick, _From, _Where>::disable_observers ()
{
    og_tup_.do_map(disable_observer_group());
}

template <class _DstTable, class _Pick, class _From, class _Where>
template <class _TrigSel>
class Connector<_DstTable, _Pick, _From, _Where>::make_observer_group {
    enum { N_ = _TrigSel::TABLE_POS };

    struct OrdinaryGroup {
        void init (Connector* p_trig)
        {
            insert_ob_.p_trig_ = p_trig;
            erase_ob_.p_trig_ = p_trig;
            clear_ob_.p_trig_ = p_trig;
            p_table_ = p_trig->table_tup_.template at_n<N_>();
        }

        void enable ()
        {
            p_table_->insert_event.subscribe(&insert_ob_);
            p_table_->erase_event.subscribe(&erase_ob_);
            p_table_->pre_replace_event.subscribe(&erase_ob_);
            p_table_->post_replace_event.subscribe(&insert_ob_);
            p_table_->clear_event.subscribe(&clear_ob_);
        }

        void disable ()
        {
            p_table_->insert_event.unsubscribe(&insert_ob_);
            p_table_->erase_event.unsubscribe(&erase_ob_);
            p_table_->pre_replace_event.unsubscribe(&erase_ob_);
            p_table_->post_replace_event.unsubscribe(&insert_ob_);
            p_table_->clear_event.unsubscribe(&clear_ob_);
        }
        
        InsertObserver<_TrigSel> insert_ob_;
        EraseObserver<_TrigSel> erase_ob_;
        ClearObserver clear_ob_;
        typename TablePtrTup::template type_at_n<N_>::R p_table_;
    };
        
    struct MaybeGroup {
        void init (Connector* p_trig) {
            insert_ob_.p_trig_ = p_trig;
            erase_ob_.p_trig_ = p_trig;
            insert_mob_.p_trig_ = p_trig;
            erase_mob_.p_trig_ = p_trig;
            clear_ob_.p_trig_ = p_trig;
            p_table_ = p_trig->table_tup_.template at_n<N_>();
        }

        void enable ()
        {
            p_table_->insert_event.subscribe(&insert_mob_);
            p_table_->erase_event.subscribe(&erase_mob_);
            p_table_->pre_replace_event.subscribe(&erase_ob_);
            p_table_->post_replace_event.subscribe(&insert_ob_);
            p_table_->clear_event.subscribe(&clear_ob_);
        }

        void disable ()
        {
            p_table_->insert_event.unsubscribe(&insert_mob_);
            p_table_->erase_event.unsubscribe(&erase_mob_);
            p_table_->pre_replace_event.unsubscribe(&erase_ob_);
            p_table_->post_replace_event.unsubscribe(&insert_ob_);
            p_table_->clear_event.unsubscribe(&clear_ob_);
        }
        
        InsertObserver<_TrigSel> insert_ob_;
        EraseObserver<_TrigSel> erase_ob_;
        InsertMaybeObserver<_TrigSel> insert_mob_;
        EraseMaybeObserver<_TrigSel> erase_mob_;
        ClearObserver clear_ob_;
        typename TablePtrTup::template type_at_n<N_>::R p_table_;
    };
public:
    typedef TCAP(if_void, TCAP(map_find, Int<N_>, MaybeM),
                 OrdinaryGroup, MaybeGroup) R;
};


}  // namespace st

